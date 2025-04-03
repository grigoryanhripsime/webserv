#include "Logger.hpp"

void Logger::printStatus(std::string status, std::string message)
{
    std::cout<<"["<<status<<"]: "<<message<<std::endl;
}
