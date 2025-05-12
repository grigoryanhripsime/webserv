#include "Request.hpp"

void    Request::fill_status_message()
{
    // status_message.insert(std::pair<int, std::string>(200, "OK"));
    status_message[200] = "OK";//GET
    status_message[201] = "CREATED";//POST
    status_message[204] = "No Content";//DELETE
    status_message[206] = "Partial Content";//Partial content delivered (e.g., for Range requests).
    
    status_message[301] = "Moved Permanently";//Resource permanently moved (e.g., redirect to a new URL).
    status_message[302] = "Found";//Resource temporarily moved (common for redirects)
    status_message[303] = "See Other";//Redirect to another URL (e.g., after POST).
    status_message[304] = "Not Modified";//Resource unchanged (e.g., for caching with If-Modified-Since).
    status_message[307] = "Temporary Redirect";//Temporary redirect, same method as original request
    status_message[308] = "Permanent Redirect";//Permanent redirect, same method as original request

    status_message[400] = "BAD REQUEST";//Invalid request syntax or parameters.
    status_message[401] = "UNAUTHORIZED";//Authentication required (e.g., missing or invalid credentials).
    status_message[403] = "FORBIDDEN";//Client lacks permission to access the resource
    status_message[404] = "NOT FOUND";//
    status_message[405] = "METHOD NOT ALLOWED";//
    status_message[408] = "REQUEST TIMEOUT";//Client took too long to send the request.petqa vor?
    status_message[413] = "PAYLOAD TOO LARGE";//
    status_message[415] = "UNSUPPORTED MEDIA TYPE";//POST
    status_message[422] = "UNPROCESSABLE ENTITY";//POST

    // Critical 5xx Server Error
    status_message[500] = "INTERNAL SERVER ERROR";//Generic server error (e.g., unexpected crash or CGI failure)
    status_message[501] = "NOT IMPLEMENTED";//Server does not support the requested method.,petqa vor?
    status_message[502] = "BAD GATEWAY";//Invalid response from an upstream server (e.g., CGI or proxy).
    status_message[503] = "SERVICE UNAVAILABLE";//Server temporarily unavailable (e.g., overloaded or maintenance)..petqa vor?  
    status_message[504] = "GATEWAY TIMEOUT";//Upstream server timed out (e.g., slow CGI script).petqa vor?  
    status_message[505] = "HTTP VERSION NOT SUPPORTED";
}

Request::Request(std::vector<ServerDirective *> servers)
{
    this->servers = servers;
    servIndex = -1;
    locIndex = -1;
    method = "";
    uri = "";
    file_type = "text/plain";
    query = "";
    error_page_num = -1;
    fill_status_message();
}

/////////////POST


std::string Request::urlDecode(const std::string &str)
{
    std::string result;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '+') {
            result += ' ';
        }
        else if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            }
            else {
                result += str[i];
            }
        }
        else {
            result += str[i];
        }
    }
    return result;
}

void Request::parseUrlEncodedForm(const std::string &body)
{
    form_data.clear();
    size_t pos = 0;
    while (pos < body.length()) {
        size_t amp_pos = body.find('&', pos);
        std::string pair = body.substr(pos, 
            (amp_pos == std::string::npos) ? body.length() - pos : amp_pos - pos);
        
        size_t eq_pos = pair.find('=');
        std::string key = (eq_pos == std::string::npos) ? pair : pair.substr(0, eq_pos);
        std::string value = (eq_pos == std::string::npos) ? "" : pair.substr(eq_pos + 1);
        
        form_data[urlDecode(key)] = urlDecode(value);
        
        pos = (amp_pos == std::string::npos) ? body.length() : amp_pos + 1;
    }
}

