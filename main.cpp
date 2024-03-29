#include "core/server.cpp"
#include "logger/logger.h"
#include "models/lockfree_threadpool.h"

#include "routes/general.cpp"
#include "util.cpp"

#define PORT 8080
#define NAME "lukas's server"

int main(int argc, char* argv[]){
    if(argc > 1){
        if(strcmp(argv[1], "-v") == 0){
            logger::SetLevel(logger::DEBUG);
        } 
    }

    Server server = Server();
    printf("Listening on port %d\n", PORT);

    server.SetName(NAME);
    server.Route("/", getIndex);
    server.Route("/hello", getHello);

    server.SetConcurrencyMode(util::CON_MODE_PROCESS);

    server.Listen(PORT); //TODO: Define Adress
    return 1;
}
