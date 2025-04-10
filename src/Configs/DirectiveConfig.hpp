#pragma once

#include "AGeneralDirective.hpp"
#include "ServerDirective.hpp"
#include "LocationDirective.hpp"
class ServerDirective;
#include "Directive.hpp"
class Directive;

class DirectiveConfig
{
    private:
        Directive directives;
        std::vector<ServerDirective *> servers;
    public:
        DirectiveConfig(const Directive& directives);
        ~DirectiveConfig();
        void directiveValidation();
        ServerDirective *fillServers(Directive *serverBlock);
        LocationDirective *fillLocationsn(Directive *locationBlock);
        void printServers();
        std::vector<ServerDirective *> get_servers() const {return servers;}

        class DirectiveConfigException : public std::exception
        {
            private:
                std::string err_message;
            public:
                DirectiveConfigException(std::string err_msg);
                ~DirectiveConfigException() throw();
                const char* what() const throw();
        };

};