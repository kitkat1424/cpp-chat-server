#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1" 

void receive_thread(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read;

    while((bytes_read = read(client_fd, buffer, BUFFER_SIZE-1)) > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "\n" << buffer << std::endl;
        std::cout << "you : ";
        std::cout.flush();
    }

    if(bytes_read == 0) {
        std::cout << "server disconnected.\n";
    } else {
        perror("read");
    }

    exit(0);
}

int main() {
    int client_fd;
    struct sockaddr_in server_address;
    std::string message;

    //create the client socket
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    //defining server's ip address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) < 0) {
        perror("invalid address / address not supported");
        exit(EXIT_FAILURE);
    }

    //connecting to the server. it is a blocking call
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    std::string username;
    std::cout << "connected to server. enter your username: ";
    std::getline(std::cin, username);
    
    write(client_fd, username.c_str(), username.length());
    std::cout << "welcome, " << username << ". you can start chatting. (type 'exit' to leave)" << std::endl;

    std::thread receiver(receive_thread, client_fd);
    receiver.detach();

    while(true) {
        std::cout << "you : ";
        std::getline(std::cin, message);
        if(std::cin.eof() || message == "exit") {
            break;
        }
        write(client_fd, message.c_str(), message.length());
    }

    close(client_fd);
    std::cout << "connection closed.\n";

    return 0;
}