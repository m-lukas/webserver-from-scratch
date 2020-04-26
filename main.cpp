#include "core/server.cpp"
#include "logger/logger.h"

#include "routes/admin.cpp"

#define PORT 8080

int main(int argc, char* argv[]){
    if(argc > 1){
        if(strcmp(argv[1], "-v") == 0){
            logger::SetLevel(logger::DEBUG);
        } 
    }

    Server server = Server();
    printf("Listening on port %d\n", PORT);

    server.Route("/hello", f_getHello);

    server.Listen(PORT); //TODO: Define Adress
    return 1;
}
