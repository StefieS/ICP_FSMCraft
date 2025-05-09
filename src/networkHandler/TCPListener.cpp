#include "NetworkHandler.h"

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

std::atomic<int> firstClientSocket{-1};  // Shared socket ID of the first client
std::mutex clientMutex;

void handleClient(int client_socket, std::function<void(const std::string&, int)> onMessage) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            safePrint("Client " + std::to_string(client_socket) + " disconnected.");
            close(client_socket);
            return;
        }

        std::string msg(buffer, bytesRead);
        try {
            onMessage(msg, client_socket);  // Can log, filter, or process differently per client
        } catch (const std::exception& e) {
            std::cerr << "Error processing message: " << e.what() << std::endl;
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

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("accept");
            close(server_fd);
            return;
        }

        safePrint("Client connected! Socket: " + std::to_string(client_socket));

        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytesRead <= 0) {

                safePrint("Client " + std::to_string(client_socket) + " disconnected.");
                break;
            }

            std::string msg(buffer, bytesRead);
            safePrint("Received message from socket " + std::to_string(client_socket) + ": " + msg);

            try {
                onMessage(msg, client_socket);
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        }

        close(client_socket);
    }

    close(server_fd);
}
