#pragma once
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"
#include <sys/epoll.h>
#include "DirectiveConfig.hpp"
#include <cstring>
#include <sstream>
#include <fstream>

#define METHOD_ERROR -77
#define MAX_EVENTS 10

class Servers
{
    private:
        DirectiveConfig *config;
        std::vector<ServerSocket> servSock;//eto server soket(bind,listen),a ne resultat accepta
        int epfd;
        std::string uri;//kara ev heto jnjvi
        int servIndex;
        // int locIndex;
    public:
        Servers(DirectiveConfig &dirConf);
        void setupEpoll();
        void runLoop();
        void acceptClient(int server_fd);
        void handleClientRequest(int client_fd);
        ~Servers();
        //utils///
        std::string get_location(char *buffer);
        int have_this_uri_in_our_current_server(int serverInd);
        

        int getServerThatWeConnectTo(std::string buffer);

        ///validation buffer
        std::string    if_received_request_valid(char *c_buffer);
        int method_is_valid(std::string first_line, int which_location, std::string& method);
        int check_this_metdod_has_in_appropriate_server(std::string method, int which_location);
        std::string    validation_of_the_first_line(char *c_buffer, std::string& method);
        int if_http_is_valid(char *c_buffer);

        void runningProcess();
        void connectingServerToSocket();
        std::string constructingResponce(std::string filePath);
        std::string constructingFilePath();

        int find_in_index_vectors_this_string(std::string left_part_of_filePath, std::vector<std::string> cur_loc_index);
        std::string get_need_string_that_we_must_be_pass_send_system_call(std::string filePath);
        /////////////////
        bool isDirectory(const std::string& path);
        bool isFile(const std::string& path);
        bool pathExists(const std::string& path);
        std::string getFilesInDirectory(std::string &path);

        std::string uri_is_file(std::string filePath);
        std::string uri_is_directory(std::string filePath);




};