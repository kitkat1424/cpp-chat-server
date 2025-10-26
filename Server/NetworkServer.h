#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include <string>
#include <functional>

class NetworkServer {
private:
    int server_fd;
    std::function<void(int)> on_connect_cb;
    std::function<void(int)> on_disconnect_cb;
    std::function<void(int, const std::string&)> on_data_cb;

    static std::string trim_string(const std::string& str);
    void client_handler_thread(int client_socket);

public:
    NetworkServer();
    void setOnConnect(std::function<void(int)> cb);
    void setOnDisconnect(std::function<void(int)> cb);
    void setOnDataReceived(std::function<void(int, const std::string&)> cb);
    void send(int client_id, const std::string& message);
    void start(int port);
    void stop();
};

#endif
