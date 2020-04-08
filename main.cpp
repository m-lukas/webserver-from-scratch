#include "core/server.cpp"
#include "logger/logger.h"

#define PORT 8080

int main(int argc, char* argv[]){
    if(argc > 1){
        if(strcmp(argv[1], "-v") == 0){
            logger::SetLevel(logger::DEBUG);
        } 
    }

    Server server = Server();
    logger::info("Listening on port %d\n", PORT);

    server.Listen(PORT); //TODO: Define Adress
    return 1;
}
