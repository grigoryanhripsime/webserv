#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives) : directives(directives) {}

std::vector<ServerDirective *> DirectiveConfig::get_servers() const { return servers; }
std::map<std::pair<std::string, int>, std::vector<int> > DirectiveConfig::get_unique_listens() const { return unique_listens; }

DirectiveConfig::~DirectiveConfig()
{
    // std::cout << "DirectiveConfig dtor is called\n";
    std::vector<ServerDirective*>::iterator it = servers.begin();
    for (; it != servers.end(); ++it)
        if (*it)
            delete *it;
}

void DirectiveConfig::directiveValidation()
{
    if (!directives.simpleDir.empty())
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");

    if (directives.blocks.empty())
        throw DirectiveConfigException("In config file you need to have at least one server block!");

    std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
    ServerDirective *serv = NULL;
        for (; it != directives.blocks.end(); ++it)
        {
            if (it->first != "server")
                throw DirectiveConfigException("You can't have any other main directive except for server!");
            Directive* serverBlock = it->second;

            if (serverBlock->simpleDir.find("listen") == serverBlock->simpleDir.end())
                throw DirectiveConfigException("Server directive must have 'listen' directive!");

            if (serverBlock->simpleDir.find("root") == serverBlock->simpleDir.end())
                throw DirectiveConfigException("Server directive must have 'root' directive!");

            serv = fillServers(serverBlock);

            std::multimap<std::string, Directive*>::iterator itLoc = serverBlock->blocks.begin();
            for (; itLoc != serverBlock->blocks.end(); ++itLoc)
            {
                if (itLoc->first != "location")
                    throw DirectiveConfigException("You can't have any other nested directive except for location!");

                if (!itLoc->second->blocks.empty() && !(itLoc->second->blocks.size() == 1 && itLoc->second->blocks.find("cgi_extension") != itLoc->second->blocks.end()))
                    throw DirectiveConfigException("location block can't have any block directives inside!");

                if (itLoc->second->simpleDir.find("path") == itLoc->second->simpleDir.end())
                    throw DirectiveConfigException("Location directive must have 'path' specified!");

                fillLocationsn(serv, itLoc->second);
            } 
        }
        if (servers.size() > 1)
        {
            if_config_has_more_servers__whether_each_server_has_name_when_they_have_the_same_ip_and_port(servers);
            if (server_names_with_duplicate_IPs_must_be_different(unique_listens) < 0)
                throw std::runtime_error("Server names with duplicate IPs must be different.");
        }
        else
            if_config_has_more_servers__whether_each_server_has_name_when_they_have_the_same_ip_and_port(servers);
        Logger::printStatus("INFO", "General validation of servers has passed successfully?!");
}

int DirectiveConfig::server_names_with_duplicate_IPs_must_be_different(std::map<std::pair<std::string, int>, std::vector<int> > unique_listens)
{
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it = unique_listens.begin();
    for(; it != unique_listens.end(); ++it)
    {
        if (checkDuplicates(it->second) < 0)
            return -1;
    }
    return 1;
}

int DirectiveConfig::checkDuplicates(std::vector<int> values)
{
    std::set<std::string> seen;

    for (size_t i = 0; i < values.size(); ++i)
    {
        if (seen.find(servers[i]->getServer_name()) != seen.end())
            return -1;
        seen.insert(servers[i]->getServer_name());
    }
    return 1;
}


void DirectiveConfig::if_config_has_more_servers__whether_each_server_has_name_when_they_have_the_same_ip_and_port(std::vector<ServerDirective *> servers)
{
    for (size_t i = 0; i < servers.size(); ++i)
    {
        std::map<std::pair<std::string, int>, std::vector<int> >::iterator itMap;
        itMap = unique_listens.find(servers[i]->getListen());
        if (itMap == unique_listens.end())
        {
            std::vector<int> indexes;
            indexes.push_back(i);
            unique_listens.insert(std::make_pair<std::pair<std::string, int>, std::vector<int> >(servers[i]->getListen(), indexes));
        }
        else
            itMap->second.push_back(i);
    }
}



ServerDirective *DirectiveConfig::fillServers(Directive *serverBlock)
{
    ServerDirective *serv = new ServerDirective();
    servers.push_back(serv);
        std::multimap<std::string, std::vector<std::string> >::iterator itSimpleDir = serverBlock->simpleDir.begin();
        for (; itSimpleDir != serverBlock->simpleDir.end(); ++itSimpleDir)
        {
            if (itSimpleDir->first != "index" && itSimpleDir->first != "error_page" && itSimpleDir->second.size() != 1)
                throw std::runtime_error("There are more than one value for " + itSimpleDir->first);
            int i = 0;
            for (; i < 6; i++)
                if (itSimpleDir->first == serv->validDirs[i])
                    break ;

            switch (i)
            {
                case 0:
                    serv->setListen(itSimpleDir->second[0]);
                    break;
                case 1:
                    serv->setServer_name(itSimpleDir->second[0]);
                    break;
                case 2:
                    serv->setIndex(itSimpleDir->second);
                    break;
                case 3:
                    serv->setClient_max_body_size(itSimpleDir->second[0]);
                    break;
                case 4:
                    serv->setRoot(itSimpleDir->second[0]);
                    break;
                case 5:
                    serv->setError_pages(itSimpleDir->second);
                    break;
                default:
                    throw std::runtime_error("Invalid directive for server");
            }
        }
        return serv;
}

