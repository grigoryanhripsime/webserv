#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog) : ServerSocket(domainIP, service, protocol, port, interface)
{
    max_count_people_in_queue = backlog;
    status = start_listening();
    if (status < 0)
        throw std::runtime_error("Error:listen");
}


int ListeningSocket::start_listening()
{
    return (listen(get_serverFd(), max_count_people_in_queue));
}

ListeningSocket::~ListeningSocket(){}
