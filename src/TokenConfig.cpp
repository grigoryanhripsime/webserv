#include "TokenConfig.hpp"

TokenConfig::TokenConfig(std::string strFile)
{
    this->directives = Directive();
    this->strFile = strFile;
}

void TokenConfig::fillingDirectivesRec(std::stringstream &ss, Directive &directives)
{
    std::string key;
    while (ss >> key)//esi key-i mej lcnelua minchev space-@?
    {
        while (std::isspace(ss.peek()))//Он смотрит на следующий символ в потоке , но не двигает курсор.
            ss.get();//get() удаляет символ из потока и передвигает курсор вперёд.

        if (key.find(";") != std::string::npos)
            throw TokenConfig::TokenConfigException("Error: ete value chi linum, gnum myus keyn el a vercnum!");
        if (ss.peek() == '{')
        {
            ss.get();  // Consume '{'
            Directive *dir = new Directive();
            directives.blocks.insert(std::make_pair(key, dir));

            std::string block;
            if (ss.str().find('}') == std::string::npos || !std::getline(ss, block, '}')) // || block.length() < 2)
                throw TokenConfig::TokenConfigException("Error: Invalid block structure");
            int cnt = std::count(block.begin(), block.end(), '{');
            while (cnt)
            {
                block += '}';
                std::string tmp;
                if (!std::getline(ss, tmp, '}'))
                    throw TokenConfig::TokenConfigException("iran hamapatasxan pakox blocky chkar!!");
                cnt += std::count(tmp.begin(), tmp.end(), '{');
                block += tmp;
                cnt--;
            }
            std::stringstream ss2(block);
            fillingDirectivesRec(ss2, *dir);
        }
        else // Key-value pair case
        {
            std::string values, value;
            if (!std::getline(ss, values, ';') || values.empty())//inch case kara lini?
            throw TokenConfig::TokenConfigException("Error: Invalid directive syntax");
            std::stringstream ss2(values);
            std::vector<std::string> vals;
            while (ss2 >> value)
            vals.push_back(value);
            directives.values.insert(std::make_pair(key, vals));
        }
    }  
}


void TokenConfig::fillingDirectives()
{
    std::stringstream ss(strFile);
    fillingDirectivesRec(ss, directives);
    // printDirective(directives, 0);
    Logger::printStatus("INFO", "Tokenization of directives was successfully done!");
}

void TokenConfig::printDirective(const Directive &directive, int indent) 
{
    (void)directive;
    (void)indent;
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

const Directive& TokenConfig::getDirectives() const
{
    return directives;
}

TokenConfig::TokenConfigException::TokenConfigException(std::string err_msg)
{
    err_message = err_msg;
}

TokenConfig::TokenConfigException::~TokenConfigException() throw() {}


const char* TokenConfig::TokenConfigException::what() const throw()
{
    return err_message.c_str();
}