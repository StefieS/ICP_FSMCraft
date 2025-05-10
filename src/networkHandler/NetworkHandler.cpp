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
void NetworkHandler::listen(int port, FsmController& controller) {
    if (listener) {
        listener->startListening(port, [this, &controller](const std::string& msg, int clientSocket) {
                // Register the first client socket
            {
                std::lock_guard<std::mutex> lock(socketMutex);
                if (firstClientSocket == -1) {
                    firstClientSocket = clientSocket;
                    safePrint("Registered first client socket: " + std::to_string(firstClientSocket));
                }
            }

            // Process message from any client
            Message message(msg);
            Message processed = controller.performAction(message);
            std::string responseStr = processed.toMessageString() + "\r\n"; 
            safePrint(responseStr);
            // Send processed result only to first client
            int targetSocket;
            {
                std::lock_guard<std::mutex> lock(socketMutex);
                targetSocket = firstClientSocket;
            }

            if (targetSocket != -1) {
                ::send(targetSocket, responseStr.c_str(), responseStr.size(), 0);
            }
            
        },
        // Add optional onDisconnect callback
        [this](int clientSocket) {
            std::lock_guard<std::mutex> lock(socketMutex);
            if (clientSocket == firstClientSocket) {
                safePrint("First client disconnected. Resetting slot.");
                firstClientSocket = -1;
            }
        });
    }
}

