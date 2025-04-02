#include "ServerDirective.hpp"

ServerDirective::ServerDirective()
{
    listen = "";
    server_name = "";

}

void ServerDirective::validate() const
{
    if (listen.empty())
        throw std::runtime_error("Server must have listen directive");

    if (root.empty())
        throw std::runtime_error("Server must have root directive");

    //port validacia
     bool port_valid = true;
     int port = 0;
     
    // Проверка что все символы - цифры
    for (std::string::const_iterator it = listen.begin(); it != listen.end(); ++it)
        if (!isdigit(*it))
        {
            port_valid = false;
            break;
        }
     
     // Конвертация и проверка диапазона
    if (port_valid)
    {
        std::stringstream ss(listen);
        ss >> port;
        if (port <= 0 || port > 65535)
            port_valid = false;
    }
     
    if (!port_valid)
        throw std::runtime_error("Server: listen must be a valid port (1-65535)");

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
    for (std::vector<LocationDirective*>::iterator it = locdir.begin(); it != locdir.end(); ++it)
        delete *it;
}


/////////////setters//////////////
void    ServerDirective::setListen(const std::string& port)
{
    std::stringstream ss(port);
    size_t _port;
    ss >> _port;
    listen = _port;
}

void    ServerDirective::setServer_name(const std::string& name)
{
    server_name = name;
}

// void    ServerDirective::setIndex(const std::string& value)
// {
//     index = value;
// }

// void    ServerDirective::setClient_max_body_size(const std::string& size)
// {
//     std::stringstream ss(size);
//     size_t sIze;
//     ss >> sIze;
//     client_max_body_size = sIze;
// }

// void    ServerDirective::setRoot(const std::string& rootPath)
// {
//     root = rootPath;
// }

// // void    ServerDirective::setError_pages(std::vecto<std::string> vec)
// // {
// //     int i = 0;
// //     std::vector<std::string>::iterator it = vec.begin();
// //     for(; it != vec.end(); ++it)
// //     {
// //         error_page.insert(std::make_pair(i, *it));
// //         i++;
// //     }
// // }