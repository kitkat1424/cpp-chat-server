#include "TerminalUI.h"
#include "NetworkClient.h" 
#include <iostream>

TerminalUI::TerminalUI(NetworkClient& net_client) : client(net_client) {
    client.setOnDataReceived([this](const std::string& data) {
        this->printMessage(data);
    });
    
    client.setOnDisconnect([this]() {
        this->onServerDisconnect();
    });
}

void TerminalUI::start() {
    // registration
    std::string username;
    std::cout << "connected to server! please enter your username: ";
    std::getline(std::cin, username);
    client.send(username); 

    std::cout << "welcome, " << username << ". you can start chatting. (type 'exit' to quit)" << std::endl;

    // 2. Main send loop
    while (is_running) {
        std::cout << "You: ";
        std::string message;
        std::getline(std::cin, message);

        if (std::cin.eof() || message == "exit") {
            is_running = false;
            break;
        }

        if (is_running) {
            client.send(message);
        }
    }
    client.stop(); 
}

void TerminalUI::printMessage(const std::string& message) {
    std::cout << "\r" << message << std::endl;
    std::cout << "You: ";
    std::cout.flush(); 
}

void TerminalUI::onServerDisconnect() {
    std::cout << "\r[SYSTEM]: server disconnected." << std::endl;
    is_running = false;
}