#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>

class AGeneralDirective
{
protected:
    std::string index;//Should have a default (e.g., index.html)., ham kara serverum lini ham locationum   size_t
    size_t client_max_body_size;//Optional but recommended.ham kara serverum lini ham locationum     bool,    default: 1MB (или другое разумное значение)
 
    std::string root;//can be overridden in location, required dlya servera no mojet bit pustoy na locatione
    std::map<int, std::string> error_pages;//type-@ yst deepseeki ,  default: стандартные страницы ошибок сервера
public:
    //////////setters////////////
    bool isAllDigits(const std::string& str);
    void    setIndex(const std::string& value);
    void    setClient_max_body_size(const std::string& size);
    void    setRoot(const std::string& value);
    void    setError_pages(std::vector<std::string> pages);
public:
    AGeneralDirective();
    virtual ~AGeneralDirective() = 0;//xi senc vor????
    virtual void validate() const = 0;  // для валидации значений
    const std::string& getRoot() const { return root; }
    /////getters////
    std::map<int, std::string> getError_page() {return error_pages;}
};