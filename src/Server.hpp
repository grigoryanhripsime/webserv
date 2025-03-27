#pragma once
#include "ListeningSocket.hpp"
class Server
{
    private:
        // ServerSocket servSock;//
                            //es sencem mtacum
        // ClientSocket clientSock;//
        //bayc videoyum senca
        ListeningSocket *lisSock;
        virtual void accepter() = 0;
        virtual void responder() = 0;
    public:
        Server(int domainIP, int service, int protocol, int port, unsigned long interface, int backlog);
        virtual void launch() = 0;
        virtual ~Server();
        //getters
        ListeningSocket* get_ListeningSocket() const;
};//inqy mihat tetser classa sarqe vory jarangela es Server classic,vori mejel accepter,responder funckcianery implementela,menq vaxy jokenq irar het inch class karlia stexcel u vonc implem=ntel dranq