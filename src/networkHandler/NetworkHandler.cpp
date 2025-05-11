/**
*  @file NetworkHandler.cpp
 * @brief Implementation file for Networking aspects of this project
 * @author xlesigm00
 * @date 05.05.2025
 */

#include "NetworkHandler.h"
#include "../messages/Message.h"
#include <memory>
#include "../controllers/fsmController/FsmController.h"

std::mutex coutMutex;
std::mutex responseMutex;

void safePrint(const std::string& msg) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << msg << std::endl;
}
// NetworkHandler constructor
NetworkHandler::NetworkHandler(const std::string& host, int port)
    : host(host), port(port) {
    // Create instances of SimpleParser, TCPListener, and TCPSender
    this->listener = std::unique_ptr<TCPListener>(new TCPListener());
    this->sender = std::unique_ptr<TCPSender>(new TCPSender());
    
    // Set the host and port for the sender
    dynamic_cast<TCPSender*>(this->sender.get())->setHostAndPort(host, port);
}

// Connect to the server
bool NetworkHandler::connectToServer() {
    if (sender) {
       return sender->connectToServer();
    }
    return false;
}

std::string NetworkHandler::recvFromHost() {
    if (sender) {
        return sender->recvMessage();  // Get response from the server
    }
    return "";
}

// Close the connection
void NetworkHandler::closeConnection() {
    if (sender) {
        sender->closeConnection();
    }
}

// Send a message to the connected host
void NetworkHandler::sendToHost(const std::string& msg) {
    if (sender) {
        sender->sendMessage(msg);
    }
}

// Listen for incoming messages
void NetworkHandler::listen(int port) {
    if (listener) {
        FsmController controller;
        // Use a shared mutex to protect connectedClients
     // List to track client sockets

        listener->startListening(port, [this, &controller](const std::string& msg, int clientSocket) {
            {
                // Lock the mutex to safely modify the list of connected clients
                std::lock_guard<std::mutex> lock(socketMutex);
                connectedClients.push_back(clientSocket);  // Add the new client socket
                safePrint("Registered client: " + std::to_string(clientSocket));
            }

            // Process the incoming message
            Message message(msg);
            Message processed = controller.performAction(message);
            std::string responseStr = processed.toMessageString() + "\r\n";
            safePrint("Response: " + responseStr);

            // Send the response to all connected clients
            {
                // Lock the mutex to ensure thread-safety while modifying the list of clients
                std::lock_guard<std::mutex> lock(socketMutex);
                std::vector<int> failedSockets;  // To store clients that failed to receive the message

                // Send response to all connected clients
                for (int targetSocket : connectedClients) {
                    int result = ::send(targetSocket, responseStr.c_str(), responseStr.size(), 0);
                    if (result <= 0) {
                        safePrint("Failed to send to client " + std::to_string(targetSocket));
                        failedSockets.push_back(targetSocket);  // Mark this socket for removal
                    }
                }

                // Remove clients that failed to receive the message
                for (int failedSocket : failedSockets) {
                    connectedClients.erase(std::remove(connectedClients.begin(), connectedClients.end(), failedSocket), connectedClients.end());
                    safePrint("Removed client " + std::to_string(failedSocket) + " due to send failure.");
                }
            }

            // If the message type is STOP, stop the FSM
            Message isStop(responseStr);
            if (isStop.getType() == EMessageType::STOP) {
                controller.getFsm()->stop();
            }
        },
        // Optional onDisconnect callback
        [this](int clientSocket) {
            // Lock the mutex to safely remove the disconnected client from the list
            std::lock_guard<std::mutex> lock(socketMutex);
            auto it = std::find(connectedClients.begin(), connectedClients.end(), clientSocket);
            if (it != connectedClients.end()) {
                connectedClients.erase(it);  // Remove the client from the list
                safePrint("Client " + std::to_string(clientSocket) + " disconnected.");
            }
        });
    }
}



