#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

class Socket
{
private:
    int m_fd;
    int m_addrlen, m_port;
    struct sockaddr_in m_address;
private:
    explicit Socket(int fd, sockaddr_in address, int port) : m_fd(fd), m_address(address), m_port(port) {};
public:
    Socket();
    ~Socket();

    void Bind(int port);
    void Close();
    void Listen();
    Socket Accept();
    void Read(char* req);    
    void Write(char *resp);
};
