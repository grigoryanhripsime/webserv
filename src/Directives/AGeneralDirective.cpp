#include "AGeneralDirective.hpp"

//1048576 == 1MB
AGeneralDirective::AGeneralDirective() : index("index.html"), client_max_body_size(1048576), root(""), error_pages()
{
    // error_pages[400] = "error_pages/400.html";
    // error_pages[404] = "error_pages/404.html";
    // error_pages[500] = "error_pages/500.html";//yani defaultov menq karanq unenanq vor errorin inch eja hamapatasxan
}

AGeneralDirective::~AGeneralDirective(){}


void    AGeneralDirective::setIndex(const std::string& value)
{
    index = value;
}

void    AGeneralDirective::setClient_max_body_size(const std::string& size)
{
    std::stringstream ss(size);
    size_t sIze;
    ss >> sIze;
    client_max_body_size = sIze;
}

void    AGeneralDirective::setRoot(const std::string& rootPath)
{
    root = rootPath;
}

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
    for(; it != pages.end(); ++it)
    {
        // redirect.push_back(*it);
        std::stringstream ss(*it);
        int ind;
        if (isAllDigits(*it))
        {
            ss >> ind;
            error_pages[ind] = pages[pages.size() - 1];
        }
    }
}
/////////////////////////////////////////////////////////////////
