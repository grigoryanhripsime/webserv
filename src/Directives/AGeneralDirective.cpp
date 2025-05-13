#include "AGeneralDirective.hpp"
#include<iostream>

AGeneralDirective::AGeneralDirective() : client_max_body_size(1048576), root("") {}

std::vector<std::string> AGeneralDirective::getIndex() { return index; } 
size_t  AGeneralDirective::getBodySize() { return client_max_body_size; }

AGeneralDirective::~AGeneralDirective()
{
    // std::cout << "AGeneralDirective dtor is called\n";
}

/////////////index/////////////////
bool AGeneralDirective::is_valid_index_value(std::string value)
{
    size_t ind = value.find('.');
    if (value[0] == '.' || (ind != std::string::npos && !is_all_letters(value.substr(ind + 1, value.size()))))
        throw std::runtime_error("Innvalid value(poxir message-@)" + value);
    if (isAllDigits(value) || value.empty() || value[0] == '/' || value[value.size() - 1] == '/'
        || value.find("//") != std::string::npos)
        return false;
    for (unsigned int i = 0; i < value.size(); ++i)
    {
        if (!(isalpha(value[i]) || isdigit(value[i]) || 
             value[i] == '.' || value[i] == '-' || 
             value[i] == '_' || value[i] == '/'))
            return false;
    }
    return true;
}

void    AGeneralDirective::setIndex(const std::vector<std::string> indexVec)
{
    index.clear();
    std::vector<std::string>::const_iterator it = indexVec.begin();
    for(; it != indexVec.end(); ++it)
    {
        if (is_valid_index_value(*it))
            index.push_back(*it);
    }
}
////////body_size/////////////////
bool  AGeneralDirective::is_valid_client_max_body_size(const std::string& value) {
    if (value.empty()) return false;
    size_t num_end = 0;
    while (num_end < value.size() && isdigit(value[num_end]))
        num_end++;
    if (num_end == value.size()) return true;
    if (num_end < value.size())
    {
        char suffix = tolower(value[num_end]);
        if (suffix != 'k' && suffix != 'm' && suffix != 'g')
            return false;
        return (num_end + 1 == value.size());
    }
    return false;
}

size_t AGeneralDirective::parse_size_to_bytes(const std::string& value)
{
    char* end;
    unsigned long num = strtoul(value.c_str(), &end, 10);
    if (end == value.c_str()) return 0;
    switch (tolower(*end)) {
        case 'k': return num * 1024;
        case 'm': return num * 1024 * 1024;
        case 'g': return num * 1024 * 1024 * 1024;
        default:  return num;
    }
}

void    AGeneralDirective::setClient_max_body_size(const std::string& size)
{
    if (!is_valid_client_max_body_size(size))
        throw std::runtime_error("invalid body size: " + size);
    client_max_body_size = parse_size_to_bytes(size);
    if (client_max_body_size > 2 * 1048576)
        throw std::runtime_error("invalid body size: " + size);
}
///////validacia Root///////
bool AGeneralDirective::is_valid_root(const std::string& rootPath)
{
    if (rootPath[0] != '/' || (rootPath.size() > 1 && rootPath[rootPath.size() - 1] == '/'))
        return false;
    for (unsigned int i = 0; i < rootPath.size(); ++i) {
        if (!isalnum(rootPath[i]) && rootPath[i] != '-' && rootPath[i] != '_' && rootPath[i] != '.' && rootPath[i] != '/')
            return false;
    }
    if (rootPath.find("/../") != std::string::npos || 
        rootPath.find("/./") != std::string::npos || 
        rootPath.find("//") != std::string::npos)
        return false;
    return true;
}

void    AGeneralDirective::setRoot(const std::string& rootPath)
{
    if (!is_valid_root(rootPath))
        throw std::runtime_error("invalid root" + rootPath);
    root = rootPath;
}

bool AGeneralDirective::isAllDigits(const std::string& str) {
    if (str.empty()) return false;
    
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(static_cast<unsigned char>(*it)))
        return false;
}
return true;
}

bool AGeneralDirective::is_all_letters(const std::string& str) {
    if (str.empty()) return false;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isalpha(static_cast<unsigned char>(*it))) {
            return false;
        }
    }
    return true;
}
