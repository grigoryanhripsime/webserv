#pragma once
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"

#include "Request.hpp"

#define METHOD_ERROR -77
#define MAX_EVENTS 10

class Servers
{
    private:
        DirectiveConfig *config;
        std::vector<ServerSocket> servSock;//eto server soket(bind,listen),a ne resultat accepta
        int epfd;
        
    public:
        Servers(DirectiveConfig &dirConf);
        void setupEpoll();
        void runLoop();
        void acceptClient(int server_fd);
        ~Servers();
        

        void runningProcess();
        void connectingServerToSocket();
};