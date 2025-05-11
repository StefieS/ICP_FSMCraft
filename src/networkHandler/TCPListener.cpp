#include "NetworkHandler.h"
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <functional>
#include "../messages/Message.h"

std::atomic<int> firstClientSocket{-1};  // Shared socket ID of the first client
std::mutex clientMutex;
std::mutex disconnectMutex;


void handleClientCommunication(int client_socket, std::function<void(const std::string&, int)> onMessage, std::atomic<bool>& stopReceived) {
    char buffer[1024];
    std::string recvBuffer;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            safePrint("Client " + std::to_string(client_socket) + " disconnected.");
            close(client_socket);
            break;
        }

        recvBuffer.append(buffer, bytesRead);

        size_t pos;
        while ((pos = recvBuffer.find("\r\n")) != std::string::npos) {
            std::string msg = recvBuffer.substr(0, pos);
            recvBuffer.erase(0, pos + 2);

            safePrint("Received message from socket " + std::to_string(client_socket) + ": " + msg);

            try {
                onMessage(msg, client_socket);
                Message isStop(msg);
                if (isStop.getType() == EMessageType::STOP) {
                    stopReceived.store(true);
                    safePrint("STOP message received.");
                    break;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        }
    }
}

void TCPListener::startListening(int port,
    std::function<void(const std::string&, int)> onMessage,
    std::function<void(int)> onDisconnect) {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return;
    }

    std::cout << "Listening for clients on port " << port << "..." << std::endl;

    std::atomic<bool> stopReceived(false);
    std::vector<std::thread> clientThreads;

    // To simulate unblocking the accept() method when it's time to stop
    std::thread unblockerThread([&]() {
        while (!stopReceived.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Check periodically
        }
        // When it's time to stop, we connect to the server to unblock accept()
        int unblockSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (unblockSocket >= 0) {
            connect(unblockSocket, (sockaddr*)&address, sizeof(address));
            close(unblockSocket);  // Immediately close it
        }
    });

    while (!stopReceived.load()) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("accept");
            close(server_fd);
            return;
        }

        safePrint("Client connected! Socket: " + std::to_string(client_socket));

        // Start client thread to handle communication
        std::thread t([client_socket, onMessage, onDisconnect, &stopReceived]() {
            handleClientCommunication(client_socket, onMessage, stopReceived);
            onDisconnect(client_socket);  // Call the disconnect handler after communication
        });
        clientThreads.push_back(std::move(t));  // Store the thread
    }

    // After stopReceived is set, join all threads
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }

    safePrint("Server has stopped accepting clients.");
    close(server_fd);

    // Join the unblocker thread as well
    unblockerThread.join();
}
