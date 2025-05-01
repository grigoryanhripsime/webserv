#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include "ServerDirective.hpp"


typedef enum e_status
{
    OK = 0,
    ERROR = 1,
    STATIC = 2,
    DINAMIC = 3
} status_type;

class Request_header_validation
{
    private:
        std::vector<ServerDirective *> servers;
        status_type status;
        int servIndex;
        std::string uri;//GET-i hamar
        std::string method;

    public:
        void    set_status(status_type status){this->status = status;}
        std::string get_uri() const;
        int get_servIndex() const; 
        std::string get_method() const; 
        status_type get_status() const {return status; }  

        void    status_handler();
        Request_header_validation(std::vector<ServerDirective *> servers);
        std::string    if_received_request_valid(char *c_buffer);
        std::string    validation_of_the_first_line(std::string line);
        int have_this_uri_in_our_current_server(int servIndex);
        int check_this_metdod_has_in_appropriate_server(std::string method, int which_location);
        int getServerThatWeConnectTo(std::string line);

        void get_validation(std::vector<std::string> lines);
        void post_validation(std::vector<std::string> lines);
        void delete_validation(std::vector<std::string> lines);

};