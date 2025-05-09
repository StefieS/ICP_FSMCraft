/**
*  @file NetworkHandler.cpp
 * @brief Implementation file for Networking aspects of this project
 * @author xlesigm00
 * @date 05.05.2025
 */

#include "NetworkHandler.h"
#include <memory>

std::mutex coutMutex;

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
void NetworkHandler::connectToServer() {
    if (sender) {
        sender->connectToServer();
    }
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
        listener->startListening(port, [this](const std::string& msg, int clientSocket) {
                // Register the first client socket
            {
                std::lock_guard<std::mutex> lock(socketMutex);
                if (firstClientSocket == -1) {
                    firstClientSocket = clientSocket;
                    safePrint("Registered first client socket: " + std::to_string(firstClientSocket));
                }
            }

            // Only send a response to the first client
            int targetSocket = firstClientSocket;
            if (targetSocket == clientSocket) {
                std::string response = "Echo: " + msg;
                ::send(targetSocket, response.c_str(), response.size(), 0);
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

