#pragma once
#include "Socket.hpp"
class   ServerSocket : public Socket
{
    private:
        int serverFd;
        // int newSocket;//xndir exav esi texapoxeci TestServero mej
        int connection;
        virtual int connectToNetwork();
    public:
        ServerSocket(int domainIP, int service, int protocol, int port, unsigned long interface);
        int get_serverFd() const;
        virtual ~ServerSocket();
};