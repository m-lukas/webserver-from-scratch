#include "client.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

Client::Client(){
   c_socket = socket(AF_INET, SOCK_STREAM, 0);
   if(c_socket == 0) throw std::runtime_error("cannot create socket");
}

Client::~Client()
{
}

void Client::Connect(char *addr, int port){
    //c_addrlen = sizeof(c_addrlen);

    memset(&c_address, '0', sizeof(c_address));
    
    c_address.sin_family = AF_INET;
    c_address.sin_port = htons(port);

    int err = inet_pton(AF_INET, addr, &c_address.sin_addr);
    if(err <= 0) throw std::runtime_error("invalid address");

    err = connect(c_socket, (struct sockaddr *)&c_address, sizeof(c_address));
    if(err < 0) throw std::runtime_error("connection failed");
}

void Client::Send(char *req, char* resp){
    long valread;

    send(c_socket, req, strlen(req), 0);
    valread = read(c_socket, resp, 1024);
}

void Client::Close(){
    close(c_socket);
}