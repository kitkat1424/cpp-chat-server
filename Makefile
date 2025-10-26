# --- Compiler and Flags ---
CXX = g++
# -std=c++17: Use C++17 standard
# -Wall: Enable all warnings (good practice)
# -lpthread: Link the pthread library for std::thread
CXXFLAGS = -std=c++17 -Wall -lpthread

# --- Server Files ---
SERVER_DIR = Server
SERVER_SRCS = $(SERVER_DIR)/main.cpp $(SERVER_DIR)/NetworkServer.cpp $(SERVER_DIR)/ChatApplication.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o) # Replaces .cpp with .o
SERVER_EXEC = server

# --- Client Files ---
CLIENT_DIR = Client
CLIENT_SRCS = $(CLIENT_DIR)/main.cpp $(CLIENT_DIR)/NetworkClient.cpp $(CLIENT_DIR)/TerminalUI.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
CLIENT_EXEC = client

# --- Targets ---

# The default target, builds both server and client
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Rule to link the server executable
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_EXEC)

# Rule to link the client executable
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_EXEC)

# Pattern rule to compile any .cpp file into a .o (object) file
# $< means "the first prerequisite" (the .cpp file)
# $@ means "the target" (the .o file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up all built files
clean:
	rm -f $(SERVER_OBJS) $(SERVER_EXEC) $(CLIENT_OBJS) $(CLIENT_EXEC)

# Phony target to prevent conflicts with a file named 'all' or 'clean'
.PHONY: all clean