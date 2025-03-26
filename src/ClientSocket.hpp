#pragma once

class   ClientSocket : public Socket
{
private:
    int clientFd;
    int connection;
    virtual void connectToNetwork();
public:
    ClientSocket(int domainIP, int service, int protocol, int port, u_long interface);
    int get_clientFd() const;
};