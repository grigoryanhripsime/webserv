#pragma once
#include "Socket.hpp"
class   ServerSocket : public Socket
{
    private:
        int max_count_people_in_queue;
        int status;
        int connection;
        virtual int connectToNetwork();
    public:
        ServerSocket();
        ServerSocket(int domainIP, int service, int protocol, int port, std::string ipAddress, int queueCount);
        virtual ~ServerSocket();
};