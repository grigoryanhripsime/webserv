#pragma once

#include "AGeneralDirective.hpp"
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
        void directiveValidation();
        void fillServers();

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