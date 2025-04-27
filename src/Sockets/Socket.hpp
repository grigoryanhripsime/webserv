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
    protected:
        int sockfd;//vor el dublikat chunenanq,serversoketum` server_fd, clientsoketum`client_fd
        struct sockaddr_in serverAddr;
    public:
        Socket();
        Socket(int domainIP, int service, int protocol, int port, std::string ipAddress);
        virtual ~Socket();
        virtual int connectToNetwork() = 0;
        //getters
        int get_socket() const;
        struct sockaddr_in get_serverAddr() const;

}; 

