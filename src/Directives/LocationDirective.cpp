#include"LocationDirective.hpp"

LocationDirective::LocationDirective() :
    path(""),
    redirect(),
    allow_methods(1, "GET"), // По умолчанию только GET
    autoindex(false),
    upload_dir(""),
    cgi_extension(""),
    cgi_path("") {
}

LocationDirective::~LocationDirective() {
}

void LocationDirective::validate() const{
    // if (path.empty())
    //     throw std::runtime_error("Location: path cannot be empty");esi fillingDirectivesRec funkciayi mejenq nayum
    if (path[0] != '/')
        throw std::runtime_error("Location: path must start with '/'");
        
    //checka anum arka metodnery,ev ete  baci get,post,delete-ic urish metod ka exception enq qcum
    std::set<std::string> unique_methods;
    for (std::vector<std::string>::const_iterator it = allow_methods.begin(); it != allow_methods.end(); ++it)
    {
       const std::string& method = *it;
       if (method != "GET" && method != "POST" && method != "DELETE") {
           throw std::runtime_error("Location: invalid HTTP method '" + method + "'");
       }
       if (!unique_methods.insert(method).second)
        throw std::runtime_error("Duplicate HTTP method: " + method);
   }

   // Проверка autoindex
   if (autoindex != true && autoindex != false)
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