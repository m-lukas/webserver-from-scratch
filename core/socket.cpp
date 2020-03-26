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
}

void Socket::Write(char *resp){
    write(m_socket, resp, strlen(resp));
}

void Socket::Close()
{
    if(m_socket != 0) close(m_socket);
    m_socket = 0;
}

