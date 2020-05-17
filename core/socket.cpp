#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <vector>

#include "socket.hpp"
#include "../util.cpp"
#include "../logger/logger.h"

static int mapSockOpt(SockOpt opt){
    switch (opt)
    {
    case REUSE_ADDRESS:
        return SO_REUSEADDR;
    case TIMEOUT:
        return SO_RCVTIMEO;
    default:
        return 0;
    }
}

Socket::Socket()
{
    m_socket = (int)(socket(AF_INET, SOCK_STREAM, 0));
    if(m_socket == 0) throw std::runtime_error("cannot create socket");
}

Socket::~Socket()
{
    //close(m_socket); //Gets triggered after request reading (outside of try block)
}

void Socket::setOpt(SockOpt opt, const void *value, size_t size){
    int err = setsockopt(m_socket, SOL_SOCKET, mapSockOpt(opt), value, size);
    if(err != 0) throw std::runtime_error("cannot set socket option");
}

void Socket::SetTimeout(int seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec  = 500000;
    setOpt(TIMEOUT, (struct timeval *)&tv, sizeof(struct timeval));
}

void Socket::SetOpt(SockOpt opt, int value){
    setOpt(opt, &value, sizeof(value));
}

void Socket::Bind(int port){
    m_addrlen = sizeof(m_address);

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);
    memset(m_address.sin_zero, 0, m_addrlen);

    int err = ::bind(m_socket, (struct sockaddr*)&m_address, m_addrlen);
    if(err != 0) throw std::runtime_error("bind failed");

    m_port = port;
}

void Socket::Listen(){
    int err = listen(m_socket, 10);
    if(err < 0) throw std::runtime_error("in listen");
}

Socket Socket::Accept(){
    int sock_accept = accept(m_socket, (struct sockaddr *)&m_address, (socklen_t*)&m_addrlen);
    if(sock_accept < 0) logger::error("cannot accept on socket");

    return Socket(sock_accept, m_address, m_port);
}

long Socket::Read(char* req){
    long valread;
    valread = read(m_socket, req, 1024);
    return valread;
}

void Socket::Write(char *resp, size_t length){
    write(m_socket, resp, length);
}

void Socket::Close(){
    if(m_socket != 0) close(m_socket);
    m_socket = 0;
}

bool Socket::InvalidDescriptor(){
    return (m_socket < 0);
}

