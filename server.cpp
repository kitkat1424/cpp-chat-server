#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <map>

#define PORT 8080
#define BUFFER_SIZE 1024

std::map <int, std::string> clients;
std::mutex clients_mutex;

void trim_newline(char* str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

void broadcast_msg(const std::string &message, int sender_socket) {
    std::lock_guard <std::mutex> lock(clients_mutex);
    for(auto const &[socket, username] : clients) {
        if(socket != sender_socket) {
            write(socket, message.c_str(), message.length());
        }
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;
    std::string username;

    //--registration of user--//
    bytes_read = read(client_socket, buffer, BUFFER_SIZE-1);
    if(bytes_read > 0) {
        buffer[bytes_read] = '\0';
        trim_newline(buffer);
        username = buffer;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients[client_socket] = username;
        }

        std::string join_msg = "[SERVER] :" + username + " has joined the room.";
        std::cout << join_msg << std::endl;
        broadcast_msg(join_msg, client_socket);
    } else {
        std::cout << "client " << client_socket << " failed to register." << std::endl;
        close(client_socket);
        return;
    }
    //--end of registration--//

    memset(buffer, 0, BUFFER_SIZE);
    while((bytes_read = read(client_socket, buffer, BUFFER_SIZE-1)) > 0) {
        buffer[bytes_read] = '\0';
        trim_newline(buffer);

        if(buffer[0] == '/') {
            std::string command = buffer;
            if (command == "/users") {
                std::string user_list = "[SERVER]: connected users:\n";
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    for (auto const& [socket, name] : clients) {
                        user_list += " - " + name + "\n";
                    }
                }
                write(client_socket, user_list.c_str(), user_list.length());
            } else {
                std::string unknown_cmd = "[SERVER]: unknown command: " + command + "\n";
                write(client_socket, unknown_cmd.c_str(), unknown_cmd.length());
            }
        }
        else {
            std::string chat_msg = "[" + username + "]: " + buffer;
            std::cout << chat_msg << std::endl;

            broadcast_msg(chat_msg, client_socket);
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    if(bytes_read == 0) {
        std::cout << "client " << username << " (" << client_socket << ") disconnected." << std::endl;
        std::string leave_msg = "[SERVER]: " + username + " has left the chat.";
        broadcast_msg(leave_msg, client_socket);
    } else {
        perror("read");
    }

    {
        std::lock_guard <std::mutex> lock(clients_mutex);
        clients.erase(client_socket);
    }
    close(client_socket);
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addr_len = sizeof(address);
    

    //creating server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //defining server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //binding socket to address and port
    int bind_check = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if(bind_check < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //listening for incoming connections
    int listen_check = listen(server_fd, 10);
    if(listen_check < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "server listening on port " << PORT << "..." << std::endl;

    //accepting a connection
    while(true) {
        int client_socket;
        if((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len)) < 0) {
            perror("accept");
            continue;
        }

        std::cout << "new client connected! socket fd is : " << client_socket << std::endl;

        std::thread clientThread(handle_client, client_socket);
        clientThread.detach();
    }

    close(server_fd);

    return 0;
}