#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <fstream>
#include <mime/mime.h>

#include "server.h"
#include "socket.cpp"
#include "../http/request.h"
#include "../http/header.h"
#include "../logger/logger.h"


using namespace std;

Server::Server(){
    m_running = false;
    m_socket = Socket();
}

void Server::Stop(){
    m_running = false;
}

void Server::Listen(int port){

    try
    {
        m_socket.SetOpt(REUSE_ADDRESS, 1);
        m_socket.Bind(port);
        m_socket.Listen();
    }
    catch(const std::exception& e)
    {
        logger::error(e.what());
        return;
    }
    
    m_running = true;

    while(m_running){
        Socket sock = m_socket.Accept();

        char msg[30000] = {0};
        sock.Read(msg);
        printf("%s\n", msg);

        Request req = Request();
        auto err = req.Parse(msg);
        if(err != 0) {
            logger::debug("Request Parsing Failed");
            SendErrorResponse(sock, 500);
            continue;
        }

        Header h = req.getHeader();

        Method m = h.getMethod();
        std::string path = h.getPath();

        std::ifstream src(path, std::ios::binary);
        if (src) {
            src.seekg(0, src.end);
            int fileLength = src.tellg();
            src.seekg(0, src.beg);

            Header h = Header();
            h.setStatus(200);
            h.Add("Content-Type", mime::lookup(path));
            h.Add("Content-Length", std::to_string(fileLength));
            h.Add("Connection", "keep-alive");
            h.Write(sock);

            char * buffer = new char [CHUNKSIZE];
            while (!src.eof())
            {
                memset(buffer, 0, CHUNKSIZE);
                src.read(buffer,CHUNKSIZE);
                std::streamsize dataSize = src.gcount();
                sock.Write(buffer, dataSize);
            }

            src.close();
        }else{
            SendErrorResponse(sock, 404);
        }

        sock.Close();
    }

    m_socket.Close();
}

void Server::SendHTTPResponse(Socket sock, int statuscode, std::string message){
    Header h = Header();
    h.setStatus(statuscode);
    h.Add("Content-Type", "text/plain");
    h.Add("Content-Length", std::to_string(message.size()));
    h.Write(sock);

    for(long min = 0; min < message.size(); min+=(CHUNKSIZE-1)){
        char resp[CHUNKSIZE];
        for(long i = min; i < (min+(CHUNKSIZE-1)); i++){
            resp[i-min] = message[i];
        }
        sock.Write(resp, CHUNKSIZE);
        memset(resp, 0, strlen(resp));
    }
    
    sock.Close();
}

void Server::SendErrorResponse(Socket sock, int statuscode){
    SendHTTPResponse(sock, statuscode, StatusCodes[statuscode]);
}

//string_view or span > later

//put file serving into seperate function
//put message serving into seperate function
//index.html redirection
//static routes
//read resources for LU
//Look into async request handling