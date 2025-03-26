#include "Server.hpp"

Server::Server(int domainIP, int service, int protocol, int port, u_long interface, int backlog) : ListeningSocket(int domainIP, int service, int protocol, int port, u_long interface, int backlog)
{
    lisSock = new ListeningSocket(int domainIP, int service, int protocol, int port, u_long interface, int backlog);
}
