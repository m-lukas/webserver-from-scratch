#pragma once

#include "socket.h"

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
