/**
*  @file NetworkHandler.h
 * @brief Header file for Networking aspects of this project
 * @author xlesigm00
 * @date 05.05.2025
 */
#pragma once 

#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <atomic>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// Abstract base class for parsing messages
class NetworkParser {
public:
    virtual std::string parseMessage(const std::string& msg) = 0;
    virtual ~NetworkParser() = default;
};

// Abstract base class for sending messages
class NetworkSender {
public:
    virtual bool sendMessage(const std::string& msg) = 0;
    virtual bool connectToServer() = 0;
    virtual std::string recvMessage() = 0;
    virtual void closeConnection() = 0;
    virtual ~NetworkSender() = default;
};

// Abstract base class for listening to messages
class NetworkListener {
public:
    virtual void startListening(int port,
        std::function<void(const std::string&, int)> onMessage,
        std::function<void(int)> onDisconnect = nullptr) = 0;
    virtual ~NetworkListener() = default;
};

// NetworkHandler which coordinates sending and listening messages
class NetworkHandler {
public:
    NetworkHandler(const std::string& host, int port);
    
    // Methods
    void sendToHost(const std::string& msg);
    void listen(int port);
    std::string recvFromHost();
    bool connectToServer();
    void closeConnection();

private:
    std::unique_ptr<NetworkSender> sender;
    std::unique_ptr<NetworkListener> listener;
    std::atomic<int> firstClientSocket{-1};
    std::vector<int> connectedClients;
    std::mutex socketMutex;
    std::mutex sockMutex2;
    std::string host;
    int port;
};

// TCP Sender for sending messages via TCP
class TCPSender : public NetworkSender {
public:
    bool sendMessage(const std::string& msg) override;
    bool connectToServer() override;
    std::string recvMessage();
    void closeConnection() override;

    void setHostAndPort(const std::string& host, int port); // Setter method for host and port

private:
    int sock = -1;
    sockaddr_in server;
    std::string host;
    std::string recvBuffer;
    int port;
};

// Simple Parser that simply returns the message as is
class SimpleParser : public NetworkParser {
public:
    std::string parseMessage(const std::string& msg) override;
};

// TCP Listener for listening to incoming TCP connections
class TCPListener : public NetworkListener {
private:
public:
    void startListening(int port,
        std::function<void(const std::string&, int)> onMessage,
        std::function<void(int)> onDisconnect = nullptr) override;
};

extern std::mutex coutMutex;
void safePrint(const std::string& msg);
