#pragma once

#include <string>
#include <ncurses.h>
#include <vector>
#include <queue>
#include <mutex>

class NetworkClient;

class TerminalUI {
public:
    TerminalUI(NetworkClient& client);
    ~TerminalUI();

    void start(); 

private:
    NetworkClient& client; 
    bool is_running = true;

    std::string input_buffer;
    std::string username;

    //--ncurses library windows--//
    WINDOW* win_border;
    WINDOW* win_chat;
    WINDOW* win_input;

    std::vector<std::string> chat_history;

    void init();              
    void draw_layout();       
    void run();               
    void handle_input(int ch);  
    void send_message();      

    void print_to_chat(const std::string& message); 
    void redraw_chat();       
    void redraw_input();      
    void get_username();      

    void on_data_received(const std::string& data);
    void on_server_disconnect();

};