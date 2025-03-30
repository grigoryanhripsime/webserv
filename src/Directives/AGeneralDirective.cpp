#include "AGeneralDirective.hpp"

AGeneralDirective::AGeneralDirective() :
    index("index.html"),
    client_max_body_size(1048576),//en mec tivy=>1MB ina hamarjeq
    root(""),
    error_pages()
{
    error_pages[400] = "error_pages/400.html";
    error_pages[404] = "error_pages/404.html";
    error_pages[500] = "error_pages/500.html";
}

AGeneralDirective::~AGeneralDirective(){}