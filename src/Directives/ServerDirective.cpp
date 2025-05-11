#include "ServerDirective.hpp"
#include <iostream>

ServerDirective::ServerDirective()
{
    index.push_back("index.html");//qani vor stex default-ov push enq anum index.html-n ,setIndex-i mej clear enq anum vectory vortev ete kanchvela setIndex serveri hamar uremn inqy uni index directiv=>default depqy el chi ogtagorcvelu
    listen.first = "0.0.0.0";
    listen.second = -1;
    server_name = "";

    validDirs[0] = "listen";
    validDirs[1] = "server_name";
    validDirs[2] = "index";
    validDirs[3] = "client_max_body_size";
    validDirs[4] = "root";
    validDirs[5] = "error_page";

    error_pages[400] = "error_pages/400.html";
    error_pages[401] = "error_pages/401.html";
    error_pages[402] = "error_pages/402.html";
    error_pages[403] = "error_pages/403.html";
    error_pages[404] = "error_pages/404.html";
    error_pages[405] = "error_pages/405.html";
    error_pages[413] = "error_pages/413.html";
    error_pages[415] = "error_pages/415.html";
    error_pages[422] = "error_pages/415.html";
    error_pages[500] = "error_pages/500.html";
}
std::map<int, std::string>& ServerDirective::getError_pages() { return error_pages; }
std::vector<LocationDirective*>& ServerDirective::getLocdir() { return locdir; }
std::pair<std::string, int> ServerDirective::getListen() const { return listen; }
std::string ServerDirective::getServer_name() const { return server_name; }
int ServerDirective::get_locIndex() const { return locIndex; }

void ServerDirective::validate() const
{
    // if (listen.empty())
    //     throw std::runtime_error("Server must have listen directive");

    // if (root.empty())
    //     throw std::runtime_error("Server must have root directive");
     
    // Проверка всех location
    for (std::vector<LocationDirective*>::const_iterator it = locdir.begin(); it != locdir.end(); ++it) 
    {
        if (*it == NULL)
            throw std::runtime_error("Server: null location directive");
        (*it)->validate();
    }
}

ServerDirective::~ServerDirective()
{
    std::cout << "ServerDirective dtor is called\n";
    for (std::vector<LocationDirective*>::iterator it = locdir.begin(); it != locdir.end(); ++it)
        if (*it)
            delete *it;
}

/////////////setters//////////////

void    ServerDirective::setError_pages(std::vector<std::string> pages)
{

    std::vector<std::string>::iterator it = pages.begin();
    if (is_valid_index_value(*(pages.end() - 1)) == false)
        throw std::runtime_error("Invalid last value(path) of error_page" + (*(pages.end() - 1)));
    for(; it != pages.end() - 1; ++it)
    {
        if ((*it).find_first_not_of("0123456789") != std::string::npos)
            throw std::runtime_error("Error page number contain other simbols except digits` " + (*it));
        std::stringstream ss(*it);
        int ind;
        if (isAllDigits(*it))
        {
            ss >> ind;
            //stugel indexi hamar vor error page-eri tverina patkanum te che
            if (ind < 300 || ind >= 600)//коды 1xx и 2xx обычно не перенаправляются на error_page).
                throw std::runtime_error("part of error page number is incorrect");
            error_pages[ind] = pages[pages.size() - 1];
            ss.clear();
        }
        else
            throw std::runtime_error("error_page values must be only digits except last");
    }
}
////////////////listen validacia/////////
int     ServerDirective::ip_part_contain_correct_integers(std::string ip_part)
{
    ip_part.insert(ip_part.end(), '.');//127.33.222.1000.(es verji kety avelacnum em)
    size_t ind_of_dot;
    while(!ip_part.empty())
    {
        ind_of_dot = ip_part.find('.');
        if (ind_of_dot == std::string::npos)
            return 1;
        std::string cur_part = ip_part.substr(0, ind_of_dot);
        if (cur_part.empty())
            throw std::runtime_error("something wrong in ip_part dots" + cur_part);
        std::stringstream ss(cur_part);
        int octet = 0;
        ss >> octet;
        if (!(octet >= 0 && octet <= 255))
            return -1;
        ip_part.erase(0, ind_of_dot + 1);
    }
    return 1;
}

void    ServerDirective::check_and_set_port(const std::string& ipAndPort, size_t& indexOfVerjaket, bool flag)
{
    if (flag)
    {
        std::string port = ipAndPort.substr(indexOfVerjaket + 1, ipAndPort.size());
        if (isAllDigits(port))
        {
            std::stringstream ss(port);
            size_t value;
            ss >> value;
            if (value <= 0 || value > 65535)
                throw std::runtime_error("port value must be in range (0,65535]");
            listen.second = value;
        }
    }
    else
    {
        if (isAllDigits(ipAndPort))
        {
            std::stringstream ss(ipAndPort);
            size_t value;
            ss >> value;
            if (value <= 0 || value > 65535)
                throw std::runtime_error("port value must be in range (0,65535]");
            listen.second = value;
        }
    }
}

void    ServerDirective::setListen(const std::string& ipAndPort)
{   
    size_t indexOfVerjaket = ipAndPort.find(":");
    if (indexOfVerjaket != std::string::npos)
    {
        std::string ip_part = ipAndPort.substr(0, indexOfVerjaket);
        if(std::count(ip_part.begin(), ip_part.end(), '.') != 3)
            throw std::runtime_error("in ip . must be 3hat" + ip_part);
        if (ip_part_contain_correct_integers(ip_part) < 0)
            throw std::runtime_error("ip octets is not range [0-255]" + ip_part);
        listen.first = ip_part;
        check_and_set_port(ipAndPort, indexOfVerjaket, true);
    }
    else
        check_and_set_port(ipAndPort, indexOfVerjaket, false);
}

/////////////done losten validaca////////////////
void    ServerDirective::setServer_name(const std::string& name)
{
    server_name = name;
}

void    ServerDirective::setLocDir(LocationDirective *loc)
{
    locdir.push_back(loc);
}

bool ServerDirective::pathExists(const std::string& path)
{
    return (access(path.c_str(), F_OK) != -1);
}

bool ServerDirective::isDirectory(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}
