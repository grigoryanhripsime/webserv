#include "Socket.hpp"
// service ->typeOfService
Socket::Socket(int domainIP, int port, unsigned long interface)
{
    // Define address structure
    // It is the data type that is used to store the address of the socket.
    serverAddr.sin_family = domainIP;
    serverAddr.sin_port = htons(port); //This function is used to convert the unsigned int from machine byte order to network byte order.
    serverAddr.sin_addr.s_addr = htonl(interface); //It is used when we don't want to bind our socket to any particular IP and instead make it listen to all the available IPs.
}

struct sockaddr_in Socket::get_serverAddr() const
{
    return serverAddr;
}

Socket::~Socket() 
{
}