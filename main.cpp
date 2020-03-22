#include "core/server.cpp"

#define PORT 1234

int main(){
    Server server = Server();

    printf("Listening on port %d\n", PORT);
    server.Listen(PORT);

    return 1;
}
