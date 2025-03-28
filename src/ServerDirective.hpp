#pragma once

#include <iostream>
#include <vector>

class ServerDirective
{
    protected:
        std::string listen;
        std::string server_name;
        std::string root;
        std::string index;
        std::string client_max_body_size;
        std::string error_page;
        std::vector<std::string> allow_methods;
        std::string autoindex;
        std::string upload_dir;
        std::string cgi_extension;
        std::string cgi_path;

};
