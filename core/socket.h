#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

enum SockOpt{
    REUSE_ADDRESS
};

class Socket
{
private:
    int m_socket;
    int m_addrlen, m_port;
    struct sockaddr_in m_address;
private:
    explicit Socket(int socket, sockaddr_in address, int port) : m_socket(socket), m_address(address), m_port(port) {};
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
    void Read(char* req);    
    void Write(char *resp, size_t length);
    void SetOpt(SockOpt opt, int value);
};
