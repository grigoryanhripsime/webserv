#include"LocationDirective.hpp"
#include <iostream>
LocationDirective::LocationDirective() :
    path(""),
    redirect(),
    allow_methods(1, "GET"),
    autoindex("off"),
    upload_dir(""),
    cgi_extension(""),
    cgi_path("") {

        validDirs[0] = "path";
        validDirs[1] = "allow_methods";
        validDirs[2] = "autoindex";
        validDirs[3] = "redirect";
        validDirs[4] = "upload_dir";
        validDirs[5] = "cgi_extension";
        validDirs[6] = "cgi_path";
        validDirs[7] = "index";
        validDirs[8] = "client_max_body_size";
        validDirs[9] = "root";
        validDirs[10] = "error_pages";
}

LocationDirective::~LocationDirective() {std::cout << "LocationDirective dtor\n";}

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
bool LocationDirective::isValidLocationPath(const std::string& path) {
    if (path.empty() || path[0] != '/')
        return false;//partadir petqa path lini u sksvi '/'-ov

    for (std::string::size_type i = 1; i < path.size(); ++i)
    {
        char c = path[i];
        if (c == '/' && path[i - 1] == '/')
            return false;//  "/not//valid"
        if (!std::isalnum(c) && c != '/' && c != '-' && c != '_' &&
            c != '.' && c != '?' && c != '=' && c != '&')
            return false;
    }

    return true;
}

void    LocationDirective::setPath(const std::string& path)
{
    std::cout<<path<<std::endl;
    if (!isValidLocationPath(path))
        throw std::runtime_error("path conatin unnecessary simnbol");
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
    std::cout<<off_or_on<<std::endl;
    autoindex = off_or_on;
}

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
