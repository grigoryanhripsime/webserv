#pragma once
#include <stdexcept>
#include "AGeneralDirective.hpp"
#include "ServerDirective.hpp"
#include <vector>
#include <map>
#include <set>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>

typedef std::map<std::string, std::string> cgi_ext_type;

class ServerDirective;

class LocationDirective : public AGeneralDirective
{
private:
    std::string path;
    std::map<int, std::string> redirect;
    std::vector<std::string> allow_methods;
    std::string autoindex;
    std::string upload_dir;
    cgi_ext_type cgi_extension;
public:
    /////////setters//////////
    void    setPath(const std::string& path, ServerDirective *serv);
    void    setAllow_methods(std::vector<std::string> methods);
    void    setAutoindex(const std::string& off_or_on);
    void    setRedirect(std::vector<std::string> red);
    void    setUpload_dir(const std::string& upload_dir);
    void    setCgi_extension(const std::multimap<std::string, std::vector<std::string> >& extension);
    void    setCgi_extension(const std::vector<std::string>& extension);
    std::string validDirs[9];
public:
        
    LocationDirective(ServerDirective *serv);
    ~LocationDirective();
    virtual void validate() const;
    void setIndex(const std::vector<std::string> indexVec);
    
    //getters////
    std::string getPath() const;
    std::vector<std::string> getAllow_methods() const;
    std::map<int, std::string> getRedirect();
    std::string getAutoindex() const;
    std::string getUpload_dir() const;
    std::string getCgi_path(const std::string& key);

    ///////path validation
    bool isValidLocationPath(const std::string& path);
    int validateRedirect(std::string codeStr, std::string url);
    int is_not_inserted(ServerDirective *serv, const std::string& path);

};
