#pragma once
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"
#include <sys/epoll.h>
#include "DirectiveConfig.hpp"
#include "Reques_header_validation.hpp"
#include <cstring>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>

#define METHOD_ERROR -77
#define MAX_EVENTS 10

class Servers
{
    private:
        DirectiveConfig *config;
        std::vector<ServerSocket> servSock;//eto server soket(bind,listen),a ne resultat accepta
        int epfd;
        int servIndex;
        std::string uri;//GET-i hamar
        std::string MainContentType;//POST-i hamar
        size_t contentLength;//POST-i hamar
        int error_page_num;
        std::string boundary;
        std::string file_type;
        std::string post_body;
        
    public:
        Servers(DirectiveConfig &dirConf);
        void setupEpoll();
        void runLoop();
        void acceptClient(int server_fd);
        void handleClientRequest(int client_fd);
        ~Servers();
        //utils///


        void    set_MainContentType(std::string line);
        void set_contentLength(std::string line);

        int getServerThatWeConnectTo(std::string buffer);

        ///validation buffer
        int if_http_is_valid(char *c_buffer);
        std::string post_method_tasovka(char *buffer);
        void    parse_post_request(char *buffer);
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

        void listFiles(std::string path, std::vector<std::string> &files);

        //deep
        std::string handle_multipart_upload(const std::string &upload_dir);
        std::string handle_simple_post(const std::string &upload_dir);
        void create_directories(const std::string &path);
};