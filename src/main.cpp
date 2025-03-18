#include <iostream>
#include "FileReader.hpp"
#include "TokenConfig.hpp"

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
}