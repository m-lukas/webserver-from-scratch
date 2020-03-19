#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <string.h>

#include "socket.h"

Socket::Socket()
{
    m_fd = (int)(socket(AF_INET, SOCK_STREAM, 0));
    if(m_fd == 0) throw std::runtime_error("cannot create socket");
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

    int err = bind(m_fd, (struct sockaddr*)&m_address, m_addrlen);
    if(err != 0) throw std::runtime_error("bind failed");

    m_port = port;
}

void Socket::Listen(){
    int err = listen(m_fd, 10);
    if(err < 0) throw std::runtime_error("in listen");
}

Socket Socket::Accept(){
    int sock_accept = accept(m_fd, (struct sockaddr *)&m_address, (socklen_t*)&m_addrlen);
    if(sock_accept < 0) throw std::runtime_error("cannot accept on socket");

    return Socket(sock_accept, m_address, m_port);
}

void Socket::Read(char* req){
    //char buffer[30000] = {0};
    long valread;

    valread = read(m_fd, req, 1024);
    if(valread < 0) throw std::runtime_error("no bytes to read");
}

void Socket::Write(char *resp){
    write(m_fd, resp, strlen(resp));
}

void Socket::Close()
{
    if(m_fd != 0) close(m_fd);
    m_fd = 0;
}

