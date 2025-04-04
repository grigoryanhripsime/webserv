#pragma once
#include "Socket.hpp"
class ClientSocket : public Socket
{
private:
    int clientFd;
    int connection;
    virtual int connectToNetwork();
public:
    ClientSocket(int domainIP, int service, int protocol, int port, unsigned long interface);
    int get_clientFd() const;
    ~ClientSocket();
};