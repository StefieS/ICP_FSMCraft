#include "NetworkHandler.h"
#include <thread>
#include <chrono>
#include <iostream>

void clientThread(int clientId, const std::string& host, int port) {
    NetworkHandler client(host, port);
    client.connectToServer();

    // Send a message from the client
    std::string message = "Hello from Client " + std::to_string(clientId) + "!";
    client.sendToHost(message);

    // Allow time for the server to process and respond
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Receive the server response (should be echoed message)
    std::string response = client.recvFromHost();
    std::cout << "Client " << clientId << " received: " << response << std::endl;

    // Clean up: close the connection
    client.closeConnection();
}

int main() {
    // Create the listener server in a separate thread
    NetworkHandler serverHandler("127.0.0.1", 8080);

    std::thread listenerThread([&serverHandler]() {
        serverHandler.listen(8080);  // This calls TCPListener::startListening internally
    });

    // Allow time for server to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // === Create client threads ===
    std::thread client1Thread(clientThread, 1, "127.0.0.1", 8080);
    std::thread client2Thread(clientThread, 2, "127.0.0.1", 8080);

    // Wait for clients to finish communication
    client1Thread.join();
    client2Thread.join();

    // End the server gracefully (in real code you'd signal it)
    listenerThread.join();

    return 0;
}
