#pragma once
#include <stdexcept>
#include "AGeneralDirective.hpp"
#include <vector>
#include <map>
#include <set>
class LocationDirective : public AGeneralDirective
{
private:
    std::string path;//example `location /api {? obizatelno
    std::map<int, std::string> redirect;// return 301 http://google.com
    // std::vector<std::string> redirect;
    std::vector<std::string> allow_methods;//esi xi vectorov?,  default: GET (если не указано)
    std::string autoindex;//deafult off(false)
    std::string upload_dir;//default: "" (загрузка файлов отключена)
    std::string cgi_extension;//default: "" (CGI отключено)
    std::string cgi_path;//default: ""
    // bool isAllDigits(const std::string& str);
    /////////setters//////////
public:
    void    setPath(const std::string& path);
    void    setAllow_methods(std::vector<std::string> methods);
    void    setAutoindex(const std::string& off_or_on);
    void    setRedirect(std::vector<std::string> red);
    void    setUpload_dir(const std::string& upload_dir);
    void    setCgi_extension(const std::string& extension);
    void    setCgi_path(const std::string& cgi_path);

public:
    std::string validDirs[11] = {"path", "allow_methods","autoindex", "redirect", "upload_dir", "cgi_extension", "cgi_path" "index", "client_max_body_size", "root", "error_pages"};//khaneq private esi
    
    LocationDirective();
    ~LocationDirective();
    virtual void validate() const;  // для валидации значений
    // /////////setters//////////
    // void    setPath(const std::string& path);
    // void    setAllow_methods(std::vector<std::string> methods);
    // void    setAutoindex(const std::string& off_or_on);
    // void    setRedirect(std::vector<std::string> red);
    // void    setUpload_dir(const std::string& upload_dir);
    // void    setCgi_extension(const std::string& extension);
    // void    setCgi_path(const std::string& cgi_path);

    // void    setIndex(const std::string& value);
    // void    setClient_max_body_size(const std::string& size);
    // void    setRoot(const std::string& value);
    // void    setError_pages(std::vector<std::string> pages);


};
