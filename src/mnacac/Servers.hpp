#pragma once
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"
#include <sys/epoll.h>
#include "DirectiveConfig.hpp"
#include <cstring>
#include <sstream>
#include <fstream>

#define MAX_EVENTS 10

class Servers
{
    private:
        DirectiveConfig *config;
        std::vector<ServerSocket> servSock;//eto server soket(bind,listen),a ne resultat accepta
        int epfd;
        std::string location;//kara ev heto jnjvi
        int servIndex;
    public:
        Servers(DirectiveConfig &dirConf);
        void setupEpoll();
        void runLoop();
        void acceptClient(int server_fd);
        void handleClientRequest(int client_fd);
        ~Servers();
        //utils///
        std::string get_location(char *buffer);
        int have_this_location_in_our_current_server(int serverInd);
        

        int getServerThatWeConnectTo(std::string buffer);

        ///validation buffer
        void    if_received_request_valid(char *c_buffer);
        int method_is_valid(std::string first_line, int which_location);
        int check_this_metdod_has_in_appropriate_server(std::string method, int which_location);
        void    validation_of_the_first_line(char *c_buffer);

};