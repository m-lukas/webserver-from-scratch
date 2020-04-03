#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>

#include "server.h"
#include "socket.cpp"
#include "../http/header.h"

using namespace std;

Server::Server(){
    m_running = false;
    m_socket = Socket();
}

void Server::Listen(int port){
    m_socket.SetOpt(REUSE_ADDRESS, 1);
    m_socket.Bind(port);
    m_socket.Listen();
    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept();

        char req[30000] = {0};
        sock.Read(req);
        printf("%s\n", req);

        char *resp;
        resp = (char *)"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!";

        Header h = Header();
        try{
            h = parseHeader(req);
        }catch(const runtime_error& e){
            SendErrorResponse(sock, 500);
            continue;
        }

        /*

        *** REQUEST PROCESSING

        */

        SendHTTPResponse(sock, 200, "Hello world!");

        sock.Close();
    }

    m_socket.Close();
}

Server::~Server(){
}

void Server::SendHTTPResponse(Socket sock, int statuscode, std::string message){
    Header h = Header();
    h.setStatusCode(statuscode);
    h.Add("Content-Type", "text/html");
    h.Add("Content-Length", std::to_string(message.size()));
    std::string headerStr = h.Stringify();

    std::string respStr = headerStr + message; 

    char resp[respStr.size() + 1];
    strcpy(resp, respStr.c_str());

    sock.Write(resp);
}

void Server::SendErrorResponse(Socket sock, int statuscode){
    Header h = Header();
    h.setStatusCode(statuscode);
    h.Add("Content-Type", "text/plain");
    h.Add("Content-Length", "0");
    std::string headerStr = h.Stringify();

    std::string respStr = headerStr; 

    char resp[respStr.size() + 1];
    strcpy(resp, respStr.c_str());

    sock.Write(resp);
}