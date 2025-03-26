#pragma once

class Server
{
private:
    ServerSocket servSock;//
                        //es sencem mtacum
    ClientSocket clientSock;//
    //bayc videoyum senca
    ListeningSocket *lisSock;
    virtual void accepter() = 0;
    virtual void handler() = 0;
    virtual void responder() = 0;
public:
    Server(int domainIP, int service, int protocol, int port, u_long interface, int backlog) : Server(int domainIP, int service, int protocol, int port, u_long interface, int backlog);
    virtual void launch() = 0;
    ~Server();
    //getters
    ListeningSocket * get_ListeningSocket() const;
};//inqy mihat tetser classa sarqe vory jarangela es Server classic,vori mejel accepter,responder funckcianery implementela,menq vaxy jokenq irar het inch class karlia stexcel u vonc implem=ntel dranq