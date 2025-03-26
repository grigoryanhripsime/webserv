#pragma once

class   ServerSocket : public Socket
{
private:
    int serverFd;
    int newSocket;
    int connection;
    virtual void connectToNetwork();
public:
    ServerSocket(int domainIP, int service, int protocol, int port, u_long interface);
    int get_serverFd() const;
};