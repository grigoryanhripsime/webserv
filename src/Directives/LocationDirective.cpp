#include"LocationDirective.hpp"
#include <iostream>
LocationDirective::LocationDirective() :
    path(""),
    redirect(),
    allow_methods(1, "GET"), // По умолчанию только GET
    autoindex("off"),
    upload_dir(""),
    cgi_extension(""),
    cgi_path("") {
}

LocationDirective::~LocationDirective() {}

void LocationDirective::validate() const
{
    // if (path.empty())
    //     throw std::runtime_error("Location: path cannot be empty");esi fillingDirectivesRec funkciayi mejenq nayum
    if (path[0] != '/')
        throw std::runtime_error("Location: path must start with '/'");
        
    //checka anum arka metodnery,ev ete  baci get,post,delete-ic urish metod ka exception enq qcum
    std::set<std::string> unique_methods;
    for (std::vector<std::string>::const_iterator it = allow_methods.begin(); it != allow_methods.end(); ++it)
    {
        const std::string& method = *it;
        if (method != "GET" && method != "POST" && method != "DELETE")
            throw std::runtime_error("Location: invalid HTTP method '" + method + "'");
        if (!unique_methods.insert(method).second)
            throw std::runtime_error("Duplicate HTTP method: " + method);
   }

   // Проверка autoindex
    if (autoindex != "on" && autoindex != "off")
        throw std::runtime_error("Location: autoindex must be 'on' or 'off'");

     // Проверка CGI конфигурации
    //    Сервер видит расширение .py
    //    Запускает файл через /usr/bin/python3 hello.py
    if (!cgi_extension.empty() && cgi_path.empty())//ete extension ka bayc cgi_path chka errora
        throw std::runtime_error("Location: CGI path required when extension is set");
    if (cgi_extension.empty() && !cgi_path.empty())//errora nayev hakarak depqy
        throw std::runtime_error("Location: CGI extension required when path is set");
    if (!cgi_extension.empty() && cgi_extension[0] != '.')
        throw std::runtime_error("CGI extension must start with '.' (e.g., '.py')");
    if (!upload_dir.empty() && upload_dir[0] != '/')
        throw std::runtime_error("Upload directory must be an absolute path (start with '/')");
}

// bool shouldHandleAsCgi(const std::string& filename) const {
//     // Проверяем, заканчивается ли файл на cgi_extension
//     if (!cgi_extension.empty() && 
//         filename.size() >= cgi_extension.size() &&
//         filename.compare(filename.size() - cgi_extension.size(), 
//                         cgi_extension.size(), 
//                         cgi_extension) == 0) {
//         return true;
//     }
//     return false;
// }



////////////////setters/////////////

void    LocationDirective::setPath(const std::string& path)
{
    this->path = path;
}

void    LocationDirective::setAllow_methods(std::vector<std::string> methods)
{
    std::vector<std::string>::iterator it = methods.begin();
    for(; it != methods.end(); ++it)
    {
        allow_methods.push_back(*it);
    }
}

void    LocationDirective::setAutoindex(const std::string& off_or_on)
{
    autoindex = off_or_on;
}


////////////////////////////////////////

void    LocationDirective::setRedirect(std::vector<std::string> red)
{
    if (red.size() != 2) {
        throw std::runtime_error("Invalid return/redirect format. Use: 'CODE URL'");
    }//redirect 302 http://temp.com;:CORRECT
    //redirect 302 http://temp.com http://idk.com; # ОШИБКА!
    //redirect 302 305 http://temp.com;  # ОШИБКА!,    mekel PARTADIRA vor URL-n`http://temp.com parunaki protocoly http
    if (isAllDigits(red[0]))
    {
        std::stringstream ss(red[0]);
        int ind;
        ss >> ind;
        redirect[ind] = red[1];
        ss.clear();
    }
    else
        std::cout << "qci indz exception:)\n";
        ///exception qci vor red[0]-n parrtadir pti tiv ylni
   
}
/////////////////////////////////////////////////

void    LocationDirective::setUpload_dir(const std::string& upload_dir)
{
    this->upload_dir = upload_dir;
}
 
void    LocationDirective::setCgi_extension(const std::string& extension)
{
    cgi_extension = extension;
}

void    LocationDirective::setCgi_path(const std::string& cgi_path)
{
    this->cgi_path = cgi_path;
}

// void    LocationDirective::setIndex(const std::string& value)
// {
//     index = value;
// }

// void    LocationDirective::setClient_max_body_size(const std::string& size)
// {
//     std::stringstream ss(size);
//     size_t sIze;
//     ss >> sIze;
//     client_max_body_size = sIze;
// }

// void    LocationDirective::setRoot(const std::string& rootPath)
// {
//     root = rootPath;
// }

// /////////////////////////////////////////////////////////////////
// bool LocationDirective::isAllDigits(const std::string& str) {
//     if (str.empty()) return false;
    
//     for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
//         if (!isdigit(static_cast<unsigned char>(*it)))
//         return false;
// }
// return true;
// }

// void    LocationDirective::setError_pages(std::vector<std::string> pages)
// {
//     //hmi ste pti jokenq qanisn en tiv qansin enq string,orinak`error_pages 500 502 503 504 /50x.html;
//     // map[500] = /50x.html;
//     // map[502] = /50x.html;
//     // map[503] = /50x.html;
//     //....senc pti lini
//     std::vector<std::string>::iterator it = pages.begin();
//     for(; it != pages.end(); ++it)
//     {
//         // redirect.push_back(*it);
//         std::stringstream ss(*it);
//         int ind;
//         if (isAllDigits(*it))
//         {
//             ss >> ind;
//             redirect[ind] = pages[pages.size() - 1];
//         }
//     }
// }
// /////////////////////////////////////////////////////////////////
