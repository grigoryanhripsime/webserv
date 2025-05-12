#pragma once

#include "AGeneralDirective.hpp"
#include <vector>
#include <stdexcept>
#include <sstream>
#include "LocationDirective.hpp"
#include "ServerSocket.hpp"
#include <algorithm>
#include <set>

class LocationDirective;

class ServerDirective : public AGeneralDirective
{
    private:
        std::map<int, std::string> error_pages;
        std::pair<std::string, int> listen;
        std::string server_name;
        std::vector<LocationDirective*> locdir;
        int locIndex;
        std::set<std::string> files;

    public:
        //setters
        void    setError_pages(std::vector<std::string> pages);
        void    setListen(const std::string& ipAndPort);
        void    setServer_name(const std::string& name);
        void    setLocDir(LocationDirective *loc);
        void    setLocIndex(int ind) {locIndex = ind;}
        std::string validDirs[6];
        
    public:
        ServerDirective();
        ~ServerDirective();
        virtual void validate() const;

        ///getter///
        std::map<int, std::string>& getError_pages();
        std::vector<LocationDirective*>& getLocdir();
        std::pair<std::string, int> getListen() const;
        std::string getServer_name() const;
        int get_locIndex() const;
        std::string get_file(const std::string &key) const;

        ////listen validacia//////////
        void    check_and_set_port(const std::string& ipAndPort, size_t& indexOfVerjaket, bool flag);
        int     ip_part_contain_correct_integers(std::string ip_part);
        ///////////////////////////////
        bool pathExists(const std::string& path);
        bool isDirectory(const std::string& path);

        void    set_file(const std::string &key) {files.insert(key);}
};
