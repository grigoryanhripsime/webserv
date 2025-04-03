#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int domainIP, int service, int protocol, int port, unsigned long interface) : Socket(domainIP, port, interface)
{
    // (void)domainIP;
    (void)service;
    (void)protocol;
    (void)port;
    (void)interface;
    clientFd = socket(domainIP, service, protocol);
    if (clientFd == -1) 
        throw std::runtime_error("Creating socket failed!");
    connection = connectToNetwork();//for the server this function will be call bind system call
    if (connection < 0)
        throw std::runtime_error("Error: connect");
}

int ClientSocket::connectToNetwork()
{
    struct sockaddr_in serverAddr = get_serverAddr();
    return (connect(clientFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

int ClientSocket::get_clientFd() const
{
    return clientFd;
}

ClientSocket::~ClientSocket(){}