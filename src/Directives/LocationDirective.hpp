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
        std::vector<std::string> allow_methods;//esi xi vectorov?,  default: GET (если не указано)
        bool autoindex;//deafult off(false)
        std::map<int, std::string> redirect;// return 301 http://google.com
        std::string upload_dir;//default: "" (загрузка файлов отключена)
        std::string cgi_extension;//default: "" (CGI отключено)
        std::string cgi_path;//default: ""

    public:
        std::string validDirs[10] = {"path", "root", "allow_methods", "index", "autoindex", "return", "upload_dir", "client_max_body_size", "cgi_extension", "cgi_path"};
        
        LocationDirective();
        ~LocationDirective();
        virtual void validate() const;  // для валидации значений
};