std::string Request::post_method_tasovka(char *buffer, int bytesRead) {
    
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    
    parse_post_request(buffer, bytesRead);

    bool supported_media_type = false;
    if (MainContentType.find("application/x-www-form-urlencoded") != std::string::npos ||
        MainContentType.find("multipart/form-data") != std::string::npos)
        supported_media_type = true;
    if (MainContentType.find("text/plain") != std::string::npos ||
        MainContentType.find("application/octet-stream") != std::string::npos)
        supported_media_type = true;
    if (!supported_media_type)
    {
        error_page_num = 415;
        throw std::runtime_error("Unsupported media type: " + MainContentType);
    }
    std::string response_body;
    std::string upload_dir = locdir[locIndex]->getUpload_dir();
    if (upload_dir.empty())
        {
            error_page_num = 403;
            throw std::runtime_error("upload_dir is empty");
        }
    std::string root = this->get_cwd() + locdir[locIndex]->getPath() + "/";
    
    if (upload_dir[0] == '/')
        upload_dir = upload_dir.substr(1);
    upload_dir = root + upload_dir;

    if (!upload_dir.empty() && upload_dir[upload_dir.size() - 1] == '/')
        upload_dir.erase(upload_dir.size() - 1);
    
    parseUrlEncodedForm(post_body);
    if (MainContentType.find("application/x-www-form-urlencoded") != std::string::npos) {
        std::stringstream ss;
        ss << "Form data received:\n";
        for (std::map<std::string, std::string>::const_iterator it = form_data.begin(); 
             it != form_data.end(); ++it) {
            ss << it->first << ": " << it->second << "\n";
        }
        response_body = ss.str();
    }
    else if (MainContentType.find("multipart/form-data") != std::string::npos)
        response_body = handle_multipart_upload(upload_dir);
    else
    {
        if (upload_dir.empty())
            upload_dir = this->get_cwd();
        response_body = handle_simple_post(upload_dir);
    }

    std::stringstream ss2;
    ss2 << error_page_num;
    std::stringstream ss;
    ss << "HTTP/1.1 " + ss2.str() + " " + status_message[error_page_num] + "\r\n"
       << "Content-Length: " << response_body.size() << "\r\n"
       << "Content-Type: " << get_content_type() + "\r\n"
       << "\r\n"
       << response_body;
    return ss.str();
}

void Request::parse_post_request(char *buffer, int bytesRead)
{
    std::string request_body;
    std::string buf_str = std::string(buffer, bytesRead);
    size_t body_start = buf_str.find("\r\n\r\n");

    if (body_start != std::string::npos) {
        request_body = buf_str.substr(body_start + 4);
        std::stringstream ss(buf_str.substr(0, body_start));
        std::string line;
        while (getline(ss, line)) {
            if (!line.empty() && line != "\r") {
                if (line.find("Content-Type: ") != std::string::npos) {
                    set_MainContentType(line);
                } else if (line.find("Content-Length: ") != std::string::npos) {
                    set_contentLength(line);
                }
            }
        }
    }

    this->post_body = request_body;
}

void Request::set_MainContentType(std::string line)
{
    size_t pos = line.find("Content-Type: ");
    if (pos != std::string::npos) {
        MainContentType = line.substr(pos + 14);
        MainContentType = MainContentType.substr(0, MainContentType.find(';'));
        if (MainContentType == "multipart/form-data")
        {
            size_t boundary_pos = line.find("boundary=");
            if (boundary_pos != std::string::npos)
            {
                boundary = "--" + line.substr(boundary_pos + 9);
                boundary = boundary.substr(0, boundary.find('\r'));
            }
        }
        else
            file_type = MainContentType;
    }
}

void Request::set_contentLength(std::string line)
{
    std::stringstream ss(line);
    std::string tmp;
    ss >> tmp >> contentLength;

    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    if (contentLength > locdir[locIndex]->getBodySize()) {
        error_page_num = 413;
        throw std::runtime_error("Contet Length is higher than our config max body size.");
    }
}

