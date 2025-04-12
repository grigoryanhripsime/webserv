#pragma once
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"
#include <sys/epoll.h>
#include "DirectiveConfig.hpp"
#include <cstring>
#include <sstream>
#include <fstream>

#define MAX_EVENTS 10

class Server
{
    private:
        DirectiveConfig *config;
        std::vector<ServerSocket> servSock;//eto server soket(bind,listen),a ne resultat accepta
        // std::vector<ClientSocket> clientSock;//a eto clinet soket(soket dlya connecta),bayc arden che
        int epfd;
        std::string location;//kara ev heto jnjvi
    public:
        Server(DirectiveConfig &dirConf);
        void setupEpoll();
        void runLoop();
        void acceptClient(int server_fd);
        void handleClientRequest(int client_fd);
        ~Server();
        //utils///
        std::string get_location(char *buffer);
};