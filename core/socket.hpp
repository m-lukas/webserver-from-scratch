#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include "../logger/logger.h"

enum SockOpt{
    REUSE_ADDRESS,
    TIMEOUT
};

class Socket
{
private:
    int m_socket;
    int m_addrlen, m_port;
    struct sockaddr_in m_address;
private:
    explicit Socket(int socket, sockaddr_in address, int port) : m_socket(socket), m_address(address), m_port(port) {};
    void setOpt(SockOpt opt, const void *value, size_t size);
public:
    Socket();
    Socket(Socket&& s) : m_socket{s.m_socket} {};
    Socket(const Socket& s) : m_socket{s.m_socket} {};
    Socket& operator=(Socket&& s) {
        if (&s != this) {
            m_socket = s.m_socket;
            s.m_socket = 0;
        }
        return *this;
    };
    ~Socket();

    void Bind(int port);
    void Close();
    void Listen();
    Socket Accept();
    long Read(char* req);    
    void Write(char *resp, size_t length);
    void SetOpt(SockOpt opt, int value);
    void SetTimeout(int seconds);
    bool InvalidDescriptor();
};
