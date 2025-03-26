#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close()
#include <arpa/inet.h> //for htons()

class Socket
{
    private:
        struct sockaddr_in serverAddr;

    public:
        Socket(int domainIP, int service, int protocol, int port, u_long interface);
        virtual ~Socket();
        virtual void connectToNetwork() = 0;
        //getters
        struct sockaddr_in get_serverAddr() const;
}; 

