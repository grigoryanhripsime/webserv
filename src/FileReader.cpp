#include "FileReader.hpp"

FileReader::FileReader(const std::string &fileName) : file(fileName.c_str())
{
    if (!file || !file.is_open())
        throw FileReadingException("Coudn't open config file!");
    std::cout<<"FileReader was created!\n";
}

FileReader::~FileReader()
{
    std::cout<<"FileReader was destroyed!\n";
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

// std::vector<Directive *> &FileReader::readConfig()
// {
//     std::string line;
//     std::vector<Directive *> nodeStack;

//     while (std::getline(file, line))
//     {
//         while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
//             line.erase(0, 1);

//         while (!line.empty() && (line[line.length() - 1] == ' ' || line[line.length() - 1] == '\t' || line[line.length() - 1] == ';')) 
//             line.erase(line.length() - 1, 1);
        
//         if (line.empty() || line[0] == '#')
//             continue;

//         std::cout<<line<<std::endl;

//     }
// }

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
        
        if (line.empty() || line[0] == '#')
            continue;


        // std::cout<<line<<std::endl;
        if (!(line[line.length() - 1] == ';' || line[line.length() - 1] == '{' || line[line.length() - 1] == '}'))
            throw FileReadingException("there is no valid symbol at the end!");

        fileStr += line;
    }
    std::cout<<fileStr<<std::endl;
}

