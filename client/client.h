#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

class Client
{
private:
    int c_socket, c_addrlen;
    struct sockaddr_in c_address;
public:
    Client();
    ~Client();

    void Connect(char *addr, int port);
    void Send(char *req, char* resp);
    void Close();
};
