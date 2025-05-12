#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <vector>
class Request;

#include "Request.hpp" 

class CGI {
private:
    void CGI_parse();
    void CGI_exec();
    void CGI_err(const std::string& error, const std::string& cont_type, const std::string& body);
    
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
    std::string root;
    
    std::string _get_index(const std::vector<std::string> &index, const std::string& path);
public:
    CGI(Request * const request);
    ~CGI();
    std::string CGI_handler();
};

#endif // CGI_HPP