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
        // std::cout << "key = " << key << std::endl;
        std::vector<std::string> path(1);
        //33
        if (key == "location" && (!(ss>>path[0]) || path[0].find("{") != std::string::npos))
            throw TokenConfig::TokenConfigException("Missing path in location! OR path include '{'");//llocation /abc{
        // std::cout << "path->" << path[0] << std::endl;
        
        while (std::isspace(ss.peek()))
            ss.get();

        if (key.find(";") != std::string::npos)
            throw TokenConfig::TokenConfigException("Ete value chi linum, gnum myus keyn el a vercnum!");
        if (ss.peek() == '{')
        {
            ss.get();  // Consume '{'
            Directive *dir = new Directive();
            directives.blocks.insert(std::make_pair(key, dir));
            // std::cout << "blokayin e->" << key << std::endl;
            
            if (key == "location")
                dir->simpleDir.insert(std::make_pair("path", path));

            std::string block;
            // std::cout << "shto eto->" << ss.str() << std::endl;
            if (ss.str().find('}') == std::string::npos || !std::getline(ss, block, '}')) // || block.length() < 2)
                throw TokenConfig::TokenConfigException("Error: Invalid block structure");
            // std::cout << "block = " << block << std::endl;
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
        else // Key-value pair case
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
    // printDirective(directives, 0);
    Logger::printStatus("INFO", "Tokenization of directives was successfully done!");
}

void TokenConfig::printDirective(const Directive &directive, int indent) 
{
    (void)directive;
    (void)indent;
    std::string indentation(indent, ' ');
    
    // Print simpleDir
    // for (std::multimap<std::string, std::vector<std::string> >::const_iterator it = directive.simpleDir.begin(); it != directive.simpleDir.end(); ++it) {
    //     std::cout << indentation << it->first;
    //     for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
    //         std::cout << " " << *vit;
    //     }
    //     std::cout << ";\n";
    // }
    std::cout << "potom\n";
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