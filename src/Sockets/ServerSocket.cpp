#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : Socket() {}

ServerSocket::ServerSocket(int domainIP, int service, int protocol, int port, std::string ipAddress, int queueCount) : Socket(domainIP, service, protocol, port, ipAddress)
{
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    connection = connectToNetwork();
    if (connection < 0)
        throw std::runtime_error("Error: binddd");
    //listen
    max_count_people_in_queue = queueCount;
    status = listen(get_socket(), max_count_people_in_queue);
    if (status < 0)
        throw std::runtime_error("Error:listen");
}

int ServerSocket::connectToNetwork()
{
    struct sockaddr_in serverAddr = get_serverAddr();
    return (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)));
}

ServerSocket::~ServerSocket(){
    // std::cout << "ServerSocket dtor is called\n";
}