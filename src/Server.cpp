#include "Server.hpp"

Server::Server(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog)
{
    lisSock = new ListeningSocket(domainIP, service, protocol, port, interface, backlog);
}

ListeningSocket* Server::get_ListeningSocket() const
{
    return lisSock;
}

Server::~Server(){}