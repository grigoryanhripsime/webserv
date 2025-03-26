#pragma once

class ListeningSocket : public ServerSocket//chem jokum xia jarangum?nra hamar vor serverFd-n listeni mej karananq ogtagorcenq spomoshyu gettera?
{
private:
    int max_count_people_in_queue;
    int status;//example not bound,hishi nkary
public:
    ListeningSocket(int domainIP, int service, int protocol, int port, u_long interface, int backlog);
    void    start_listening();
};