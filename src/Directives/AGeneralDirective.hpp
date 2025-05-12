#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>

class AGeneralDirective
{
protected:
    std::vector<std::string> index;
    size_t client_max_body_size;
 
    std::string root;
public:
    //////////setters////////////
    bool isAllDigits(const std::string& str);
    bool is_all_letters(const std::string& str);
    virtual void    setIndex(const std::vector<std::string> indexVec);
    virtual void    setClient_max_body_size(const std::string& size);
    void    setRoot(const std::string& rootPath);
    void    setError_pages(std::vector<std::string> pages);
public:
    AGeneralDirective();
    virtual ~AGeneralDirective() = 0;
    virtual void validate() const = 0;
    const std::string& getRoot() const { return root; }

        /////getters////
    std::map<int, std::string>& getError_pages();
    std::vector<std::string> getIndex();
    size_t  getBodySize();
    ////////////index validacia
    bool is_valid_index_value(std::string index);
    /////////body size vsalidacia/
    bool is_valid_client_max_body_size(const std::string& value);
    size_t parse_size_to_bytes(const std::string& value);
    //////////validacia root////
    bool is_valid_root(const std::string& rootPath);

};