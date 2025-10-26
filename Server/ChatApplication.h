#ifndef CHATAPPLICATION_H
#define CHATAPPLICATION_H

#include <string>
#include <map>
#include <mutex>
#include "NetworkServer.h"

class ChatApplication {
private:
    std::map<int, std::string> clients;
    std::mutex clients_mutex;
    NetworkServer& server;

    std::string getUsername(int client_id);
    void broadcast(const std::string& message, int sender_id);
    void registerUser(int client_id, const std::string& username);
    void handleChatMessage(int client_id, const std::string& msg);

public:
    ChatApplication(NetworkServer& net_server);
    void onClientConnected(int client_id);
    void onClientDisconnected(int client_id);
    void onDataReceived(int client_id, const std::string& data);
};

#endif
