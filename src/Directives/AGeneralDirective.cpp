#include "AGeneralDirective.hpp"
#include<iostream>
//1048576 == 1MB
AGeneralDirective::AGeneralDirective() : client_max_body_size(1048576), root(""), error_pages()
{
    index.push_back("index.html");
    // error_pages[400] = "error_pages/400.html";
    // error_pages[404] = "error_pages/404.html";
    // error_pages[500] = "error_pages/500.html";//yani defaultov menq karanq unenanq vor errorin inch eja hamapatasxan
}

AGeneralDirective::~AGeneralDirective(){}
/////////////index/////////////////
bool AGeneralDirective::is_valid_index_value(std::string value)
{
    // std::cout << "elav = "
    if (value.empty() || value[0] == '/' || value[value.size() - 1] == '/'
        || value.find("//") != std::string::npos)
        return false;
    for (unsigned int i = 0; i < value.size(); ++i)
    {
        if (!(isalpha(value[i]) || isdigit(value[i]) || 
             value[i] == '.' || value[i] == '-' || 
             value[i] == '_' || value[i] == '/')) {
                std::cout << "vaaaaaaaaaaaaaaaaaaaaa\n";
            return false;
        }
    }
    return true;
}

void    AGeneralDirective::setIndex(const std::vector<std::string>& indexVec)
{
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
    // Проверка числа + суффикса
    size_t num_end = 0;
    while (num_end < value.size() && isdigit(value[num_end]))
        num_end++;
    // Если строка состоит только из цифр (например, "1000000")
    if (num_end == value.size()) return true;
    // Если есть суффикс (например, "10M")
    if (num_end < value.size())
    {
        char suffix = tolower(value[num_end]);
        if (suffix != 'k' && suffix != 'm' && suffix != 'g')
            return false;  // Недопустимый суффикс
        // После суффикса не должно быть других символов
        return (num_end + 1 == value.size());
    }
    return false;
}

size_t AGeneralDirective::parse_size_to_bytes(const std::string& value)
{
    char* end;
    unsigned long num = strtoul(value.c_str(), &end, 10);//string to unsigned long
    //10-@ yani 10-akan hamakarg(base-na)
    // end-указатель, который будет установлен на первый символ после распознанного числа.
    if (end == value.c_str()) return 0;  // Не число
    // Если strtoul не нашла ни одной цифры, она вернёт 0, а endptr останется указывать на начало строки.
    // Это значит, что строка не является числом (например, "M10" или "invalid").
    switch (tolower(*end)) {
        case 'k': return num * 1024;
        case 'm': return num * 1024 * 1024;
        case 'g': return num * 1024 * 1024 * 1024;
        default:  return num;  // Без суффикса (байты)
    }
}

void    AGeneralDirective::setClient_max_body_size(const std::string& size)
{
    if (!is_valid_client_max_body_size(size))
        throw std::runtime_error("invalid body size");
    client_max_body_size = parse_size_to_bytes(size);

}
//////////////////////
///////validacia Root///////
bool AGeneralDirective::is_valid_root(const std::string& rootPath)
{
    if (rootPath[0] != '/' || (rootPath.size() > 1 && rootPath[rootPath.size() - 1] == '/'))
        return false;
    for (unsigned int i = 0; i < rootPath.size(); ++i) {
        if (!isalnum(rootPath[i]) && rootPath[i] != '-' && rootPath[i] != '_' && rootPath[i] != '.' && rootPath[i] != '/')
            return false;
    }
    // 3. Запрет на "/../", "/./", "//"
    if (rootPath.find("/../") != std::string::npos || 
        rootPath.find("/./") != std::string::npos || 
        rootPath.find("//") != std::string::npos)
        return false;
    return true;
}

void    AGeneralDirective::setRoot(const std::string& rootPath)
{
    if (!is_valid_root(rootPath))
        throw std::runtime_error("invalid root");
    root = rootPath;
}


////////////////////
/////////////////////////////////////////////////////////////////
bool AGeneralDirective::isAllDigits(const std::string& str) {
    if (str.empty()) return false;
    
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(static_cast<unsigned char>(*it)))
        return false;
}
return true;
}

void    AGeneralDirective::setError_pages(std::vector<std::string> pages)
{
    //hmi ste pti jokenq qanisn en tiv qansin enq string,orinak`error_pages 500 502 503 504 /50x.html;
    // map[500] = /50x.html;
    // map[502] = /50x.html;
    // map[503] = /50x.html;
    //....senc pti lini
    std::vector<std::string>::iterator it = pages.begin();
    std::cout << "gres-?" << *(pages.end() - 1) << std::endl;
    if (!is_valid_index_value(*(pages.end() - 1)))
        throw std::runtime_error("Invalid last value(path) of error_page");
    for(; it != pages.end() - 1; ++it)
    {
        // redirect.push_back(*it);
        std::stringstream ss(*it);
        int ind;
        if (isAllDigits(*it))
        {
            ss >> ind;
            error_pages[ind] = pages[pages.size() - 1];
        }
        else
            throw std::runtime_error("error_page values must be only digits except last");
    }
}
/////////////////////////////////////////////////////////////////
