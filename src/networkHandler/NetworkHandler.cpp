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
        std::vector<int> connectedClients;
        listener->startListening(port, [this, &controller, &connectedClients](const std::string& msg, int clientSocket) {
                // Register the client socket
            {
                std::lock_guard<std::mutex> lock(socketMutex);
                // Add the new client to the list of connected clients
                connectedClients.push_back(clientSocket);
                safePrint("Registered client: " + std::to_string(clientSocket));
            }

            // Process message from the client
            Message message(msg);
            Message processed = controller.performAction(message);
            std::string responseStr = processed.toMessageString() + "\r\n"; 
            safePrint(responseStr);
            
            // Send the response to all connected clients
            {
                std::lock_guard<std::mutex> lock(socketMutex);
                for (int targetSocket : connectedClients) {
                    int result = ::send(targetSocket, responseStr.c_str(), responseStr.size(), 0);
                    if (result <= 0) {
                        // If the send fails (or the socket is closed), reset that socket
                        safePrint("Failed to send to client " + std::to_string(targetSocket) + ", removing from list.");
                        connectedClients.erase(std::remove(connectedClients.begin(), connectedClients.end(), targetSocket), connectedClients.end());
                    }
                }
            }

            // If the message is a STOP type, stop the FSM
            Message isStop(responseStr);
            if (isStop.getType() == EMessageType::STOP) {
                controller.getFsm()->stop();
            }

        },
        // Add optional onDisconnect callback
        [this, &connectedClients](int clientSocket) {
            std::lock_guard<std::mutex> lock(socketMutex);
            // Remove the client from the connectedClients list when they disconnect
            auto it = std::find(connectedClients.begin(), connectedClients.end(), clientSocket);
            if (it != connectedClients.end()) {
                connectedClients.erase(it);
                safePrint("Client " + std::to_string(clientSocket) + " disconnected.");
            }
        });
    }
}


