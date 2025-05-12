#include <iostream>
#include "FileReader.hpp"
#include "TokenConfig.hpp"
#include "Socket.hpp"
#include "Servers.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "DirectiveConfig.hpp"
#include "Directive.hpp"
int main(int argc, char *argv[])
{
    std::string fileName = argc == 2 ? argv[1] : "configs/webserv.conf";
    try 
    {
        FileReader fileReader(fileName);
        fileReader.fileToStr();
        std::string strFile = fileReader.getFileStr();
        
        TokenConfig tokenConfig(strFile);
        tokenConfig.fillingDirectives();
        Logger::printStatus("INFO", "Parsing of configuration file completed!");
        DirectiveConfig dirConf(tokenConfig.getDirectives());
        dirConf.directiveValidation();
        Servers obj(dirConf);
    } catch(std::exception &e)
    {
        Logger::printStatus("ERROR", e.what());
    }
}