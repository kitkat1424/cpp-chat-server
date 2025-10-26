#include "NetworkServer.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

#define BUFFER_SIZE 1024

NetworkServer::NetworkServer() : server_fd(0) {}

std::string NetworkServer::trim_string(const std::string& str) {
    std::string s = str;
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
    return s;
}

void NetworkServer::client_handler_thread(int client_socket) {
    if(on_connect_cb) on_connect_cb(client_socket);

    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    while((bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        std::string data = trim_string(buffer);
        if (on_data_cb && !data.empty()) on_data_cb(client_socket, data);
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (on_disconnect_cb) on_disconnect_cb(client_socket);
    close(client_socket);
}

void NetworkServer::setOnConnect(std::function<void(int)> cb) { on_connect_cb = cb; }
void NetworkServer::setOnDisconnect(std::function<void(int)> cb) { on_disconnect_cb = cb; }
void NetworkServer::setOnDataReceived(std::function<void(int, const std::string&)> cb) { on_data_cb = cb; }

void NetworkServer::send(int client_id, const std::string& message) {
    std::string msg = message + "\n";
    write(client_id, msg.c_str(), msg.length());
}

void NetworkServer::start(int port) {
    struct sockaddr_in address;
    int opt = 1;
    int addr_len = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << "...\n";

    while(true) {
        int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addr_len);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }
        std::thread(&NetworkServer::client_handler_thread, this, client_socket).detach();
    }
}

void NetworkServer::stop() {
    if(server_fd) close(server_fd);
}