std::string Request::handle_multipart_upload(const std::string &upload_dir)
{
    std::string file_content;
    std::string file_path;
    std::string headers;
    std::string filename;

    if (boundary.empty()) {
        error_page_num = 400;
        throw std::runtime_error("Error: No boundary found for multipart/form-data");
    }
    if (!isDirectory(upload_dir) || access(upload_dir.c_str(), W_OK) != 0) {
        error_page_num = 403;
        throw std::runtime_error("Upload directory does not exist or is not writable");
    }
    std::string body = post_body;
    std::string response = "Files uploaded successfully:\n";
    size_t pos = 0;

    while ((pos = body.find(boundary)) != std::string::npos)
    {
        body = body.substr(pos + boundary.length());
        if (body.empty() || body.find("--") == 0)
            break;
        size_t header_end = body.find("\r\n\r\n");
        if (header_end == std::string::npos)
            continue;
        headers = body.substr(0, header_end);

        body = body.substr(header_end + 4);
        size_t part_end = body.find(boundary);
        if (part_end == std::string::npos)
            part_end = body.length();
        size_t filename_pos = headers.find("filename=\"");
        if (filename_pos != std::string::npos) {
            filename_pos += 10;
            size_t filename_end = headers.find("\"", filename_pos);
            filename = headers.substr(filename_pos, filename_end - filename_pos);
        }
        if (!filename.empty())
        {
            if (filename.find("/") != std::string::npos || filename.find("..") != std::string::npos) {
                error_page_num = 400;
                throw std::runtime_error("Filename contains invalid characters");
            }
            file_content = body.substr(0, part_end - 2);
            file_path = upload_dir + "/" + filename;
    
            std::string dir_path = file_path.substr(0, file_path.find_last_of('/'));
            check_directories(dir_path);
            int fd = open(file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd == -1) {
                error_page_num = 500;
                throw std::runtime_error("Error: Failed to open file for writing");
            }
            ssize_t written = write(fd, file_content.c_str(), file_content.size());
            close(fd);

            if (written == -1 || static_cast<size_t>(written) != file_content.size()) {
                error_page_num = 500;
                  throw std::runtime_error("Error: Failed to write file");
            }

            response += "Uploaded: " + filename + "\n";
        }
        // Обрезаем body для следующей части
        body = body.substr(part_end);
    }
    servers[servIndex]->set_file(file_path);
    return response;
}


void Request::check_directories(const std::string& dir_path)
{
    if (dir_path.empty()) {
        error_page_num = 403;
        throw std::runtime_error("Invalid directory path");
    }
    struct stat st;
    if (stat(dir_path.c_str(), &st) != 0) {
        error_page_num = 403;
        throw std::runtime_error("Directory does not exist: " + dir_path);
    }
    if (!S_ISDIR(st.st_mode)) {
        error_page_num = 403;
        throw std::runtime_error("Path is not a directory: " + dir_path);
    }
    if (access(dir_path.c_str(), W_OK) != 0) {
        error_page_num = 403;
        throw std::runtime_error("Directory is not writable: " + dir_path);
    }
}

std::string Request::handle_simple_post(const std::string &upload_dir)
{
    if (!isDirectory(upload_dir) || access(upload_dir.c_str(), W_OK) != 0) {
        error_page_num = 403;
        throw std::runtime_error("Upload directory does not exist or is not writable");
    }
    
    std::string file_path = upload_dir + "/" + form_data["name"];
    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        error_page_num = 500;
        throw std::runtime_error("Error: Failed to open file for writing");
    }

    for (std::map<std::string, std::string>::const_iterator it = form_data.begin(); it != form_data.end(); ++it) {
        if (it->first == "name") continue ;
        ssize_t written = write(fd, it->second.c_str(), it->second.length());
        if (written == -1 || static_cast<size_t>(written) != it->second.length()) {
            error_page_num = 500;
            throw std::runtime_error("Error: Failed to write POST data");
        }
    }
    close(fd);
    servers[servIndex]->set_file(file_path);
    return "POST data saved successfully";
}


bool Request::pathExists(const std::string& path)
{
    return (access(path.c_str(), F_OK) != -1);
}

