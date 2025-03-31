#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives) : directives(directives)//aranc reference obj poxancely ahavor vat ban chka?copii mej copi chi kanchvi?
{
}
// void DirectiveConfig::directiveValidation()
// {
//     if (!directives.simpleDir.empty())//es nra hamara vor serveri skopkecqic durs sovorakan(key-value) directive chunenanq=>vor sax pti nerarvac linen server{<sra mej>}
//         throw DirectiveConfigException("In config file can't be simpleDir directives outside of any block!");
//     if (directives.blocks.empty())//gone mek hatik server(blok directive) piti lini, stex karlia hashvel qani hat server server ka,vortev ete mihata servery kara anun(server_name) chunena, isk ete mekic avela u dranq unen nuyn ip-n ev port-@ PARTADIRA unenan server_name vorpisi karananq iranc tarberakel 
//         throw DirectiveConfigException("In config file you need to have at least one server block!");
//     std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
//     for (; it != directives.blocks.end(); ++it)//blokayin directivneri vrov pttvum enq,hastatelu hamar vor miayn server{} blokayin directive ka
//     {
//         if (it->first != "server")//ete urish anunov blokayin directive a exception enq qcum
//             throw DirectiveConfigException("You can't have any other main directive except for server!");
//         std::multimap<std::string, Directive *>::iterator itLocations = it->second->blocks.begin();
//         for (; itLocations != it->second->blocks.end(); ++it)
//         {
//             if (itLocations->first != "location")
//                 throw DirectiveConfigException("You can't have any other nested directive except for location!");
//             if (!itLocations->second->blocks.empty())
//                 throw DirectiveConfigException("location block can;t have any block direvtives inside!");    
//         }
//     }
//     std::cout<<"ehe\n";

// }



void DirectiveConfig::directiveValidation()
{
    // 1. Проверка, что нет простых директив вне блоков
    if (!directives.simpleDir.empty()) {
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");
    }

    // 2. Проверка, что есть хотя бы один сервер
    if (directives.blocks.empty()) {
        throw DirectiveConfigException("In config file you need to have at least one server block!");
    }

    // 3. Проверка всех блоков сервера
    for (std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin(); it != directives.blocks.end(); ++it) {
        // 3.1. Проверка, что блок именно server
        if (it->first != "server") {
            throw DirectiveConfigException("You can't have any other main directive except for server!");
        }

        Directive* serverBlock = it->second;
        
        // 3.2. Проверка обязательных полей сервера
        if (serverBlock->simpleDir.find("listen") == serverBlock->simpleDir.end()) {
            throw DirectiveConfigException("Server directive must have 'listen' directive!");
        }
        if (serverBlock->simpleDir.find("root") == serverBlock->simpleDir.end()) {
            throw DirectiveConfigException("Server directive must have 'root' directive!");
        }

        // 3.3. Проверка location блоков
        for (std::multimap<std::string, Directive*>::iterator itLoc = serverBlock->blocks.begin(); itLoc != serverBlock->blocks.end(); ++itLoc) {
            if (itLoc->first != "location") {
                throw DirectiveConfigException("You can't have any other nested directive except for location!");
            }
            
            if (!itLoc->second->blocks.empty()) {
                throw DirectiveConfigException("location block can't have any block directives inside!");
            }

            // Проверка обязательного path у location
            // if (itLoc->second->simpleDir.find("path") == itLoc->second->simpleDir.end()) {
            //     throw DirectiveConfigException("Location directive must have 'path' specified!");
            // }
        }
    }
}


void DirectiveConfig::fillServers()
{

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
