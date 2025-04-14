#pragma once
#include "Socket.hpp"

class ClientSocket : public Socket
{
    private:
        int connection;
        virtual int connectToNetwork();
    public:
        ClientSocket(int domainIP, int service, int protocol, int port, std::string ipAddress);
        ~ClientSocket();
};