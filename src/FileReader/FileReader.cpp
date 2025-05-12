#include "FileReader.hpp"

FileReader::FileReader(const std::string &fileName) : file(fileName.c_str())
{
    if (!file || !file.is_open())
        throw FileReadingException("Coudn't open config file!");
    // std::cout<<"FileReader was created!\n";
}

FileReader::~FileReader()
{
    file.close();
    // std::cout << "FileReader dtor is called\n";
    
    // std::cout<<"FileReader was destroyed!\n";
}

std::string FileReader::getFileStr() const
{
    return fileStr;
}


FileReader::FileReadingException::FileReadingException(std::string err_msg)
{
    err_message = err_msg;
}

FileReader::FileReadingException::~FileReadingException() throw() {}


const char* FileReader::FileReadingException::what() const throw()
{
    return err_message.c_str();
}

void FileReader::fileToStr()
{
    std::string line;

    while (std::getline(file, line))
    {
        if (line.find_first_of('#') != std::string::npos)
            line.erase(line.find_first_of('#'));

        while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
            line.erase(0, 1);

        while (!line.empty() && (line[line.length() - 1] == ' ' || line[line.length() - 1] == '\t' || line[line.length() - 1] == '\n')) 
            line.erase(line.length() - 1, 1);
        
        if (line.empty())
            continue;

        if (!(line[line.length() - 1] == ';' || line[line.length() - 1] == '{' || line[line.length() - 1] == '}'))
            throw FileReadingException("there is no valid symbol at the end!");

        fileStr += line;
    }
    Logger::printStatus("INFO", "Config file was successfully read!");
}
