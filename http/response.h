#pragma once

#include <fstream>
#include <mime/mime.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <array>
#include <iomanip>

#include "header.h"
#include "http.h"
#include "../util.cpp"

class Response
{
private:
    Header r_header;
    Socket r_socket;
    std::string r_server;
public:
    Response(Socket sock, std::string server) : r_socket(sock), r_server(server) {
        r_header = Header();
        r_header.setStatus(200); //default
    };

    void Set(std::string field, std::string value);
    Response *Status(int statuscode);
    void Error();
    void Send(std::string message);
    void SendFile(std::string path);
};

void Response::Set(std::string field, std::string value){
    r_header.Add(field, value);
    return;
}

Response *Response::Status(int statuscode){
    r_header.setStatus(statuscode);
    return this;
}

void Response::Error(){
    Send(StatusCodes[r_header.getStatus()]);
    return;
}

void Response::Send(std::string message){
    r_header.Add("Content-Type", "text/plain");
    r_header.Add("Content-Length", std::to_string(message.size()));
    r_header.Add("Date", util::GetTimeString("%a, %e %b %Y %X %Z"));
    if(r_server != ""){ r_header.Add("Server", r_server); }
    r_header.Write(r_socket);

    for(long min = 0; min < message.size(); min+=(CHUNKSIZE-1)){
        char resp[CHUNKSIZE];
        for(long i = min; i < (min+(CHUNKSIZE-1)); i++){
            resp[i-min] = message[i];
        }
        r_socket.Write(resp, CHUNKSIZE);
        memset(resp, 0, strlen(resp));
    }

    return;
}

void Response::SendFile(std::string path){
    std::ifstream src(path, std::ios::binary);
    if (src) {
        printf("Sending File: %s\n", path.c_str());

        src.seekg(0, src.end);
        int fileLength = src.tellg();
        src.seekg(0, src.beg);

        r_header.setStatus(200);
        r_header.Add("Content-Type", mime::lookup(path));
        r_header.Add("Content-Length", std::to_string(fileLength));
        r_header.Add("Connection", "keep-alive");
        r_header.Add("Date", util::GetTimeString("%a, %e %b %Y %X %Z"));
        if(r_server != ""){ r_header.Add("Server", r_server); }
        r_header.Write(r_socket);

        char * buffer = new char [CHUNKSIZE];
        while (!src.eof())
        {
            memset(buffer, 0, CHUNKSIZE);
            src.read(buffer,CHUNKSIZE);
            std::streamsize dataSize = src.gcount();
            r_socket.Write(buffer, dataSize);
        }

        src.close();
    }else{
        Status(404)->Error();
    }

    return;
}
