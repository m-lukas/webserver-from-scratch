#include <iostream>
#include <stdexcept>

#include "server.h"
#include "socket.cpp"

using namespace std;

Server::Server(){
    m_running = false;
    m_socket = Socket();
    printf("Server-Init");
}

void Server::Listen(int port){
    printf("Socket-Listen");
    m_socket.Bind(port);
    m_socket.Listen();
    m_running = true;

    while(m_running){
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        Socket sock = m_socket.Accept();

        char req[30000] = {0};
        sock.Read(req);
        printf("%s\n", req);

        char *resp = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!";

        sock.Write(resp);
        sock.Close();
        printf("------------------Response sent-------------------\n");
    }

    m_socket.Close();
}

Server::~Server(){
}