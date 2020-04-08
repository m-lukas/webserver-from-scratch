#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <fstream>

#include "server.h"
#include "socket.cpp"
#include "../http/header.h"
#include "../logger/logger.h"

using namespace std;

Server::Server(){
    m_running = false;
    m_socket = Socket();
}

void Server::Listen(int port){

    //CONSULTING - Where to error handle?

    //exception
    m_socket.SetOpt(REUSE_ADDRESS, 1);
    m_socket.Bind(port);
    m_socket.Listen();
    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept();

        char req[30000] = {0};
        sock.Read(req);
        printf("%s\n", req);

        const char *resp;
        resp = static_cast<const char *>("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 12\n\nHello world!"); //alternativ: raw string literals

        Header h = Header();
        //no exception
        try{
            h = parseHeader(req);
        }catch(const runtime_error& e){
            logger::error(e.what());
            SendErrorResponse(sock, 500);
            continue;
        }

        Method m = h.getMethod();
        std::string path = h.getPath();

        //printf("PATH..........................................................: %s\n", path.c_str());

        std::ifstream src(path, std::ios::binary);
        if (src) {
            src.seekg(0, src.end);
            int length = src.tellg();
            src.seekg(0, src.beg);

            char * buffer = new char [length];
            src.read (buffer,length);

            if(src){
                SendHTTPResponse(sock, 200, buffer);
            }else{
                SendErrorResponse(sock, 500);
            }

            src.close();
            continue;
        }

        /*

        *** REQUEST PROCESSING

        */

        SendHTTPResponse(sock, 200, "Hello world!");
    }

    m_socket.Close();
}

void Server::SendHTTPResponse(Socket sock, int statuscode, std::string message){
    Header h = Header();
    h.setStatus(statuscode);
    h.Add("Content-Type", "text/html");
    h.Add("Content-Length", std::to_string(message.size()));
    std::string headerStr = h.Stringify();

    std::string respStr = headerStr + message; 

    char resp[respStr.size() + 1];
    strcpy(resp, respStr.c_str());

    sock.Write(resp);
    sock.Close();
}

void Server::SendErrorResponse(Socket sock, int statuscode){
    Header h = Header();
    h.setStatus(statuscode);
    h.Add("Content-Type", "text/plain");
    h.Add("Content-Length", "0");
    std::string headerStr = h.Stringify();

    std::string respStr = headerStr; 

    char resp[respStr.size() + 1]; //not legal c++ (char array with dynamic size)
    strcpy(resp, respStr.c_str());

    sock.Write(resp); //string_view or span
    sock.Close();
}