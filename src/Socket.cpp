#include "Socket.hpp"

Socket::Socket(int domainIP, int typeOfService, int protocol, int port, u_long interface)
{
    // Define address structure
    // It is the data type that is used to store the address of the socket.
    serverAddr.sin_family = domainIP;
    serverAddr.sin_port = htons(port); //This function is used to convert the unsigned int from machine byte order to network byte order.
    serverAddr.sin_addr.s_addr = htonl(interface); //It is used when we don't want to bind our socket to any particular IP and instead make it listen to all the available IPs.
    
    // Establish socket
    sockFd = socket(domainIP, typeOfService, protocol);
    if (sockFd == -1) 
        throw std::runtime_error("Creating socket failed!");

    // // Establish network connection
    // // connection = connect_to_network();
    // if (bind(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    // {
    //     close(sockFd);
    //     throw std::runtime_error("Bind failed!");
    // }
    // if (listen(sockFd, 10) < 0)
    //     throw std::runtime_error("Listen failed!");
    // while(1)
    // {
    //     std::cout<<"\n+++++++ Waiting for new connection ++++++++\n\n";
    //     int new_socket = accept(sockFd, (struct sockaddr *)&serverAddr, (socklen_t*)&serverAddr);
    //     if (new_socket < 0)
    //         throw std::runtime_error("Accept failed!");
    //     char buffer[30000] = {0};
    //     long valread = read( new_socket , buffer, 30000);
    //     std::cout<<buffer;
    //     write(new_socket, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!", 74);
    //     printf("------------------Hello message sent-------------------");
    //     close(new_socket);
    // }
}

Socket::~Socket() 
{
    close(sockFd); 
}