bool Request::isFile(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

bool Request::isDirectory(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

std::string Request::getFilesInDirectory(std::string &path)
{
    std::vector<std::string> dirFiles;
    DIR *dir = opendir(path.c_str());
    if (!dir)
        return "NULL";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name != "." && name != "..")
            dirFiles.push_back(name);
    }
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    std::string index_value;
    for(size_t i = 0; i < locdir[locIndex]->getIndex().size(); ++i)
    {
        index_value = locdir[locIndex]->getIndex()[i];
        for(std::vector<std::string>::iterator it = dirFiles.begin(); it != dirFiles.end(); ++it)
        {
            if (index_value == *it)
            {
                closedir(dir);
                return index_value;
            }
        }
    }
    closedir(dir);
    return "NULL";
}

int Request::find_in_index_vectors_this_string(std::string left_part_of_filePath, std::vector<std::string> cur_loc_index)
{
    for(size_t i = 0; i < cur_loc_index.size(); ++i)
    {
        if (cur_loc_index[i] == left_part_of_filePath) {
            return i;
        }
    }
    return -1;
}

std::string Request::get_need_string_that_we_must_be_pass_send_system_call(std::string filePath)
{
    std::stringstream ss;
    if (filePath.find("</a>") == std::string::npos)
    {
        std::ifstream file(filePath.c_str());
        if (!file)
            std::cerr << "Failed to open file" << std::endl;
    
        ss << file.rdbuf();
    }
    else 
        ss << filePath;


    std::stringstream ss2;
    ss2 << error_page_num;
    std::string header = "HTTP/1.1 " + ss2.str() + " " + status_message[error_page_num] + "\r\nContent-Length: ";
    std::string whiteSpaces = "\r\n\r\n";
    std::stringstream ss1;
    ss1 << ss.str().size();

    return header + ss1.str() + whiteSpaces + ss.str();
}


std::string Request::uri_is_file(std::string filePath)
{
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    std::string str = getFilepath(locdir[locIndex]->getPath());
    size_t i = 0;
    for (; i < filePath.size() && i < str.size() && filePath[i] == str[i]; ++i)
        i++;
    if (i != str.size())
        i--;
    std::string left_part_of_filePath = filePath.substr(i);
    if (left_part_of_filePath[0] == '/')
        left_part_of_filePath.erase(0,1);
    currentIndex = find_in_index_vectors_this_string(left_part_of_filePath, locdir[locIndex]->getIndex());
    if (currentIndex < 0)
    {
        error_page_num = 404;
        throw std::runtime_error("not right location");
    }
    return get_need_string_that_we_must_be_pass_send_system_call(filePath);
}

void Request::listFiles(std::string path, std::vector<std::string> &files)
{
    DIR *dir = opendir(path.c_str());
    if (!dir)
        throw std::runtime_error("opendir error!");
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // Skip . and ..
        if (name == "." || name == "..")
            continue;
    
        std::string fullPath = path  + name;

        struct stat st;
        if (stat(fullPath.c_str(), &st) == -1) continue;

        if (S_ISDIR(st.st_mode))
            listFiles(fullPath, files);

        files.push_back(name);
    }
}

std::string Request::uri_is_directory(std::string filePath)
{
    if (filePath[filePath.size() - 1] != '/')
        filePath += '/';
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    std::string str = getFilepath(locdir[locIndex]->getPath());
    if (str[str.size() - 1] == '?')
        str = str.substr(0, str.size() - 1);
    if (str[str.size() - 1] != '/')
        str += '/';
    if (filePath != str)
    {
        error_page_num = 404;
        throw std::runtime_error("es el a inchvor exception");
    }
    else if (getFilesInDirectory(filePath) != "NULL")
    {
        if (filePath[filePath.size() - 1] != '/')
            filePath += '/' + getFilesInDirectory(filePath);
        else
            filePath += getFilesInDirectory(filePath);
        return get_need_string_that_we_must_be_pass_send_system_call(filePath);
    }
    if (filePath == str && locdir[locIndex]->getAutoindex() == "off")
    {
        error_page_num = 403;
        throw std::runtime_error("Error, I dont know of what type");
    }
    else
        return autoindex(filePath);
}

