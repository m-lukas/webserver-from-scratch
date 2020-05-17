#include "client.cpp"

#include <stdio.h>

#define ADDRESS "127.0.0.1"
#define PORT 8080
#define REQUEST "GET /hello HTTP/1.1\r\nHost: localhost:8080\r\n"

int main(int argc, char* argv[]) {
    pid_t child_pid, wpid;
    int status = 0;
    int conns = 100;

    if(argc > 2){
        if(strcmp(argv[1], "--conns") == 0){
            conns = atoi(argv[2]);
        }   
    }
    
    for (int i = 0; i < conns; i++){
        auto pid = fork();

        if(pid == 0){
            Client client = Client();
            client.Connect((char *)ADDRESS, PORT);

            char resp[1024] = {0};
            client.Send((char *)REQUEST, resp);
            printf("%s\n",resp);
            exit(0);
        }
    }

    while ((wpid = wait(&status)) > 0);

    return 0;
}