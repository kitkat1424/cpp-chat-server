#include "TerminalUI.h"
#include "NetworkClient.h" 
#include <iostream>

TerminalUI::TerminalUI(NetworkClient& net_client) : client(net_client) {
    client.setOnDataReceived([this](const std::string& data) {
        this->on_data_received(data);
    });
    
    client.setOnDisconnect([this]() {
        this->on_server_disconnect();
    });
}

TerminalUI::~TerminalUI() {
    endwin();
}

void TerminalUI::start() {
    init();
    draw_layout();
    get_username();
    print_to_chat("welcome " + username + " to the room. you can start chatting.");

    run();
}

void TerminalUI::init() {
    initscr();        //start ncurses mode
    cbreak();         //disable line buffering
    noecho();
    keypad(stdscr, TRUE);

    int height, width;
    getmaxyx(stdscr, height, width);

    win_border = newwin(height, width, 0, 0);
    win_chat = newwin(height - 5, width - 2, 1, 1);
    win_input  = newwin(3, width - 2, height - 4, 1);

    scrollok(win_chat, TRUE); 
    refresh(); 
}

void TerminalUI::draw_layout() {
    box(win_border, 0, 0);
    box(win_input, 0, 0);
    wrefresh(win_border);

    mvwprintw(win_border, 0, 3, " cpp chat client ");
    mvwprintw(win_input, 0, 3, " your message ");

    wrefresh(win_border);
    wrefresh(win_chat);
    wrefresh(win_input);
}

// In Client/TerminalUI.cpp

void TerminalUI::get_username() {
    const std::string prompt = "connected. please enter your username: ";
    const int MAX_NAME_LEN = 99; 
    char buffer[MAX_NAME_LEN + 1] = {0};

    int i = 0;
    int ch;
    mvwprintw(win_chat, 1, 1, "%s", prompt.c_str());
    wrefresh(win_chat);

    while((ch = wgetch(win_chat)) != '\n') {
        if(ch == KEY_BACKSPACE || ch == 127) {
            if(i > 0) {
                i--;
                buffer[i] = '\0';
            }
        } else if(i < MAX_NAME_LEN && isprint(ch)) {
            buffer[i++] = ch;
            buffer[i] = '\0';
        }

        wmove(win_chat, 1, 1);   
        wclrtoeol(win_chat);   
        
        mvwprintw(win_chat, 1, 1, "%s%s", prompt.c_str(), buffer);
        
        wmove(win_chat, 1, prompt.length() + i + 1); 
        wrefresh(win_chat);
    }

    username = buffer;
    client.send(username);

    wclear(win_chat);
    wrefresh(win_chat);
}

void TerminalUI::run() {
    while(is_running) {
        redraw_input();
        int ch = wgetch(win_input); 
        handle_input(ch);
    }
}

void TerminalUI::handle_input(int ch) {
    switch (ch) {
        case '\n': 
            send_message();
            break;
        case KEY_BACKSPACE:
        case 127: 
            if (!input_buffer.empty()) {
                input_buffer.pop_back();
            }
            break;
        default:
            if (isprint(ch)) { 
                input_buffer.push_back(ch);
            }
            break;
    }
    redraw_input();
}

void TerminalUI::send_message() {
    if (!input_buffer.empty() && is_running) {
        if (input_buffer == "exit") {
            is_running = false;
        } else {
            std::string formatted_msg = "[" + username + "]: " + input_buffer;
            print_to_chat(formatted_msg);
            client.send(input_buffer);
        }
        input_buffer.clear();
    }
}

void TerminalUI::print_to_chat(const std::string& message) {
    chat_history.push_back(message); 
    redraw_chat();
}

void TerminalUI::redraw_chat() {
    wclear(win_chat); 

    int height, width;
    getmaxyx(win_chat, height, width);

    int start_index = std::max(0, (int)chat_history.size() - height);
    
    for (size_t i = start_index; i < chat_history.size(); ++i) {
        std::string msg = chat_history[i];
        while ((int)msg.length() > width - 2) {
            wprintw(win_chat, " %s\n", msg.substr(0, width - 2).c_str());
            msg = msg.substr(width - 2);
        }
        wprintw(win_chat, " %s\n", msg.c_str());
    }

    wrefresh(win_chat);
    redraw_input(); 
}

void TerminalUI::redraw_input() {
    wclear(win_input); 
    box(win_input, 0, 0); 
    mvwprintw(win_input, 0, 3, " Your Message ");
    
    // print buffer
    mvwprintw(win_input, 1, 1, "%s", input_buffer.c_str());
    
    // move cursor to end
    wmove(win_input, 1, 1 + input_buffer.length());
    
    wrefresh(win_input);
}

void TerminalUI::on_data_received(const std::string& data) {
    print_to_chat(data);
}

void TerminalUI::on_server_disconnect() {
    std::cout << "\r[SYSTEM]: server disconnected." << std::endl;
    is_running = false;
}