std::string Request::autoindex(std::string filePath)
{
    std::vector<std::string> files;
    listFiles(filePath, files);

    std::string listedFiles = "";

    std::vector<std::string>::iterator it = files.begin();
    for (; it != files.end(); it++)
    {
        std::string server;
        if (servers[servIndex]->getServer_name() != "")
            server = servers[servIndex]->getServer_name();
        else
            server = servers[servIndex]->getListen().first;
        std::stringstream ss_num;
        ss_num << server << ":" << servers[servIndex]->getListen().second;
        std::string uri_to_use = "";
        if (uri != "/") uri_to_use = uri;
        listedFiles += "<a href=\"http://" + ss_num.str() + uri_to_use + "/" + *it + "\">" + *it + "</a><br>";
    }
    return get_need_string_that_we_must_be_pass_send_system_call(listedFiles);
}

std::string Request::constructingResponce(std::string filePath)
{
    if (filePath[filePath.size() - 1] == '?')
        filePath = filePath.substr(0, filePath.size() - 1);
    if (pathExists(filePath) == false)
    {
        error_page_num = 404;//kam 403
        throw std::runtime_error("Path does not exist or no permission to file/directory");
    }
    if (isFile(filePath))
        return uri_is_file(filePath);
    else if (isDirectory(filePath))
        return uri_is_directory(filePath);
    return "";
}

static void    send_response(int client_fd, std::string response, int epfd)
{
    ssize_t bytesSent = send(client_fd, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        std::cerr << "Error sending response to client" << std::endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }
    if (bytesSent < (ssize_t)response.size()) {
        std::cerr << "Partial send, sent " << bytesSent << " of " << response.size() << " bytes" << std::endl;
    }
}

void Request::handleClientRequest(int client_fd) {
    char buffer[SIZE] = {0};
    ssize_t bytesRead = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        std::cerr << "Error reading from client socket" << std::endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }
    else if (bytesRead == 0) {
        std::stringstream ss;
        ss<<"Client disconnected: fd " << client_fd;
        Logger::printStatus("INFO", ss.str());
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        return;
    }

    Request_header_validation request_header_validation(servers);
    std::string response;
    try 
    {
        request_header_validation.if_received_request_valid(*this, buffer);
        request_header_validation.fill_headers_map(headers);
        locIndex = servers[servIndex]->get_locIndex();
        request_header_validation.status_handler();
        CGI cgi(this);
        switch(request_header_validation.get_status()) {
            case DYNAMIC:
                response = cgi.CGI_handler();
                break;
            case STATIC:
                response = get_response(method, buffer, bytesRead);
                break;
        }
        send_response(client_fd, response, epfd);
    } catch (std::exception &e) {
        Logger::printStatus("ERROR", e.what());
        if (error_page_num == -1) error_page_num = 500;
        std::string filePath;
        if (servIndex != -1)
        {
            std::map<int, std::string>::iterator it = servers[servIndex]->getError_pages().find(error_page_num);
            if (it == servers[servIndex]->getError_pages().end())
            {
                throw std::runtime_error("Error page for that code is not found");
            }
            std::string filename = it->second;
            std::string root =  servers[servIndex]->getRoot();
            filePath = root + "/" + filename;
        }
        if (servIndex == -1 || pathExists(filePath) == false ||  !isFile(filePath))
        {
            Logger::printStatus("WARNING", "The error page from config file couldn't be reached!");
            char root_char[1024] = {0};
            getcwd(root_char, 1024);
            std::stringstream ss;
            ss << error_page_num;
            filePath = std::string(root_char) + "/error_pages/" + ss.str() + ".html";
        }
        response = get_need_string_that_we_must_be_pass_send_system_call(filePath);
        send_response(client_fd, response, epfd);
    }
}

