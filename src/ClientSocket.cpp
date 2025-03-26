#include "ClientSocket.hpp"

ClientSocket::ClientSocket(int domainIP, int service, int protocol, int port, u_long interface) : Server(int domainIP, int service, int protocol, int port, u_long interface)
{
    connection = connect_to_network();//for the server this function will be call bind system call
    if (connection < 0)
        throw std::runtime_error("Error: connect");
}


int ClientSocket::connectToNetwork()
{
    return (connect(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

int ClientSocket::get_clientFd() const
{
    return clientFd;
}
