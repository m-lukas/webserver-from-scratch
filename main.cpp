#include "core/server.cpp"

#define PORT 8080

int main(){
    Server server = Server();

    printf("Listening on port %d\n", PORT);
    server.Listen(PORT); //TODO: Define Adress


    return 1;
}
