#pragma once

#include <string>
#include <functional>
#include <thread>

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient(); 

    bool connect(const std::string& ip, int port);
    void send(const std::string& message);
    void stop();

    void setOnDataReceived(std::function<void(const std::string&)> cb);
    void setOnDisconnect(std::function<void()> cb);

private:
    int client_fd;
    std::thread receiver_thread;
    bool is_running = false;

    std::function<void(const std::string&)> on_data_cb;
    std::function<void()> on_disconnect_cb;

    void receive_loop();
};