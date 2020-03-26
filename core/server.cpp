#include <iostream>
#include <stdexcept>

#include "server.h"
#include "socket.cpp"
#include "../http/header.h"

using namespace std;

Server::Server(){
    m_running = false;
    m_socket = Socket();
}

void Server::Listen(int port){
    m_socket.Bind(port);
    m_socket.Listen();
    m_running = true;

    while(m_running){
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        Socket sock = m_socket.Accept();

        char req[30000] = {0};
        sock.Read(req);
        printf("%s\n", req);

        char *resp;
        resp = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!";

        Header h;
        try{
            h = parseHeader(req);
        }catch(const runtime_error& e){
            std::string serr = e.what();
            printf("%s\n", serr.c_str());
            resp = (char *)"HTTP/1.1 500 Internal Server Error\nContent-Type: text/html\nContent-Length: 12\n\nInvalid Header";
        }

        //sendErrorResponse(statuscode, message)
        //sendHTTPResponse((statuscode?), header, body)

        sock.Write(resp);
        sock.Close();
        printf("------------------Response sent-------------------\n");
    }

    m_socket.Close();
}

Server::~Server(){
}