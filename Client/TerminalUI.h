#pragma once

#include <string>

class NetworkClient;

class TerminalUI {
public:
    TerminalUI(NetworkClient& client);
    void start(); 

private:
    NetworkClient& client; 
    bool is_running = true;

    void printMessage(const std::string& message);
    void onServerDisconnect();
};