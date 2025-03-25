#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close()

class Socket
{
    private:
        struct sockaddr_in serverAddr;
        int sockFd;
        int newSocket;

    public:
        Socket(int domainIP, int service, int protocol, int port, u_long interface);
        virtual ~Socket();
        virtual void connectToNetwork()
};