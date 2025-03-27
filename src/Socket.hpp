#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close()
#include <arpa/inet.h> //for htons()
#include <stdexcept>
class Socket
{
    private:
        struct sockaddr_in serverAddr;
    public:
        Socket(int domainIP, int port, unsigned long interface);
        virtual ~Socket();
        virtual int connectToNetwork() = 0;
        //getters
        struct sockaddr_in get_serverAddr() const;
}; 

