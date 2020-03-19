#pragma once

#include "socket.h"

class Server
{
private:
    bool m_running;
    Socket m_socket;
public:
    Server();
    ~Server();

    void Listen(int port);
};
