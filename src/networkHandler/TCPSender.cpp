/**
 * @file TCPSender.cpp
 * @brief Implementation of the TCPSender class.
 * @author xlesigm00
 * @date 05.05.2025
 */

#include "NetworkHandler.h"
#include <mutex>

/**
 * @brief Mutex to protect socket operations.
 */
std::mutex sockMutex;  /**< Mutex for protecting socket access. */
std::mutex sockMutex2; /**< Mutex for additional socket operations. */
std::mutex readMutex;  /**< Mutex for protecting read operations. */

/**
 * @brief Sets the host and port for the TCP connection.
 * 
 * This method updates the host and port values and locks the mutex to ensure
 * thread safety when setting the host and port values.
 * 
 * @param host The host address to connect to.
 * @param port The port number to connect to.
 */
void TCPSender::setHostAndPort(const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(sockMutex);  /**< Protect shared resource */
    this->host = host;
    this->port = port;
}

/**
 * @brief Connects to the server using the set host and port.
 * 
 * This method attempts to create a socket and connect to the server. It
 * handles socket creation errors, address validation, and connection failures.
 * 
 * @return true if the connection is successful, false otherwise.
 */
bool TCPSender::connectToServer() {
    std::lock_guard<std::mutex> lock(sockMutex);  /**< Protect socket access */

    if (sock != -1) {
        std::cerr << "Already connected. Close the current connection before reusing." << std::endl;
        return false;
    }

    this->sock = socket(AF_INET, SOCK_STREAM, 0);
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

/**
 * @brief Receives a message from the server.
 * 
 * This method reads from the server socket until it finds a newline character
 * and returns the message. It ensures thread-safety with a mutex lock and
 * handles socket errors and connection issues.
 * 
 * @return The received message from the server, or an empty string in case of errors.
 */
std::string TCPSender::recvMessage() {
    std::lock_guard<std::mutex> lock(readMutex);

    char buffer[1024]; /**< Buffer to store received data. */
    size_t delimiterPos; /**< Position of the delimiter (newline) in the buffer. */
    
    // Keep reading until we find a newline in recvBuffer
    while ((delimiterPos = recvBuffer.find("\r\n")) == std::string::npos) {
        if (sock < 0) {
            std::cerr << "Invalid socket, cannot receive data!" << std::endl;
            return "";  // Or handle the error appropriately
        }

        int bytesRead = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cerr << "Receive failed or connection closed!" << std::endl;
            if (bytesRead < 0) perror("recv failed");
            return "";
        } 
        recvBuffer.append(buffer, bytesRead);
        
    }

    // Extract the message up to the newline (exclude)
    std::string message = recvBuffer.substr(0, delimiterPos);
    // Remove the extracted message from the buffer
    recvBuffer.erase(0, delimiterPos + 2);
    return message;
}

/**
 * @brief Closes the connection to the server.
 * 
 * This method safely closes the socket connection and sets the socket handle
 * to -1. It ensures thread-safety with a mutex lock.
 */
void TCPSender::closeConnection() {
    std::lock_guard<std::mutex> lock(sockMutex2);  /**< Protect socket access */

    if (sock != -1) {
        safePrint("Connection closed for sock: " + std::to_string(sock));
        close(sock);
        sock = -1;
    }
}

/**
 * @brief Sends a message to the connected server.
 * 
 * This method sends a message to the server, appending a newline character
 * to the message. It handles socket errors, connection issues, and ensures
 * thread safety.
 * 
 * @param msg The message to be sent to the server.
 * @return true if the message was sent successfully, false otherwise.
 */
bool TCPSender::sendMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(sockMutex);  /**< Protect socket access */

    if (sock == -1) {
        std::cerr << "Not connected! Call connectToServer first." << std::endl;
        return false;
    }

    std::string fullMessage = msg + "\r\n";

    ssize_t bytesSent = send(sock, fullMessage.c_str(), fullMessage.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "Send failed: " << strerror(errno) << " (" << errno << ")" << std::endl;
        closeConnection();
        return false;
    }
    safePrint("Message sent from socket " + std::to_string(sock) + ": " + msg);
    return true;
}
