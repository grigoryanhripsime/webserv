#pragma once

#include <string>
#include <map>
#include <vector>
class AGeneralDirective
{
protected:
    std::string index;//Should have a default (e.g., index.html)., ham kara serverum lini ham locationum   size_t
    size_t client_max_body_size;//Optional but recommended.ham kara serverum lini ham locationum     bool,    default: 1MB (или другое разумное значение)
    
    std::string root;//can be overridden in location, required dlya servera no mojet bit pustoy na locatione
    std::map<int, std::string> error_pages;//type-@ yst deepseeki ,  default: стандартные страницы ошибок сервера
    // std::string error_page;
public:
    AGeneralDirective();
    virtual ~AGeneralDirective() = 0;//xi senc vor????
    virtual void validate() const = 0;  // для валидации значений
    const std::string& getRoot() const { return root; }
    void setRoot(const std::string& r) { root = r; }

};