#include <iostream>
#include "FileReader.hpp"
#include "TokenConfig.hpp"
#include "Socket.hpp"
#include "TestServer.hpp"
#include "Server.hpp"
#include "ServerSocket.hpp"
#include "ListeningSocket.hpp"
#include "ClientSocket.hpp"

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
    } catch(std::exception &e)
    {
        Logger::printStatus("ERROR", e.what());
    }
    try{
        TestServer t(AF_INET, SOCK_STREAM, 0, 8090, INADDR_ANY, 10);
        // Socket(AF_INET, SOCK_STREAM, 0, 8080, 10);
    }
    catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
}