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
#include "CGI.hpp"

typedef std::map<std::string, std::string>  headers_map;
#define SIZE (1 << 14)

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
    std::string query;//uri-i mej ?-ic heto sax,ete ka
    int error_page_num;
    int epfd;
    std::map<std::string, std::string> form_data;//POST-i hamar
    headers_map headers;
    
    std::string get_response(std::string &method, char *buffer, int bytesRead);

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
    std::string get_method() const{return method;}
    size_t get_content_length() const{return contentLength;}
    std::string get_content_type() const{return MainContentType;}
    std::string get_query() const{return query;}
    std::string get_body() {return post_body;}
    headers_map &get_headers() {return headers;}
    
    int get_servIndex() const {return servIndex;}
    std::vector<ServerDirective *> get_servers() const {return servers;}
    std::string get_uri() const{return uri;}

    void    set_query(std::string query){this->query = query;}
    void    set_error_page_num(int error_num){this->error_page_num = error_num;}
    void    set_servIndex(int servIndex) {this->servIndex = servIndex;}
    void    set_method(std::string method) {this->method = method;}
    void    set_uri(std::string uri) {this->uri = uri;}

    std::string autoindex(std::string filePath);


};


//TODO resposnum content type-@ mery dnel
// error_page_num-ery init anel amen erroric araj throw aneluc