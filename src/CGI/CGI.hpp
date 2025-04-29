#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <vector>

#if 0 // TODO: include true headers

#include "Request.hpp" // Assuming Request is a struct/class defined elsewhere

#endif 

class CGI {
private:
    void CGI_parse();
    void CGI_exec();
    void CGI_err();
    
    // Data members
    Request request;
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
    CGI();
    ~CGI();
    std::string CGI_handler(const Request& request, const std::string& interpreter, const std::string& script_path,
                           const std::string& script_name, const std::string& path_info,
                           const std::string& server_name, const std::string& server_port,
                           const std::string& remote_addr);
};

#endif // CGI_HPP