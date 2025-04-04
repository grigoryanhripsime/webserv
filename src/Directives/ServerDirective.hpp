#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
#include <stdexcept>
#include <sstream>
#include "LocationDirective.hpp"
class LocationDirective;
class ServerDirective : public AGeneralDirective
{
    private:
        // std::string listen;//no default=>REQUIRE,   Must define port (no default)
        std::pair<std::string, int> listen;
        std::string server_name;//allow but not required
        std::vector<LocationDirective*> locdir;
    public:
        //setters
        void    setListen(const std::string& ipAndPort);
        void    setServer_name(const std::string& name);
        void    setLocDir(LocationDirective *loc);
        std::string validDirs[6];
    public:
        ServerDirective();
        ~ServerDirective();
        virtual void validate() const;  // для валидации значений

        ///getter///
        std::vector<LocationDirective*> getLocdir() const {return locdir;}
        std::pair<std::string, int> getListen() const {return listen;}
        std::string getServer_name() const {return server_name;}

        //validation//
        void listenValidation();



};
