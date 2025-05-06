#include "NetworkHandler.h"

void TCPListener::startListening(int port, std::function<void(const std::string&, int)> onMessage) {
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

    std::cout << "Listening for a single client on port " << port << "..." << std::endl;

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("accept");
            close(server_fd);
            return;
        }

        std::cout << "Client connected!" << std::endl;

        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesRead = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytesRead <= 0) {
                std::cout << "Client disconnected." << std::endl;
                break;  // Break the inner loop to accept a new client if they disconnect
            }

            std::string msg(buffer, bytesRead);
            try {
                onMessage(msg, client_socket); 
            } catch (const std::exception& e) {
                std::cerr << "Error processing message: " << e.what() << std::endl;
            }
        }

        close(client_socket);  // Close the client socket after they disconnect
        std::cout << "Waiting for the same client to reconnect..." << std::endl;
    }

    close(server_fd);  // Close the server socket when shutting down
}
