#include "ServerDirective.hpp"
#include <iostream>
ServerDirective::ServerDirective()
{
    listen.first = "0.0.0.0";
    listen.second = -1;
    server_name = "";

    validDirs[0] = "listen";
    validDirs[1] = "server_name";
    validDirs[2] = "index";
    validDirs[3] = "client_max_body_size";
    validDirs[4] = "root";
    validDirs[5] = "error_page";

}

void ServerDirective::listenValidation()
{
    // int port;
    // if (listen.size() == 4)
    // {
    //     std::stringstream ss(listen);

    // }
}

void ServerDirective::validate() const
{
    // if (listen.empty())
    //     throw std::runtime_error("Server must have listen directive");

    // if (root.empty())
    //     throw std::runtime_error("Server must have root directive");

    // //port validacia
    //  bool port_valid = true;
    //  int port = 0;
     
    // // Проверка что все символы - цифры
    // for (std::string::const_iterator it = listen.begin(); it != listen.end(); ++it)
    //     if (!isdigit(*it))
    //     {
    //         port_valid = false;
    //         break;
    //     }
     
    //  // Конвертация и проверка диапазона
    // if (port_valid)
    // {
    //     std::stringstream ss(listen);
    //     ss >> port;
    //     if (port <= 0 || port > 65535)
    //         port_valid = false;
    // }
     
    // if (!port_valid)
    //     throw std::runtime_error("Server: listen must be a valid port (1-65535)");

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
    std::cout<<"helloooo\n";
    for (std::vector<LocationDirective*>::iterator it = locdir.begin(); it != locdir.end(); ++it)
        delete *it;
}


/////////////setters//////////////
void    ServerDirective::setListen(const std::string& ipAndPort)
{
    // std::cout << "ekav->" << port << std::endl;
    // std::stringstream ss(port);
    // size_t _port;
    // ss >> _port;

    // listen = port;
    // std::cout << "listen is " << listen << std::endl;
    
    size_t indexOfVerjaket = ipAndPort.find(":");
    if (indexOfVerjaket != std::string::npos)
        listen.first = ipAndPort.substr(0, indexOfVerjaket);

    



}

void    ServerDirective::setServer_name(const std::string& name)
{
    server_name = name;
}

void    ServerDirective::setLocDir(LocationDirective *loc)
{
    locdir.push_back(loc);
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