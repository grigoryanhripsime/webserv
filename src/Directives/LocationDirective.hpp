#pragma once
#include <stdexcept>
#include "AGeneralDirective.hpp"
#include <vector>
#include <map>
#include <set>
#include <sys/stat.h>
#include <unistd.h>

class LocationDirective : public AGeneralDirective
{
    private:
        std::string path;//example `location /api {? obizatelno
        std::map<int, std::string> redirect;// return 301 http://google.com
        std::vector<std::string> allow_methods;//esi xi vectorov?,  default: GET (если не указано)
        std::string autoindex;//deafult off(false)
        std::string upload_dir;//default: "" (загрузка файлов отключена)
        std::string cgi_extension;//default: "" (CGI отключено)
        std::string cgi_path;//default: ""
        
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
        std::string validDirs[11]; //= {"path", "allow_methods", "autoindex", "redirect", "upload_dir", "cgi_extension", "cgi_path", "index", "client_max_body_size", "root", "error_pages"};//khaneq private esi
        
        LocationDirective();
        ~LocationDirective();
        virtual void validate() const;  // для валидации значений
        //getters////
        std::string getPath() const {return path;}
        std::map<int, std::string> getRedirect() {return redirect;}
        std::string getAutoindex() const {return autoindex;}
        std::string getUpload_dir() const {return upload_dir;}
        std::string getCgi_extension() const {return cgi_extension;}
        std::string getCgi_path() const {return cgi_path;}


        int validateRedirect(std::string codeStr, std::string url);

};
