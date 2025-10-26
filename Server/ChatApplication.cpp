#include "ChatApplication.h"
#include <iostream>

ChatApplication::ChatApplication(NetworkServer& net_server) : server(net_server) {
    server.setOnConnect([this](int id) { onClientConnected(id); });
    server.setOnDisconnect([this](int id) { onClientDisconnected(id); });
    server.setOnDataReceived([this](int id, const std::string& data) { onDataReceived(id, data); });
}

std::string ChatApplication::getUsername(int client_id) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    auto it = clients.find(client_id);
    if (it != clients.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

void ChatApplication::broadcast(const std::string& message, int sender_id) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for(auto const& [socket, username] : clients) {
        if(socket != sender_id) {
            server.send(socket, message);
        }
    }
}

void ChatApplication::registerUser(int client_id, const std::string& username) {
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients[client_id] = username;
    }
    std::string join_msg = "[SERVER]: " + username + " has joined the room.";
    std::cout << join_msg << std::endl;
    broadcast(join_msg, client_id);
}

void ChatApplication::handleChatMessage(int client_id, const std::string& msg) {
    if (msg.empty()) return;

    if (msg[0] == '/') {
        if (msg == "/users") {
            std::string user_list = "[SERVER]: connected users:\n";
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for (auto const& [socket, name] : clients) user_list += " - " + name + "\n";
            }
            server.send(client_id, user_list);
        } 
        else {
            server.send(client_id, "[SERVER]: unknown command: " + msg + "\n");
        }
    } 
    else {
        std::string chat_msg = "[" + getUsername(client_id) + "]: " + msg;
        std::cout << chat_msg << std::endl;
        broadcast(chat_msg, client_id);
    }
}

void ChatApplication::onClientConnected(int client_id) {
    std::cout << "client " << client_id << " connected.\n";
}

void ChatApplication::onClientDisconnected(int client_id) {
    std::string username = "UNKNOWN";
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        auto it = clients.find(client_id);
        if (it != clients.end()) {
            username = it->second;
            clients.erase(it);
        }
    }
    if (username != "UNKNOWN") {
        std::cout << username << " disconnected.\n";
        broadcast("[SERVER]: " + username + " has left the chat.", client_id);
    }
}

void ChatApplication::onDataReceived(int client_id, const std::string& data) {
    bool is_registered;
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        is_registered = clients.find(client_id) != clients.end();
    }

    if (!is_registered) registerUser(client_id, data);
    else handleChatMessage(client_id, data);
}
