#include "NetworkClient.h"
#include "TerminalUI.h"
#include <iostream>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    NetworkClient client;
    TerminalUI ui(client);

    if (!client.connect(SERVER_IP, PORT)) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1; 
    }

    ui.start(); 

    return 0;
}