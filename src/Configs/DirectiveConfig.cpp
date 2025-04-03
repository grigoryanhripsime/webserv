#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives) : directives(directives) {}

void DirectiveConfig::directiveValidation()
{
    std::cout << "eee\n";
    // 1. Проверка, что нет простых директив вне блоков
    if (!directives.simpleDir.empty())
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");

    // 2. Проверка, что есть хотя бы один сервер
    if (directives.blocks.empty())
        throw DirectiveConfigException("In config file you need to have at least one server block!");

    // 3. Проверка всех блоков сервера
    std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
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

std::cout << "baste\n";
ServerDirective serv = fillServers(serverBlock);//chem jokum inchnel lcnelu
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

LocationDirective loc = fillLocationsn(itLoc->second);
serv.get_locdir().push_back(&loc);
}
std::vector<ServerDirective *>::iterator it = servers.begin();
for(; it != servers.end(); ++it)
{
    std::cout << "listen = " << (*it)->getListen()<<std::endl;
    std::cout << "server_name = " << (*it)->getServer_name() <<std::endl;
}
        servers.push_back(&serv);
    }

    ///printf anenq serverdirectivy ev location directivy tenanq chishtenq grel setternery u vapshe es funkcian 
}


ServerDirective DirectiveConfig::fillServers(Directive *serverBlock)//&-@ maqrelem,local popoxakani reference veradardznely etqany xelaci ban chi:)
{
    ServerDirective serv;
    std::multimap<std::string, std::vector<std::string>>::iterator itSimpleDir = serverBlock->simpleDir.begin();
    for (; itSimpleDir != serverBlock->simpleDir.end(); ++itSimpleDir)
    {
        std::cout << "amot\n";
        // if (itSimpleDir->first != "allow_methods" && itSimpleDir->first != "return" && itSimpleDir->second.size() != 1)
        //     throw std::runtime_error("There are more than one value for " + itSimpleDir->first);
        int i = 0;
        for (; i < 6; i++)
        {
            if (itSimpleDir->first == serv.validDirs[i])
            {
                break ;
            }
            std::cout << "hres->" << itSimpleDir->first << std::endl;
        }

        switch (i)
        {
            case 0:
                serv.setListen(itSimpleDir->second[0]);//itSimpleDir->second[0]//erb setternery dnum em private sranq compile chi linum, karoxa vor es poxancvox parametrery getter ov poxancem dzvi??
                break;
            case 1:
                serv.setServer_name(itSimpleDir->second[0]);
                break;
            case 2:
                serv.setIndex(itSimpleDir->second[0]);
                break;
            case 3:
                serv.setClient_max_body_size(itSimpleDir->second[0]);
                break;
            case 4:
                serv.setRoot(itSimpleDir->second[0]);
                break;
            case 5:
                serv.setError_pages(itSimpleDir->second);
                break;
            default:
                throw std::runtime_error("Invalid directive for server");
        }
    }
    return serv;
}

LocationDirective DirectiveConfig::fillLocationsn(Directive *locationBlock)
{
    std::multimap<std::string, std::vector<std::string>>::iterator itSimpleDirLoc = locationBlock->simpleDir.begin();
    LocationDirective loc;
    for (; itSimpleDirLoc != locationBlock->simpleDir.end(); ++itSimpleDirLoc)
    {
        // if (itSimpleDirLoc->first != "allow_methods" && itSimpleDirLoc->first != "return" && itSimpleDirLoc->second.size() != 1)
        //     throw std::runtime_error("There are more than one value for " + itSimpleDirLoc->first);
        int i = 0;
        for (; i < 11; i++)
            if (itSimpleDirLoc->first == loc.validDirs[i])
                break ;
        switch (i)
        {
            case 0:
                loc.setPath(itSimpleDirLoc->second[0]);
                break;
            case 1:
                loc.setAllow_methods(itSimpleDirLoc->second);//vectora dra hamar
                break;
            case 2:
                loc.setAutoindex(itSimpleDirLoc->second[0]);
                break;
            case 3:
                loc.setRedirect(itSimpleDirLoc->second);//qani vor vectora
                break;
            case 4:
                loc.setUpload_dir(itSimpleDirLoc->second[0]);
                break;
            case 5:
                loc.setCgi_extension(itSimpleDirLoc->second[0]);
                break;
            case 6:
                loc.setCgi_path(itSimpleDirLoc->second[0]);
                break;
            case 7:
                loc.setIndex(itSimpleDirLoc->second[0]);
                break;                
            case 8:
                loc.setClient_max_body_size(itSimpleDirLoc->second[0]);
                break;
            case 9:
                loc.setRoot(itSimpleDirLoc->second[0]);
                break;
            case 10:
                loc.setError_pages(itSimpleDirLoc->second);//mapa,bayc chem patkeracnum vonca petq lcnel,kam vapshe karoxa map-@ poxenq vector?
                break;
            default:
                throw std::runtime_error("Invalid directive for location");
        }
    }
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
