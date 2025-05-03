#pragma once
#include <stdexcept>
#include "AGeneralDirective.hpp"
#include <vector>
#include <map>
#include <set>
#include <cctype>
#include <sys/stat.h>  // Для stat(), S_IFDIR, S_IFREG
#include <unistd.h>    // Для access(), W_OK, X_OK

typedef std::map<std::string, std::string> cgi_ext_type;

class LocationDirective : public AGeneralDirective
{
    private:
        std::string path;//example `location /api {? obizatelno
        std::map<int, std::string> redirect;// return 301 http://google.com
        std::vector<std::string> allow_methods;//esi xi vectorov?,  default: GET (если не указано)
        std::string autoindex;//deafult off(false)
        std::string upload_dir;//default: "" (загрузка файлов отключена)
        // std::map<std::string, std::string> cgi_extension;
        cgi_ext_type cgi_extension;
        /////////setters//////////
    public:
        void    setPath(const std::string& path);
        void    setAllow_methods(std::vector<std::string> methods);
        void    setAutoindex(const std::string& off_or_on);
        void    setRedirect(std::vector<std::string> red);
        void    setUpload_dir(const std::string& upload_dir);
        void    setCgi_extension(const std::multimap<std::string, std::vector<std::string> >& extension);
        void    setCgi_extension(const std::vector<std::string>& extension);

    public:
        std::string validDirs[10]; //= {"path", "allow_methods", "autoindex", "redirect", "upload_dir", "cgi_extension", "cgi_path", "index", "client_max_body_size", "root", "error_pages"};//khaneq private esi
        
        LocationDirective();
        ~LocationDirective();
        virtual void validate() const;  // для валидации значений
        void setIndex(const std::vector<std::string>& indexVec);
        
        //getters////
        std::string getPath() const;
        std::vector<std::string> getAllow_methods() const;
        std::map<int, std::string> getRedirect();
        std::string getAutoindex() const;
        std::string getUpload_dir() const;
        // std::string getCgi_extension() const;
        std::string getCgi_path(const std::string& key);

        ///////path validacia
        bool isValidLocationPath(const std::string& path);
        int validateRedirect(std::string codeStr, std::string url);

};
