#include"LocationDirective.hpp"
#include "ServerDirective.hpp"
#include <iostream>

LocationDirective::LocationDirective(ServerDirective *serv) :
    path(""),
    redirect(),
    autoindex("off"),
    upload_dir(""),
    cgi_extension()
{
    this->index = serv->getIndex();
    validDirs[0] = "path";
    validDirs[1] = "allow_methods";
    validDirs[2] = "autoindex";
    validDirs[3] = "redirect";
    validDirs[4] = "upload_dir";
    validDirs[5] = "cgi_extension";
    validDirs[6] = "index";
    validDirs[7] = "client_max_body_size";
    validDirs[8] = "root";
    validDirs[9] = "error_pages";
}

std::string LocationDirective::getPath() const { return path; }
std::vector<std::string> LocationDirective::getAllow_methods() const { return allow_methods; }
std::map<int, std::string> LocationDirective::getRedirect() { return redirect; }
std::string LocationDirective::getAutoindex() const { return autoindex; }
std::string LocationDirective::getUpload_dir() const { return upload_dir; }
std::string LocationDirective::getCgi_path(const std::string& key) { return cgi_extension[key]; }

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
    // if (autoindex != "on" && autoindex != "off")
    //     throw std::runtime_error("Location: autoindex must be 'on' or 'off'");
    if (!upload_dir.empty() && upload_dir[0] != '/')
        throw std::runtime_error("Upload directory must be an absolute path (start with '/')");
}

void    LocationDirective::setIndex(const std::vector<std::string> indexVec)
{
    index.clear();
    std::vector<std::string>::const_iterator it = indexVec.begin();
    for(; it != indexVec.end(); ++it)
    {
        if (is_valid_index_value(*it))
            index.push_back(*it);
    }
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
        if (*it != "GET" && *it != "POST" && *it != "DELETE")
            throw std::runtime_error("invalid method in config file: " + *it);
        allow_methods.push_back(*it);///ste xienq aranc check anelu push anum?, arden anum enq)
    }
}

void    LocationDirective::setAutoindex(const std::string& off_or_on)
{
    if (off_or_on != "on" && off_or_on != "off")
        throw std::runtime_error("Autoindex must be 'on' or 'off'");
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
    // struct stat info;

    if (upload_dir.size() > 1 && upload_dir[upload_dir.size() - 1] == '/')
        throw std::runtime_error("Upload dir must not have / at the end." + upload_dir);
    // if (stat(upload_dir.c_str(), &info) != 0)
    //     throw std::runtime_error("Directory does not exist." + upload_dir);
    // if (!(info.st_mode & S_IFDIR))
    //     throw std::runtime_error("Path is not a directory.");
    // if (access(upload_dir.c_str(), W_OK) != 0)
    //     throw std::runtime_error("Directory is not writable.");//bacel comentyyyyy
    this->upload_dir = upload_dir;
}

static void _validateCgi_extension_val(const std::string& cgi_path)
{
    std::clog << "JUPA JUPA\n";
    if (cgi_path.length() < 2 || cgi_path[0] != '/')
        throw std::runtime_error("CGI path must start with /" + cgi_path);
    if (access(cgi_path.c_str(), X_OK) != 0)
        throw std::runtime_error("File is not executable.");
}

static void _validateCgi_extension_key(const std::string& extension)
{
    if (extension.size() < 2 || extension[0] != '.')
        throw std::runtime_error("CGI extension must start with '.'" + extension);
    if (extension.find_first_of(" \t\n*?$&|;<>(){}[]'\"\\") != std::string::npos || isdigit(extension[1]))
        throw std::runtime_error("CGI extension has invalid char in it." + extension);
}

void    LocationDirective::setCgi_extension(const std::multimap<std::string, std::vector<std::string> >& extension)
{
    for (std::multimap<std::string, std::vector<std::string> >::const_iterator itExt = extension.begin(); itExt != extension.end(); ++itExt) {
        if (itExt->first.size() < 2 || itExt->second.size() != 1)
            throw std::runtime_error("CGI extension must look like this with '.py /usr/bin/python3'");
        std::string key = itExt->first;
        std::string val = itExt->second[0];
        std::clog << "val = " << val << "\nkey = " << key;
        _validateCgi_extension_val(val);
        _validateCgi_extension_key(key);
        cgi_extension[key] = val;
    }
}

void    LocationDirective::setCgi_extension(const std::vector<std::string>& extension)
{
    if (extension.size() != 2)
        throw std::runtime_error("CGI extension must look like this with '.py /usr/bin/python3'");
    std::string key = extension[0];
    std::string val = extension[1];
    std::clog << "val = " << val << "\nkey = " << key;
    _validateCgi_extension_val(val);
    _validateCgi_extension_key(key);
    cgi_extension[key] = val;
}