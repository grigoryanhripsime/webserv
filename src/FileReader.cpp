#include "FileReader.hpp"

FileReader::FileReader(const std::string &fileName) : file(fileName.c_str())
{
    if (!file || !file.is_open())
        throw FileReadingException("Coudn't open config file!");
    std::cout<<"FileReader was created!\n";
}

FileReader::~FileReader()
{
    file.close();
    
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

        if (!(line[line.length() - 1] == ';' || line[line.length() - 1] == '{' || line[line.length() - 1] == '}'))
            throw FileReadingException("there is no valid symbol at the end!");

        fileStr += line;
    }
    std::cout<<fileStr<<std::endl;
}

void rec(std::stringstream &ss, Directive &directives)
{
    std::string key;
    while (ss >> key)
    {
        while (std::isspace(ss.peek()))
            ss.get();

        if (ss.peek() == '{')
        {
            ss.get();  // Consume '{'
            Directive *dir = new Directive();
            directives.blocks.insert(std::make_pair(key, dir));

            std::string block;
            if (!std::getline(ss, block, '}') || block.length() < 2)
                throw std::runtime_error("Error: Invalid block structure");
            std::stringstream ss2(block);
            rec(ss2, *dir);
        }
        else  // Key-value pair case
        {
            std::string values, value;
            if (!std::getline(ss, values, ';'))
                throw std::runtime_error("Error: Invalid directive syntax");
            std::stringstream ss2(values);
            std::vector<std::string> vals;
            // std::cout<<"key = "<<key<<std::endl;
            while (ss2 >> value)
            {
                // std::cout<<"value = "<<value<<std::endl;
                vals.push_back(value);
            }
            directives.values.insert(std::make_pair(key, vals));
        }
    }  
}


void FileReader::printDirective(const Directive &directive, int indent = 0) 
{
    std::string indentation(indent, ' ');
    
    // Print values
    for (std::multimap<std::string, std::vector<std::string> >::const_iterator it = directive.values.begin(); it != directive.values.end(); ++it) {
        std::cout << indentation << it->first;
        for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
            std::cout << " " << *vit;
        }
        std::cout << ";\n";
    }
    
    // Print blocks
    for (std::multimap<std::string, Directive *>::const_iterator it = directive.blocks.begin(); it != directive.blocks.end(); ++it) {
        std::cout << indentation << it->first << " {\n";
        printDirective(*(it->second), indent + 4);
        std::cout << indentation << "}\n";
    }
}



void FileReader::parseConfig()
{
    std::stringstream ss(fileStr);
    rec(ss, directives);
    printDirective(directives, 0);
}




