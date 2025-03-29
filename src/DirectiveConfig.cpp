#include "DirectiveConfig.hpp"

DirectiveConfig::DirectiveConfig(const Directive& directives) : directives(directives)//aranc reference obj poxancely ahavor vat ban chka?copii mej copi chi kanchvi?
{
}
void DirectiveConfig::directiveValidation()
{
    if (!directives.values.empty())//es nra hamara vor serveri skopkecqic durs sovorakan(key-value) directive chunenanq=>vor sax pti nerarvac linen server{<sra mej>}
        throw DirectiveConfigException("In config file can't be simple directives outside of any block!");
    if (directives.blocks.empty())//gone mek hatik server(blok directive) piti lini, stex karlia hashvel qani hat server server ka,vortev ete mihata servery kara anun(server_name) chunena, isk ete mekic avela u dranq unen nuyn ip-n ev port-@ PARTADIRA unenan server_name vorpisi karananq iranc tarberakel 
        throw DirectiveConfigException("In config file you need to have at least one server block!");
    std::multimap<std::string, Directive *>::iterator it = directives.blocks.begin();
    for (; it != directives.blocks.end(); ++it)//blokayin directivneri vrov pttvum enq,hastatelu hamar vor miayn server{} blokayin directive ka
    {
        if (it->first != "server")//ete urish anunov blokayin directive a exception enq qcum
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
    std::cout<<"ehe\n";

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
