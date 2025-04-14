#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
#include <stdexcept>
#include <sstream>
#include "LocationDirective.hpp"
#include "ServerSocket.hpp"
#include <algorithm>

class LocationDirective;
class ServerDirective : public AGeneralDirective
{
    private:
        // std::string listen;//no default=>REQUIRE,   Must define port (no default)
        std::pair<std::string, int> listen;//<ipAdress, port>
        std::string server_name;//allow but not required
        std::vector<LocationDirective*> locdir;
        // ServerSocket sock;//Sorry Zipcik jan bayc esi hanelem,segfault uneinq(((

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
        std::vector<LocationDirective*>& getLocdir() {return locdir;}
        std::pair<std::string, int> getListen() const {return listen;}
        std::string getServer_name() const {return server_name;}

    ////listen validacia//////////
    void    check_and_set_port(const std::string& ipAndPort, size_t& indexOfVerjaket, bool flag);
    int     ip_part_contain_correct_integers(std::string ip_part);
    ///////////////////////////////
};
