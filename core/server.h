#pragma once

#include "socket.h"

#define CHUNKSIZE 16*1024

class Server
{
private:
    bool m_running;
    Socket m_socket;
public:
    Server();

    void SendErrorResponse(Socket sock, int statuscode);
    void SendHTTPResponse(Socket sock, int statuscode, std::string message);

    void Listen(int port);
};
