/**
 * @file NetworkHandler.h
 * @brief Header file for networking aspects of the FSM tool project.
 * Defines abstract interfaces and concrete classes for TCP communication,
 * including sending, receiving, and listening to messages from remote clients or servers.
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

/**
 * @class NetworkParser
 * @brief Abstract base class for message parsers.
 */
class NetworkParser {
public:
    /**
     * @brief Parse the given message.
     * @param msg Input message string.
     * @return Parsed message string.
     */
    virtual std::string parseMessage(const std::string& msg) = 0;
    virtual ~NetworkParser() = default;
};

/**
 * @class NetworkSender
 * @brief Abstract base class for sending messages over a network.
 */
class NetworkSender {
public:
    virtual bool sendMessage(const std::string& msg) = 0;
    virtual bool connectToServer() = 0;
    virtual std::string recvMessage() = 0;
    virtual void closeConnection() = 0;
    virtual ~NetworkSender() = default;
};

/**
 * @class NetworkListener
 * @brief Abstract base class for listening to incoming network messages.
 */
class NetworkListener {
public:
    /**
     * @brief Start listening on a specific port.
     * @param port Port number.
     * @param onMessage Callback for incoming messages.
     * @param onDisconnect Optional callback when a client disconnects.
     */
    virtual void startListening(int port,
        std::function<void(const std::string&, int)> onMessage,
        std::function<void(int)> onDisconnect = nullptr) = 0;

    virtual ~NetworkListener() = default;
};

/**
 * @class NetworkHandler
 * @brief Handles coordination of message sending and listening for a single host and port.
 */
class NetworkHandler {
public:
    /**
     * @brief Constructor that initializes a network handler for the given host and port.
     * @param host Hostname or IP address to connect/listen to.
     * @param port Port number for communication.
     */
    NetworkHandler(const std::string& host, int port);

    /**
     * @brief Send a message to the connected host.
     * @param msg The message string to send.
     */
    void sendToHost(const std::string& msg);

    /**
     * @brief Start listening for incoming client connections and handle communication.
     * @param port Port number on which to listen.
     */
    void listen(int port);

    /**
     * @brief Receive a message from the connected host.
     * @return Message string received.
     */
    std::string recvFromHost();

    /**
     * @brief Establish a connection to the remote host.
     * @return True on success, false on failure.
     */
    bool connectToServer();

    /**
     * @brief Close the current network connection.
     */
    void closeConnection();

private:
    std::unique_ptr<NetworkSender> sender;       /**< Object responsible for sending messages. */
    std::unique_ptr<NetworkListener> listener;   /**< Object responsible for listening to messages. */
    std::atomic<int> firstClientSocket{-1};      /**< Socket of the first connected client. */
    std::vector<int> connectedClients;           /**< Vector for client responses */
    std::mutex socketMutex;                      /**< Mutex for thread-safe socket access. */
    std::mutex sockMutex2;                       /**< Secondary mutex for socket operations. */
    std::string host;                            /**< Target host name. */
    int port;                                    /**< Target port number. */
};

/**
 * @class TCPSender
 * @brief Sends messages to a server using the TCP protocol.
 */
class TCPSender : public NetworkSender {
public:
    bool sendMessage(const std::string& msg) override;
    bool connectToServer() override;
    std::string recvMessage() override;
    void closeConnection() override;

    /**
     * @brief Set the host and port for outgoing connections.
     * @param host IP or hostname.
     * @param port Port number.
     */
    void setHostAndPort(const std::string& host, int port);

private:
    int sock = -1;                    /**< Socket descriptor. */
    sockaddr_in server;              /**< Server socket address. */
    std::string host;                /**< Server hostname/IP. */
    std::string recvBuffer;          /**< Buffer for incoming data. */
    int port;                        /**< Server port. */
};

/**
 * @class SimpleParser
 * @brief A simple parser that returns messages as-is.
 */
class SimpleParser : public NetworkParser {
public:
    std::string parseMessage(const std::string& msg) override;
};

/**
 * @class TCPListener
 * @brief Listens for incoming TCP connections and messages.
 */
class TCPListener : public NetworkListener {
public:
    void startListening(int port,
        std::function<void(const std::string&, int)> onMessage,
        std::function<void(int)> onDisconnect = nullptr) override;
};

/**
 * @brief Global mutex for thread-safe console output.
 */
extern std::mutex coutMutex;

/**
 * @brief Utility function for printing to console in a thread-safe way.
 * @param msg The message to be printed.
 */
void safePrint(const std::string& msg);
