#include "NetworkHandler.h"

void TCPSender::setHostAndPort(const std::string& host, int port) {
    this->host = host;
    this->port = port;
}

void TCPSender::connectToServer() {
    if (sock != -1) {
        std::cerr << "Already connected. Close the current connection before reusing." << std::endl;
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &server.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported!" << std::endl;
        closeConnection();
        return;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed!" << std::endl;
        closeConnection();
        return;
    }

    std::cout << "Connected to server: " << host << ":" << port << std::endl;
}

std::string TCPSender::recvMessage() {
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
    if (sock != -1) {
        close(sock);
        sock = -1;
        std::cout << "Connection closed." << std::endl;
    }
}

bool TCPSender::sendMessage(const std::string& msg) {
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

    std::cout << "Message sent: " << msg << std::endl;
    return true;
}


std::string SimpleParser::parseMessage(const std::string& msg) {
    return msg;  // Just return the message as is
}