LocationDirective *DirectiveConfig::fillLocationsn(ServerDirective *serv, Directive *locationBlock)
{
    std::multimap<std::string, std::vector<std::string> >::iterator itSimpleDirLoc = locationBlock->simpleDir.begin();
    std::multimap<std::string, Directive*>::iterator itCGIBlock = locationBlock->blocks.find("cgi_extension");
    LocationDirective *loc = new LocationDirective(serv);
    serv->setLocDir(loc);
        for (; itSimpleDirLoc != locationBlock->simpleDir.end(); ++itSimpleDirLoc)
        {
            if (itSimpleDirLoc->first != "index" && itSimpleDirLoc->first != "error_page" && itSimpleDirLoc->first != "allow_methods" && itSimpleDirLoc->first != "return" && itSimpleDirLoc->first != "cgi_extension" && itSimpleDirLoc->second.size() != 1)
                throw std::runtime_error("There are more than one value for " + itSimpleDirLoc->first);
            int i = 0;
            for (; i < 11; i++)
                if (itSimpleDirLoc->first == loc->validDirs[i])
                    break ;
            switch (i)
            {
                case 0:
                    loc->setPath(itSimpleDirLoc->second[0], serv);
                    break;
                case 1:
                    loc->setAllow_methods(itSimpleDirLoc->second);
                    break;
                case 2:
                    loc->setAutoindex(itSimpleDirLoc->second[0]);
                    break;
                case 3:
                    loc->setRedirect(itSimpleDirLoc->second);
                    break;
                case 4:
                    loc->setUpload_dir(itSimpleDirLoc->second[0]);
                    break;
                case 5:
                    loc->setCgi_extension(itSimpleDirLoc->second);
                    break;
                case 6:
                    loc->setIndex(itSimpleDirLoc->second);
                    break;                
                case 7:
                    loc->setClient_max_body_size(itSimpleDirLoc->second[0]);
                    break;
                case 8:
                    loc->setRoot(itSimpleDirLoc->second[0]);
                    break;
                default:
                    throw std::runtime_error("Invalid directive for location");
            }   
        }
        if (itCGIBlock != locationBlock->blocks.end()) 
            loc->setCgi_extension(itCGIBlock->second->simpleDir);
        return loc;
}

DirectiveConfig::DirectiveConfigException::DirectiveConfigException(std::string err_msg)
{
    err_message = err_msg;
}

DirectiveConfig::DirectiveConfigException::~DirectiveConfigException() throw() {}


const char* DirectiveConfig::DirectiveConfigException::what() const throw()
{
    return err_message.c_str();
}


void DirectiveConfig::printServers()
{
    std::vector<ServerDirective *>::iterator it = servers.begin();
    for(; it != servers.end(); ++it)
    {
        std::cout << "body size = " << (*it)->getBodySize() << std::endl;
        std::pair<std::string, int> par =  (*it)->getListen();
        std::cout << "listen-i ip = " <<par.first<<std::endl;
        std::cout << "listen-i port = " <<par.second<<std::endl;
        
        std::cout << "server_name = " << (*it)->getServer_name() <<std::endl;
        std::map<int, std::string> err  = (*it)->getError_pages();
        std::map<int, std::string>::iterator it1 = err.begin();
        for (; it1 != err.end(); ++it1)
        {
            std::cout << "error pages->" << it1->first << " " << it1->second << std::endl;
        }
        std::vector<LocationDirective*> locdir = (*it)->getLocdir();
        std::vector<LocationDirective*>::iterator ot = locdir.begin();
        for(; ot != locdir.end(); ++ot)
        {
            std::cout << "path = " << (*ot)->getPath() << std::endl;
            ////////
            std::map<int, std::string> red  = (*ot)->getRedirect();
            std::map<int, std::string>::iterator it = red.begin();
            for (; it != red.end(); ++it)
            {
                std::cout << "redirect->" << it->first << "  " << it->second << std::endl;
            }
            //////////
            // std::cout << "path = " << (*ot)->getPath() << std::endl;
            std::cout << "autoindex = " << (*ot)->getAutoindex() << std::endl;
            std::cout << "upload_dir = " << (*ot)->getUpload_dir() << std::endl;
            // std::cout << "cgi_extension = " << (*ot)->getCgi_extension() << std::endl;
            // std::cout << "cgi_path = " << (*ot)->getCgi_path() << std::endl;
    
            
        }
    }
}
