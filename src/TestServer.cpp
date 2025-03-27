#include "TestServer.hpp"
#include <cstring>
TestServer::TestServer(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog) : Server(domainIP, service, protocol, port, interface, backlog)
{
    launch();
}

void TestServer::accepter()
{
    struct sockaddr_in address = get_ListeningSocket()->get_serverAddr();
    unsigned int addrlen = sizeof(address);
    newSocket = accept(get_ListeningSocket()->get_serverFd(), (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (newSocket < 0)
        throw std::runtime_error("Error:accept");
    read(newSocket, buffer, 30000);
    std::cout << "buffer->" << buffer << std::endl;
}

void TestServer::responder()
{
    char hello[30] = "hello from server";
    write(newSocket, hello, strlen(hello));
    close(newSocket);
}

void TestServer::launch()
{
    while(true)
    {
        std::cout << "Waiting::" << std::endl;
        accepter();
        responder();
        std::cout << "Done::" << std::endl;

    }
}

int TestServer::get_newSocket() const
{
    return newSocket;
}

TestServer::~TestServer(){}