#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int domainIP, int service, int protocol, int port, std::string ipAddress) : Socket(domainIP, service, protocol, port, ipAddress)
{
    (void)service;
    (void)protocol;
    (void)port;
    (void)ipAddress;
    connection = connectToNetwork();
    if (connection < 0)
        throw std::runtime_error("Error: connect");
}

int ClientSocket::connectToNetwork()
{
    struct sockaddr_in serverAddr = get_serverAddr();
    return (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

ClientSocket::~ClientSocket(){
    // std::cout << "ClientSocket dtor is called\n";

}