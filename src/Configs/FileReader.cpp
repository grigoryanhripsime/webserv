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
        // std::cout << "htes->" << line << std::endl;
        if (line.find_first_of('#') != std::string::npos)
            line.erase(line.find_first_of('#'));//esi en pahna vor commentner karananq anenq?, hastat config fayli mej chi lenlu nenc tox(valid hraman)vor # chi parenakelu?

        while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
            line.erase(0, 1);//skzbic ktrel white spacery

        while (!line.empty() && (line[line.length() - 1] == ' ' || line[line.length() - 1] == '\t' || line[line.length() - 1] == '\n')) 
            line.erase(line.length() - 1, 1);//verjic ktrel white spacery
        
        if (line.empty())
            continue;//inch case kara lini????

        if (!(line[line.length() - 1] == ';' || line[line.length() - 1] == '{' || line[line.length() - 1] == '}'))
            throw FileReadingException("there is no valid symbol at the end!");

        fileStr += line;//fileStr i mej celi fayly kpcnum enq mi stroki mej
    }
    // std::cout<<fileStr<<std::endl;
    Logger::printStatus("INFO", "Config file was successfully read!");
}
