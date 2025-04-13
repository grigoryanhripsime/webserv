#pragma once
#include "Socket.hpp"
class   ServerSocket : public Socket
{
    private:
        int max_count_people_in_queue;
        int status;//example not bound,hishi nkary
        int connection;
        virtual int connectToNetwork();
    public:
        ServerSocket() : Socket() {}
        ServerSocket(int domainIP, int service, int protocol, int port, std::string ipAddress, int queueCount);
        // int get_serverFd() const;
        virtual ~ServerSocket();
};