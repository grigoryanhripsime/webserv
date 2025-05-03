#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives) : directives(directives) {}

std::vector<ServerDirective *> DirectiveConfig::get_servers() const { return servers; }
std::map<std::pair<std::string, int>, std::vector<int> > DirectiveConfig::get_unique_listens() const { return unique_listens; }

DirectiveConfig::~DirectiveConfig()
{
    std::cout << "DirectiveConfig dtor is called\n";
    for (std::vector<ServerDirective*>::iterator it = servers.begin(); it != servers.end(); ++it)
        delete *it;
}

void DirectiveConfig::directiveValidation()
{
    // 1. Проверка, что нет простых директив вне блоков
    if (!directives.simpleDir.empty())
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");

    // 2. Проверка, что есть хотя бы один сервер
    if (directives.blocks.empty())
        throw DirectiveConfigException("In config file you need to have at least one server block!");

    // 3. Проверка всех блоков сервера
    std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
    ServerDirective *serv = NULL;
    try{
        for (; it != directives.blocks.end(); ++it)//es fory bolor serverneri(blokayin directiv) vrayov ancnelu hamara, pritom menak server enq unenalu,nu vapshe 2hat blokayin directiva linelu es proektum,bayc 2rdy`location-@ serveri meja linelu
        {
            // 3.1. Проверка, что блок именно server
            if (it->first != "server")
                throw DirectiveConfigException("You can't have any other main directive except for server!");
            Directive* serverBlock = it->second;//es serverBlock-nel uni simpleDir u blocks

            // 3.2. Проверка обязательных полей сервера
            if (serverBlock->simpleDir.find("listen") == serverBlock->simpleDir.end())
                throw DirectiveConfigException("Server directive must have 'listen' directive!");

            if (serverBlock->simpleDir.find("root") == serverBlock->simpleDir.end())
                throw DirectiveConfigException("Server directive must have 'root' directive!");

            // ServerDirective *serv = NULL;
            serv = fillServers(serverBlock);//chem jokum inchnel lcnelu
            ////////////////////////////////////////////////
            // 3.3. Проверка location блоков
            std::multimap<std::string, Directive*>::iterator itLoc = serverBlock->blocks.begin();
            for (; itLoc != serverBlock->blocks.end(); ++itLoc)
            {
                if (itLoc->first != "location")
                    throw DirectiveConfigException("You can't have any other nested directive except for location!");

                if (!itLoc->second->blocks.empty())
                    throw DirectiveConfigException("location block can't have any block directives inside!");

                // Проверка обязательного path у location
                if (itLoc->second->simpleDir.find("path") == itLoc->second->simpleDir.end())
                    throw DirectiveConfigException("Location directive must have 'path' specified!");

                LocationDirective *loc = fillLocationsn(itLoc->second);
                serv->setLocDir(loc);
            } 
            servers.push_back(serv);

        }
        std::cout << "axpeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeer\n";
        if (servers.size() > 1)
        {
            if_config_has_more_servers__whether_each_server_has_name_when_they_have_the_same_ip_and_port(servers);
            if (server_names_with_duplicate_IPs_must_be_different(unique_listens) < 0)
                throw std::runtime_error("Server names with duplicate IPs must be different.");
            //stugum enq vor unique listensi secondi`toist int-eri vectori(voronq irancic nerkayacnum enq serverneri indeqsnery voronc ip:port-ery nuynn en, iranc server_name-ery ampayman tarber linen,hakarak depqum karanq exception qcenq)
            //Armany asec vor ete nuyn ip port unen 1ic avel serverver configi error tanq,lav chi vor nayum enq ete anunnery tarber enq asum enq lav 
        }
        else
        {
            std::cout << "axpeeeeeeeeeeeeeeeeeeeeeeeeer\n";
        }
    }
    catch(...)
    {
        if (serv) 
            delete serv;//es toxy prkec????))))))))
        throw ;
    }
///////////////sharunakeli

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
        // servers[*it]->getServer_name();
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
    // 127.0.0.1:8080         0 2 3
    // 172.19.183.204:8089    1 4
 
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator pr = unique_listens.begin();
    for (; pr != unique_listens.end(); ++pr)
    {
        std::cout << "ip and port->" << (*pr).first.first << " " << (*pr).first.second << std::endl;
        std::vector<int> vec = (*pr).second;
        std::cout << "krknvox ip ev porteri indexnery->";
        for(size_t i = 0; i < vec.size(); ++i)
            std::cout << vec[i] << " ";
        std::cout << std::endl;
    }
}



ServerDirective *DirectiveConfig::fillServers(Directive *serverBlock)//&-@ maqrelem,local popoxakani reference veradardznely etqany xelaci ban chi:)
{
    ServerDirective *serv = new ServerDirective();
    try{
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
                    serv->setListen(itSimpleDir->second[0]);//itSimpleDir->second[0]
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
    catch(...)
    {
        delete serv;
        throw ;
    }
}

LocationDirective *DirectiveConfig::fillLocationsn(Directive *locationBlock)
{
    std::multimap<std::string, std::vector<std::string> >::iterator itSimpleDirLoc = locationBlock->simpleDir.begin();
    LocationDirective *loc = new LocationDirective();
    try{
        for (; itSimpleDirLoc != locationBlock->simpleDir.end(); ++itSimpleDirLoc)
        {
            if (itSimpleDirLoc->first != "index" && itSimpleDirLoc->first != "error_page" && itSimpleDirLoc->first != "allow_methods" && itSimpleDirLoc->first != "return" && itSimpleDirLoc->second.size() != 1)
                throw std::runtime_error("There are more than one value for " + itSimpleDirLoc->first);
            int i = 0;
            for (; i < 11; i++)
                if (itSimpleDirLoc->first == loc->validDirs[i])
                    break ;
            switch (i)
            {
                case 0:
                    loc->setPath(itSimpleDirLoc->second[0]);
                    break;
                case 1:
                    loc->setAllow_methods(itSimpleDirLoc->second);//vectora dra hamar
                    break;
                case 2:
                    loc->setAutoindex(itSimpleDirLoc->second[0]);
                    break;
                case 3:
                    loc->setRedirect(itSimpleDirLoc->second);//qani vor vectora
                    break;
                case 4:
                    loc->setUpload_dir(itSimpleDirLoc->second[0]);
                    break;
                case 5:
                    loc->setCgi_extension(itSimpleDirLoc->second[0]);
                    break;
                case 6:
                    loc->setCgi_path(itSimpleDirLoc->second[0]);
                    break;
                case 7:
                    loc->setIndex(itSimpleDirLoc->second);
                    break;                
                case 8:
                    loc->setClient_max_body_size(itSimpleDirLoc->second[0]);
                    break;
                case 9:
                    loc->setRoot(itSimpleDirLoc->second[0]);
                    break;
                case 10:
                    loc->setError_pages(itSimpleDirLoc->second);//mapa,bayc chem patkeracnum vonca petq lcnel,kam vapshe karoxa map-@ poxenq vector?
                    break;
                default:
                    throw std::runtime_error("Invalid directive for location");
            }
        }
        return loc;
    }
    catch(...)
    {
        delete loc;
        throw ;
    }
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
            for(std::vector<std::string>::iterator ott = (*ot)->getIndex().begin(); ott != (*ot)->getIndex().end(); ++ott)
            {
                std::cout << "index = " << *ott << std::endl;
            }
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
            std::cout << "cgi_extension = " << (*ot)->getCgi_extension() << std::endl;
            std::cout << "cgi_path = " << (*ot)->getCgi_path() << std::endl;
    
            
        }
    }
}
