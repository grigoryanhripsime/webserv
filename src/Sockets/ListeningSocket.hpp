#pragma once
#include "ServerSocket.hpp"
#include "Socket.hpp"
class ListeningSocket : public ServerSocket//chem jokum xia jarangum?nra hamar vor serverFd-n listeni mej karananq ogtagorcenq spomoshyu gettera?
{
private:
    int max_count_people_in_queue;
    int status;//example not bound,hishi nkary
public:
    ListeningSocket(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog);
    int    start_listening();
    ~ListeningSocket();
};