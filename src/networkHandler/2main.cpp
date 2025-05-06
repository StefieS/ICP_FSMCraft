#include "NetworkHandler.h"
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    // Create NetworkHandler with the server details
    NetworkHandler handler("127.0.0.1", 8080);

    // Create a listener thread that will handle incoming connections
    std::thread listenerThread([&handler]() {
        handler.listen(8080); // Server listens on port 8080
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Sleep to give listener thread time to start

    // Connect to the server and send a message
    handler.connectToServer();  // Connect to the server at 127.0.0.1:8080
    handler.sendToHost("Hello, Server!");  // Send message to server

    // Receive response from the server
    std::string response = handler.recvFromHost();
    std::cout << "Server response: " << response << std::endl;

    // Close the connection after use
    handler.closeConnection();

    handler.connectToServer();  // Connect to the server at 127.0.0.1:8080
    handler.sendToHost("Hello, Server!"); 
    handler.closeConnection();
    // Join listener thread before exiting
    listenerThread.join();

    return 0;
}
