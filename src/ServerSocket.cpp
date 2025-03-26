#include "ServerSocket.hpp"

ServerSocket::ServerSocket(int domainIP, int service, int protocol, int port, u_long interface) : Server(int domainIP, int service, int protocol, int port, u_long interface)
{
    connection = connect_to_network();//for the server this function will be call bind system call
    if (connection < 0)
        throw std::runtime_error("Error: bind");
}

int ServerSocket::connectToNetwork()
{
    return (bind(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

int ServerSocket::get_serverFd() const
{
    return serverFd;
}
