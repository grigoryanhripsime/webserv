#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <vector>
class Request;
#if 1// TODO: include true headers

#include "Request.hpp" // Assuming Request is a struct/class defined elsewhere

#endif 

class CGI {
private:
    void CGI_parse();
    void CGI_exec();
    void CGI_err();
    
    // Data members
    Request * const request;
    std::string interpreter;
    std::string script_path;
    std::string script_name;
    std::string path_info;
    std::string server_name; 
    std::string server_port;
    std::string remote_addr;
    std::vector<std::string> env;
    std::string output;

public:
    CGI(Request * const request);
    ~CGI();
    std::string CGI_handler();
};

#endif // CGI_HPP