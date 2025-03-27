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
    if (argc != 2)
    {
        std::cerr<<"Invalid number of arguments!\n";
        return 1;
    }
    FileReader fileReader(argv[1]);
    fileReader.fileToStr();
    std::string strFile = fileReader.getFileStr();
    TokenConfig tokenConfig(strFile);
    tokenConfig.fillingDirectives();
    try{
        TestServer t(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10);
        // Socket(AF_INET, SOCK_STREAM, 0, 8080, 10);
    }
    catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
}