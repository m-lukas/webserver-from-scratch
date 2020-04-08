#include "core/server.cpp"
#include "logger/logger.h"

#define PORT 8080

int main(){
    Server server = Server();
    logger::SetLevel(logger::INFO);

    logger::info("Listening on port %d\n", PORT);
    //logger::setLevel

    //printf("Listening on port %d\n", PORT);
    server.Listen(PORT); //TODO: Define Adress


    return 1;
}
