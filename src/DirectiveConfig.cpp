#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives)
{
    this->directives = directives;
}
void DirectiveConfig::directiveValidation()
{
    
    if (!directives.values.empty())
    {

        std::cout << directives.values.size() << std::endl;
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");
    }
        
    if (directives.blocks.empty())
        throw DirectiveConfigException("In config file you need to have at least one server block!");
    std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
    for (; it != directives.blocks.end(); ++it)
    {
        if (it->first != "server")
            throw DirectiveConfigException("You can't have any other main directive except for server!");
        std::multimap<std::string, Directive *>::iterator itLocations = it->second->blocks.begin();
        for (; itLocations != it->second->blocks.end(); ++it)
        {
            if (itLocations->first != "location")
                throw DirectiveConfigException("You can't have any other nested directive except for location!");
            if (!itLocations->second->blocks.empty())
                throw DirectiveConfigException("location block can;t have any block direvtives inside!");
                
        }
    }
}
void DirectiveConfig::fillServers()
{

}

DirectiveConfig::DirectiveConfigException::DirectiveConfigException(std::string err_msg)
{
    std::cout << "NEW SIG IN INIT\n";
    err_message = err_msg;
    std::cout << "NEW SIG AFTER INIT\n";
}

DirectiveConfig::DirectiveConfigException::~DirectiveConfigException() throw() {}


const char* DirectiveConfig::DirectiveConfigException::what() const throw()
{
    std::cout << "NEW SIG\n";
    return err_message.c_str();
}
