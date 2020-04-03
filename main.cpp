#include "core/server.cpp"
#include "core/logger.h"

#define PORT 8080

int main(){
    Server server = Server();

    logger::error("Listening on port %d\n", PORT);
    //printf("Listening on port %d\n", PORT);
    server.Listen(PORT); //TODO: Define Adress


    return 1;
}
