#include "ServerDirective.hpp"

ServerDirective::ServerDirective() :
    locdir(),
    listen(""),
    server_name("") {
}

void ServerDirective::validate() const
{
    if (listen.empty()) {
        throw std::runtime_error("Server must have listen directive");
    }
    if (root.empty()) {
        throw std::runtime_error("Server must have root directive");
    }
    //port validacia
     bool port_valid = true;
     int port = 0;
     
     // Проверка что все символы - цифры
     for (std::string::const_iterator it = listen.begin(); it != listen.end(); ++it) {
         if (!isdigit(*it)) {
             port_valid = false;
             break;
         }
     }
     
     // Конвертация и проверка диапазона
     if (port_valid) {
        std::stringstream ss(listen);
        ss >> port;
         if (port <= 0 || port > 65535) {
             port_valid = false;
         }
     }
     
     if (!port_valid) {
         throw std::runtime_error("Server: listen must be a valid port (1-65535)");
     }


    // Проверка всех location
     for (std::vector<LocationDirective*>::const_iterator it = locdir.begin();
     it != locdir.end(); ++it) 
{
    if (*it == NULL) {
        throw std::runtime_error("Server: null location directive");
    }
    (*it)->validate();
}
}

ServerDirective::~ServerDirective() {
    for (std::vector<LocationDirective*>::iterator it = locdir.begin(); it != locdir.end(); ++it) {
        delete *it;
    }
}