std::string Request::handleDelete(std::string filePath)
{
    if (!filePath.empty() && filePath[filePath.size() - 1] == '?') {
        filePath = filePath.substr(0, filePath.size() - 1);
    }
    if (filePath.find("..") != std::string::npos) {
        error_page_num = 400;
        throw std::runtime_error("Invalid path: contains parent directory reference");
    }
    if (!pathExists(filePath)) {
        error_page_num = 404;
        throw std::runtime_error("Path does not exist");
    }
    if (access(filePath.c_str(), W_OK) != 0) {
        error_page_num = 403;
        throw std::runtime_error("Permission denied for deletion");
    }

    if (isFile(filePath)) {
        // if (servers[servIndex]->get_file(filePath).empty())
        // {
        //     error_page_num = 403;
        //     throw std::runtime_error("File can't be deleted, you did't create it!");
        // }
        if (std::remove(filePath.c_str()) != 0 && servers[servIndex]->get_file(filePath.c_str()) != "") {
            error_page_num = 500;
            throw std::runtime_error("Failed to delete file");
        }
    } else if (isDirectory(filePath)) {
        error_page_num = 403;
        throw std::runtime_error("Directory deletion is not supported");
    } else {
        error_page_num = 403;
        throw std::runtime_error("Path is neither a file nor a directory");
    }
    std::string response = "HTTP/1.1 204 No Content\r\n";
    response += "Content-Length: 0\r\n";
    response += "\r\n";
    return response;
}

std::string Request::get_response(std::string &method, char *buffer, int bytesRead)
{
    std::string res;
    if (method == "GET")
    {
        error_page_num = 200;
        //////redirecti masna//////
        if (!servers[servIndex]->getLocdir()[servers[servIndex]->get_locIndex()]->getRedirect().empty())
        {
            error_page_num = servers[servIndex]->getLocdir()[servers[servIndex]->get_locIndex()]->getRedirect().begin()->first;
            std::string filePath = servers[servIndex]->getLocdir()[servers[servIndex]->get_locIndex()]->getRedirect().begin()->second;
            return generateRedirectResponse(filePath);
        }
        ////////////////////////////////
        std::string filePath = getFilepath(uri);
        res = constructingResponce(filePath);
    }
    else if(method == "POST")
    {
        error_page_num = 201;
        res = post_method_tasovka(buffer, bytesRead);
    }
    else if (method == "DELETE")
    {
        error_page_num = 204;
        std::string filePath = getFilepath(uri);
        res = handleDelete(filePath);
    }
    return res;
}

int Request::if_http_is_valid(char *c_buffer)
{
    std::stringstream ss_h(c_buffer);
    std::string first_line;
    getline(ss_h, first_line);

    std::stringstream ss(first_line);
    std::string http_version;
    ss >> http_version;
    ss >> http_version;
    ss >> http_version;
    if (http_version != "HTTP/1.1")
        return -20;
    return 1;
}

std::string Request::getFilepath(std::string needly_atribute)
{
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    std::string root = this->get_cwd();
    if (method == "DELETE")
    {
        root += locdir[locIndex]->getPath();
        if (std::strncmp(locdir[locIndex]->getPath().c_str(), needly_atribute.c_str(), locdir[locIndex]->getPath().size()) == 0)
            needly_atribute = needly_atribute.substr(locdir[locIndex]->getPath().size());
    }
    std::string filePath = root + needly_atribute;
    return filePath;
}


std::string Request::get_cwd()
{
    return (servers[servIndex]->getLocdir()[locIndex]->getRoot() != "") ? servers[servIndex]->getLocdir()[locIndex]->getRoot() : servers[servIndex]->getRoot();
}


//redirect////
std::string Request::generateRedirectResponse(std::string filePath) {
    std::string response;
    std::stringstream ss;
    ss << error_page_num;
    response += "HTTP/1.1 " + ss.str() + " ";
   
    response += status_message[error_page_num] + "\r\n";
    response += "Location: " + filePath + "\r\n";
    response += "Content-Length: 0\r\n";
    response += "Connection: keep-alive\r\n";
    response += "Cache-Control: no-cache\r\n";
    response += "\r\n";
    return response;
}

