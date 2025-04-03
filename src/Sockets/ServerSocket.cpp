#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int domainIP, int service, int protocol, int port, unsigned long interface) : Socket(domainIP, port, interface)
{
    serverFd = socket(domainIP, service, protocol);
    if (serverFd == -1) 
        throw std::runtime_error("Creating socket failed!");
    connection = connectToNetwork();//for the server this function will be call bind system call
    if (connection < 0)
        throw std::runtime_error("Error: bind");
}

int ServerSocket::connectToNetwork()
{
    struct sockaddr_in serverAddr = get_serverAddr();
    std::cout<<serverFd<<std::endl;
    return (bind(serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

int ServerSocket::get_serverFd() const
{
    return serverFd;
}

ServerSocket::~ServerSocket(){
    close(serverFd);
}