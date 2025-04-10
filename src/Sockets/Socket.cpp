#include "Socket.hpp"
// service ->typeOfService
Socket::Socket(int domainIP, int service, int protocol, int port, std::string ipAddress)
{
    sockfd = socket(domainIP, service, protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Creating socket failed!");
    // Define address structure
    // It is the data type that is used to store the address of the socket.
    
    std::cout << "domainIP = " << domainIP << std::endl;
    serverAddr.sin_family = domainIP;
    std::cout << "ekela = " << port << std::endl;
    serverAddr.sin_port = htons(port); //This function is used to convert the unsigned int from machine byte order to network byte order.
    // serverAddr.sin_addr.s_addr = htonl(ipAddress); //It is used when we don't want to bind our socket to any particular IP and instead make it listen to all the available IPs.
    if (inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address or Address not supported" << std::endl;
        // return -1; // Handle error
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
}