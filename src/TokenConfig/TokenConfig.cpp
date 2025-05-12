#include "TokenConfig.hpp"

TokenConfig::TokenConfig(std::string strFile)
{
    this->directives = Directive();
    this->strFile = strFile;
}

void TokenConfig::fillingDirectivesRec(std::stringstream &ss, Directive &directives)
{
    std::string key;
    while (ss >> key)
    {
        std::vector<std::string> path(1);
        if (key == "location" && (!(ss>>path[0]) || path[0].find("{") != std::string::npos))
            throw TokenConfig::TokenConfigException("Missing path in location! OR path include '{'");
        
        while (std::isspace(ss.peek()))
            ss.get();

        if (key.find(";") != std::string::npos)
            throw TokenConfig::TokenConfigException("Ete value chi linum, gnum myus keyn el a vercnum!");
        if (ss.peek() == '{')
        {
            ss.get();
            Directive *dir = new Directive();
            directives.blocks.insert(std::make_pair(key, dir));            
            if (key == "location")
                dir->simpleDir.insert(std::make_pair("path", path));

            std::string block;
            if (ss.str().find('}') == std::string::npos || !std::getline(ss, block, '}')) // || block.length() < 2)
                throw TokenConfig::TokenConfigException("Error: Invalid block structure");
            int cnt = std::count(block.begin(), block.end(), '{');
            while (cnt)
            {
                block += '}';
                std::string tmp;
                if (!std::getline(ss, tmp, '}'))
                    throw TokenConfig::TokenConfigException("Iran hamapatasxan pakox blocky chkar!!");
                cnt += std::count(tmp.begin(), tmp.end(), '{');
                block += tmp;
                cnt--;
            }
            std::stringstream ss2(block);
            fillingDirectivesRec(ss2, *dir);
        }
        else
        {
            std::string values, value;
            if (!std::getline(ss, values, ';') || values.empty())
                throw TokenConfig::TokenConfigException("Invalid directive syntax");
            std::stringstream ss2(values);
            std::vector<std::string> vals;
            while (ss2 >> value)
                vals.push_back(value);
            directives.simpleDir.insert(std::make_pair(key, vals));
        }
    }  
}


void TokenConfig::fillingDirectives()
{
    std::stringstream ss(strFile);
    fillingDirectivesRec(ss, directives);
    Logger::printStatus("INFO", "Tokenization of directives was successfully done!");
}

void TokenConfig::printDirective(const Directive &directive, int indent) 
{
    (void)directive;
    (void)indent;
    std::string indentation(indent, ' ');
    
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