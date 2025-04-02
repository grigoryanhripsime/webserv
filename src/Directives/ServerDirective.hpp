#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
#include <stdexcept>
#include <sstream>
#include "LocationDirective.hpp"
class LocationDirective;
class ServerDirective : public AGeneralDirective{
    private:
    std::string listen;//no default=>REQUIRE,   Must define port (no default)
    std::string server_name;//allow but not required
    std::vector<LocationDirective*> locdir;
public:
    //setters
    void    setListen(const std::string& port);
    void    setServer_name(const std::string& name);
    public:
        // std::string validDirs[10] = {"path", "root", "allow_methods", "index", "autoindex", "return", "upload_dir", "client_max_body_size", "cgi_extension", "cgi_path"};
        std::string validDirs[6] = {"listen", "server_name", "index", "client_max_body_size", "root", "error_pages"};
        ServerDirective();
        ~ServerDirective();
        virtual void validate() const;  // для валидации значений
       ///getter///
       std::vector<LocationDirective*> get_locdir() const {return locdir;}

};
