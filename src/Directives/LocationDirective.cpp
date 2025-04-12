#include"LocationDirective.hpp"
#include <iostream>
LocationDirective::LocationDirective() :
    path(""),
    redirect(),
    autoindex("off"),
    upload_dir(""),
    cgi_extension(""),
    cgi_path("") {

        allow_methods.push_back("GET");//arajarkum em esi toxenq,vortev ete allow metod configi mej chlni lselem vor default pti GET lini
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
        throw std::runtime_error("path conatin unnecessary symbol");
    this->path = path;
}

void    LocationDirective::setAllow_methods(std::vector<std::string> methods)
{
    std::vector<std::string>::iterator it = methods.begin();
    for(; it != methods.end(); ++it)
    {
        allow_methods.push_back(*it);///ste xienq aranc check anelu push anum?
    }
}

void    LocationDirective::setAutoindex(const std::string& off_or_on)
{
    if (off_or_on != "on" && off_or_on != "off")
        throw std::runtime_error("Autoindex must be 'on' or 'off'" + off_or_on);
    autoindex = off_or_on;
}

int LocationDirective::validateRedirect(std::string codeStr, std::string url)
{
    if (codeStr.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Invalid HTTP code " + codeStr);
    std::stringstream ss(codeStr);
    int code;
    if (ss >> code)
        throw std::runtime_error("Invalid HTTP code " + codeStr);
    if (code < 100 || code >= 600)
        throw std::runtime_error("Status code must be 1xx-5xx.");
    if (code >= 300 && code < 400) {
        if (url.empty())
            throw std::runtime_error("Redirect requires a URL (e.g., 'return 301 /new').");
        if (url[0] != '/' && !(url.find("http://") == 0 || url.find("https://") == 0))
            throw std::runtime_error("URL must start with '/' or 'http'.");
    }
    if ((code < 300 || code >= 400) && (url.empty() && url.size() > 1024))
            throw std::runtime_error("Custom response text too long (max 1KB).");    
    return true;   
}
        
void    LocationDirective::setRedirect(std::vector<std::string> red)
{
    if (red.size() != 2)
        throw std::runtime_error("Invalid return format. Use: 'CODE URL'");        
    this->redirect.insert(std::pair<int, std::string>(this->validateRedirect(red[0], red[1]), red[1]));
}

void    LocationDirective::setUpload_dir(const std::string& upload_dir)
{
    struct stat info;

    if (upload_dir.size() > 1 && upload_dir[upload_dir.size() - 1] == '/')
        throw std::runtime_error("Upload dir must not have / at the end." + upload_dir);
    if (stat(upload_dir.c_str(), &info) != 0)
        throw std::runtime_error("Directory does not exist." + upload_dir);
    if (!(info.st_mode & S_IFDIR))
        throw std::runtime_error("Path is not a directory.");
    if (access(upload_dir.c_str(), W_OK) != 0)
        throw std::runtime_error("Directory is not writable.");
    this->upload_dir = upload_dir;
}
 

void    LocationDirective::setCgi_path(const std::string& cgi_path)
{
    struct stat info;
    
    if (cgi_path.size() < 2 || cgi_path[0] != '/')
        throw std::runtime_error("CGI path must start with '//'" + cgi_path);
    if (stat(upload_dir.c_str(), &info) != 0)
        throw std::runtime_error("File does not exist.");
    if (!(info.st_mode & S_IFREG))
        throw std::runtime_error("Path is not a file.");///stex partadir pti fayl lini,upload_dir-umel direktoria pti lini(partadir)???????
    if (access(cgi_path.c_str(), X_OK) != 0)
        throw std::runtime_error("File is not executable.");
    this->cgi_path = cgi_path;
}

void    LocationDirective::setCgi_extension(const std::string& extension)
{
    if (extension.size() < 2 || extension[0] != '.')
        throw std::runtime_error("CGI extension must start with '.'" + extension);
    if (extension.find_first_of(" \t\n*?$&|;<>(){}[]'\"\\") != std::string::npos || isdigit(extension[1]))
        throw std::runtime_error("CGI extension has invalid char in it." + extension);
    cgi_extension = extension;
}