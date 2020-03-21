#include "client.cpp"

#define ADDRESS "127.0.0.1"
#define PORT 8080

int main() {
    Client client = Client();
    client.Connect((char *)ADDRESS, PORT);

    char resp[1024] = {0};
    client.Send((char *)"Hello Message", resp);
    printf("%s\n",resp);

    return 0;
}