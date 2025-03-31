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
    
    public:
        std::vector<LocationDirective*> locdir;
        std::string validDirs[10] = {"path", "root", "allow_methods", "index", "autoindex", "return", "upload_dir", "client_max_body_size", "cgi_extension", "cgi_path"};
        ServerDirective();
        ~ServerDirective();
        virtual void validate() const;  // для валидации значений

};
