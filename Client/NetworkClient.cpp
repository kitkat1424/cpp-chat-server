#include "NetworkClient.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096 

NetworkClient::NetworkClient() : client_fd(0), is_running(false) {}

NetworkClient::~NetworkClient() {
    stop(); 
}

bool NetworkClient::connect(const std::string& ip, int port) {
    struct sockaddr_in server_address;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        return false;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr) <= 0) {
        perror("invalid address");
        return false;
    }

    if (::connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        return false;
    }

    is_running = true;
    receiver_thread = std::thread(&NetworkClient::receive_loop, this);
    return true;
}

void NetworkClient::send(const std::string& message) {
    if(is_running) {
        write(client_fd, message.c_str(), message.length());
    }
}

void NetworkClient::stop() {
    is_running = false;
    
    if(client_fd) {
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        client_fd = 0;
    }
    
    if(receiver_thread.joinable()) {
        receiver_thread.join();
    }
}

void NetworkClient::setOnDataReceived(std::function<void(const std::string&)> cb) { on_data_cb = cb; }
void NetworkClient::setOnDisconnect(std::function<void()> cb) { on_disconnect_cb = cb; }

void NetworkClient::receive_loop() {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    while(is_running && (bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        
        std::string full_data = buffer;
        std::string message;
        size_t pos = 0;
        
        while((pos = full_data.find('\n')) != std::string::npos) {
            message = full_data.substr(0, pos);
            if (on_data_cb) {
                on_data_cb(message); 
            }
            full_data.erase(0, pos + 1);
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    if(is_running) {
        is_running = false;
        if (on_disconnect_cb) {
            on_disconnect_cb();
        }
    }
}