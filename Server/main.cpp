#include "NetworkServer.h"
#include "ChatApplication.h"

#define PORT 8080

int main() {
    NetworkServer server;
    ChatApplication app(server); 
    
    server.start(PORT);
    
    return 0;
}