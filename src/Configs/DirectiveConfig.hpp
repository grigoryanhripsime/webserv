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
        std::map<std::pair<std::string, int>, std::vector<int> > unique_listens;
    public:
        DirectiveConfig(const Directive& directives);
        ~DirectiveConfig();
        void directiveValidation();
        ServerDirective *fillServers(Directive *serverBlock);
        LocationDirective *fillLocationsn(Directive *locationBlock);
        void printServers();
        
        class DirectiveConfigException : public std::exception
        {
            private:
            std::string err_message;
            public:
            DirectiveConfigException(std::string err_msg);
            ~DirectiveConfigException() throw();
            const char* what() const throw();
        };
        
        ////getter
        std::vector<ServerDirective *> get_servers() const {return servers;} //TODO: move this to cpp
        std::map<std::pair<std::string, int>, std::vector<int> > get_unique_listens() const {return unique_listens;}
        
        ////
        void if_config_has_more_servers__whether_each_server_has_name_when_they_have_the_same_ip_and_port(std::vector<ServerDirective *> servers);
        bool isIndexAlreadyIncluded(const std::vector<std::vector<int> >& result, int index);
        

};