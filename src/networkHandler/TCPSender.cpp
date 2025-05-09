#include "NetworkHandler.h"
#include <mutex>

std::mutex sockMutex;  // Mutex to protect socket operations

void TCPSender::setHostAndPort(const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(sockMutex);  // Protect shared resource
    this->host = host;
    this->port = port;
}

bool TCPSender::connectToServer() {
    std::lock_guard<std::mutex> lock(sockMutex);  // Protect socket access

    if (sock != -1) {
        std::cerr << "Already connected. Close the current connection before reusing." << std::endl;
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return false;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &server.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported!" << std::endl;
        closeConnection();
        return false;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        closeConnection();
        return false;
    }

    safePrint("Connected to server: " + host + ":" + std::to_string(port));
    return true;
}

std::string TCPSender::recvMessage() {
    std::lock_guard<std::mutex> lock(sockMutex);  // Protect socket access

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        std::cerr << "Receive failed or connection closed!" << std::endl;
        return "";
    }

    return std::string(buffer, bytesRead);
}

void TCPSender::closeConnection() {
    std::lock_guard<std::mutex> lock(sockMutex);  // Protect socket access

    if (sock != -1) {
        close(sock);
        sock = -1;
        safePrint("Connection closed.");
    }
}

bool TCPSender::sendMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(sockMutex);  // Protect socket access

    if (sock == -1) {
        std::cerr << "Not connected! Call connectToServer first." << std::endl;
        return false;
    }

    ssize_t bytesSent = send(sock, msg.c_str(), msg.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "Send failed: " << strerror(errno) << " (" << errno << ")" << std::endl;
        closeConnection();
        return false;
    }
    safePrint("Message sent from socket " + std::to_string(sock) + ": " + msg);
    return true;
}
