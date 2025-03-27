#pragma once
#include "Server.hpp"
class TestServer : public Server
{
private:
    char buffer[30000];
    // int new_socket;
    int newSocket;
    virtual void accepter();
    virtual void responder();
public:
    TestServer(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog);
    virtual void launch();
    ~TestServer();
    //getters
    // ListeningSocket* get_ListeningSocket() const;
    int get_newSocket() const;
};