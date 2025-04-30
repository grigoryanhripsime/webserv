#pragma once
#include <sys/epoll.h>
#include "DirectiveConfig.hpp"
#include "Request_header_validation.hpp"
#include <cstring>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>
#include <sstream>
#include <dirent.h>
#include <limits>
class Request
{
private:
    std::vector<ServerDirective *> servers;
    int servIndex;
    int locIndex;
    std::string method;
    std::string uri;//GET-i hamar
    std::string MainContentType;//POST-i hamar
    size_t contentLength;//POST-i hamar
    std::string boundary;//POST-i hamar
    std::string post_body;//POST-i hamar
    std::string file_type;//POST-i hamar
    int error_page_num;
    int epfd;
    std::map<std::string, std::string> form_data;//POST-i hamar
    
public:
    Request(std::vector<ServerDirective *> servers);
    ~Request(){};
    std::string handle_multipart_upload(const std::string &upload_dir);
    std::string handle_simple_post(const std::string &upload_dir);
    void create_directories(const std::string &path);
    bool isDirectory(const std::string& path);
    bool isFile(const std::string& path);
    bool pathExists(const std::string& path);
    std::string getFilesInDirectory(std::string &path);
    std::string uri_is_file(std::string filePath);
    std::string uri_is_directory(std::string filePath);
    std::string post_method_tasovka(char *buffer, int bytesRead);
    void    parse_post_request(char *buffer, int bytesRead);
    void listFiles(std::string path, std::vector<std::string> &files);
    int find_in_index_vectors_this_string(std::string left_part_of_filePath, std::vector<std::string> cur_loc_index);
    std::string get_need_string_that_we_must_be_pass_send_system_call(std::string filePath);
    std::string constructingResponce(std::string filePath);
    void    set_MainContentType(std::string line);
    void    set_contentLength(std::string line);
    void handleClientRequest(int client_fd);
    int if_http_is_valid(char *c_buffer);
    std::string getFilepath(std::string uri);
    std::string urlDecode(const std::string &str);
    void parseUrlEncodedForm(const std::string &body);
};


