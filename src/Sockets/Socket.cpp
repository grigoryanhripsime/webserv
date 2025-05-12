#include "Socket.hpp"

Socket::Socket() {}

Socket::Socket(int domainIP, int service, int protocol, int port, std::string ipAddress)
{
    sockfd = socket(domainIP, service, protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Creating socket failed!");
    serverAddr.sin_family = domainIP;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
    }
}

int Socket::get_socket() const
{
    return sockfd;
}

struct sockaddr_in Socket::get_serverAddr() const
{
    return serverAddr;
}

Socket::~Socket() 
{
    // std::cout << "Socket dtor is called\n";

}