#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <vector>

#include "socket.h"
#include "../util.cpp"

enum method 
{    
    GET,
    POST,
    PUT,
    DELETE,
    CONNECT,
    HEAD,
    OPTIONS,
    PATCH,
    TRACE,
    UNKNOWN = -1,
};

method getMethod(std::string method){
    if(method == "GET") return GET;
    if(method == "POST") return POST;
    if(method == "PUT") return PUT;
    if(method == "DELETE") return DELETE;
    if(method == "CONNECT") return CONNECT;
    if(method == "HEAD") return HEAD;
    if(method == "OPTIONS") return OPTIONS;
    if(method == "PATCH") return PATCH;
    if(method == "TRACE") return TRACE;

    return UNKNOWN;
}

bool isValidMethod(std::string method){
    switch (getMethod(method))
    {
    case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }
}

bool isValidVersion(std::string version){
    if(version == "HTTP/1.1"){
        return true;
    }
}

void parseHeader(char* msg){
    //std::vector<std::string> v;
    std::stringstream ss(msg);
    std::string line;

    //Header created
    
    if(getline(ss, line, '\n')){
        std::vector<std::string> v = Split(line, ' ', 2);
        std::string method = v.at(0);
        std::string path = v.at(1);
        std::string version = v.at(2);

        std::printf("Method: %s\n", method.c_str());
        std::printf("Path: %s\n", path.c_str());
        std::printf("Version: %s\n", version.c_str());
    }

    while(getline(ss, line, '\n')){
        if(line == "\r"){
            break; //end of header
        }

        std::vector<std::string> v = Split(line, ':', 1);

        std::string field = v.at(0);
        std::string value = v.at(1);

        std::printf("Field: %s - Value: %s\n", field.c_str(), value.c_str());

        //std::printf("%s\n", line.c_str());
        //v.push_back(line);
    }

    //std::printf("%lu\n", v.size());
}

Socket::Socket()
{
    m_socket = (int)(socket(AF_INET, SOCK_STREAM, 0));
    if(m_socket == 0) throw std::runtime_error("cannot create socket");
}

Socket::~Socket()
{
}

void Socket::Bind(int port){
    m_addrlen = sizeof(m_address);

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);
    memset(m_address.sin_zero, 0, m_addrlen);

    int err = bind(m_socket, (struct sockaddr*)&m_address, m_addrlen);
    if(err != 0) throw std::runtime_error("bind failed");

    m_port = port;
}

void Socket::Listen(){
    int err = listen(m_socket, 10);
    if(err < 0) throw std::runtime_error("in listen");
}

Socket Socket::Accept(){
    int sock_accept = accept(m_socket, (struct sockaddr *)&m_address, (socklen_t*)&m_addrlen);
    if(sock_accept < 0) throw std::runtime_error("cannot accept on socket");

    return Socket(sock_accept, m_address, m_port);
}

void Socket::Read(char* req){
    long valread;

    valread = read(m_socket, req, 1024);
    if(valread < 0) throw std::runtime_error("no bytes to read");

    parseHeader(req);
}

void Socket::Write(char *resp){
    write(m_socket, resp, strlen(resp));
}

void Socket::Close()
{
    if(m_socket != 0) close(m_socket);
    m_socket = 0;
}

