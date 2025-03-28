#include <iostream>
#include "FileReader.hpp"
#include "TokenConfig.hpp"
#include "Socket.hpp"
#include "TestServer.hpp"
#include "Server.hpp"
#include "ServerSocket.hpp"
#include "ListeningSocket.hpp"
#include "ClientSocket.hpp"
#include "DirectiveConfig.hpp"

int main(int argc, char *argv[])
{
    std::string fileName = argc == 2 ? argv[1] : "configs/default.conf";
    try 
    {
        FileReader fileReader(fileName);
        fileReader.fileToStr();
        std::string strFile = fileReader.getFileStr();
        TokenConfig tokenConfig(strFile);
        tokenConfig.fillingDirectives();
        DirectiveConfig dirConf(tokenConfig.getDirectives());
        dirConf.directiveValidation();
        
    } catch(std::exception &e)
    {
        std::cout << "SIG\n";
        Logger::printStatus("ERROR", e.what());
    }
    std::cout<<"ehe\n";
    // try{
    //     TestServer t(AF_INET, SOCK_STREAM, 0, 8091, INADDR_ANY, 10);
    //     // Socket(AF_INET, SOCK_STREAM, 0, 8080, 10);
    // }
    // catch(std::runtime_error& e)
    // {
    //     Logger::printStatus("ERROR", e.what());
    // }
}