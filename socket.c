#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define PORT 1234

int main(int args, char const *argv[])
{

    int socket_fd, bind_res, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!";

    socket_fd = (int)(socket(AF_INET, SOCK_STREAM, 0)); //<- conncetion-establishing socket
    if(socket_fd < 0)
    {
        perror("cannot create socket");
        return 0;
    }

    /* htonl converts a long integer (e.g. address) to a network representation */ 
    /* htons converts a short integer (e.g. port) to a network representation */ 

    memset((char *)&address, 0, addrlen); //creates and preserves a pointer to a block of memory

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY); //converts localhost into network representation
    address.sin_port = htons(PORT); //converts port into network representation

    bind_res = bind(socket_fd, (struct sockaddr *)&address, addrlen);
    if (bind_res < 0) 
    {
        perror("bind failed");
        return 0;
    }

    if(listen(socket_fd, 3) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    //work-routine
    while(1){
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        new_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen); //<-request-responding socket
        if(new_socket < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer); //reading buffer of socket
        if(valread < 0)
        {
            perror("No bytes to read");
        }
        
        write(new_socket, hello, strlen(hello)); //writing to socket
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }

    return 0;
}