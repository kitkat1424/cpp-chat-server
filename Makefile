# --- Compiler and Flags ---
CXX = g++
CPPFLAGS = -std=c++17 -Wall -I.
LDFLAGS = -lpthread

# --- Server ---
SERVER_DIR = Server
SERVER_SRCS = $(SERVER_DIR)/main.cpp $(SERVER_DIR)/NetworkServer.cpp $(SERVER_DIR)/ChatApplication.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
SERVER_EXEC = server

# --- Client ---
CLIENT_DIR = Client
CLIENT_SRCS = $(CLIENT_DIR)/main.cpp $(CLIENT_DIR)/NetworkClient.cpp $(CLIENT_DIR)/TerminalUI.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
CLIENT_EXEC = client
# Client-specific linker flags
CLIENT_LDFLAGS = $(LDFLAGS) -lncurses

# --- Targets ---
.PHONY: all clean

# Default target
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Rule to link the server executable
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) -o $(SERVER_EXEC) $(LDFLAGS)

# Rule to link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CXX) $(CLIENT_OBJS) -o $(CLIENT_EXEC) $(CLIENT_LDFLAGS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(SERVER_EXEC) $(CLIENT_OBJS) $(CLIENT_EXEC)