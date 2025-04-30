#include "Request.hpp"

Request::Request(std::vector<ServerDirective *> servers)
{
    this->servers = servers;
    // client_fd = -1;
    servIndex = -1;
    locIndex = -1;
    method = "";
    uri = "";//GET-i hamar
    file_type = "text/plain";//default
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
    std::cout << "ekanq parseUrlEncodedForm\n";
    form_data.clear();
    size_t pos = 0;
    std::cout << "body->"<<body <<std::endl;
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
    std::cout << "matryyyyyyyyyyyyy>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << buffer<<std::endl;
    parse_post_request(buffer, bytesRead);
    
    // Handle error cases first
    if (error_page_num == 413) {
        std::string filePath = getFilepath("/web/error413.html");
        return get_need_string_that_we_must_be_pass_send_system_call(filePath);
    }

    std::string response_body;
    std::string upload_dir = locdir[locIndex]->getUpload_dir();
    
    // Handle different content types
    if (MainContentType.find("application/x-www-form-urlencoded") != std::string::npos) {
        parseUrlEncodedForm(post_body);
        
        // Build response from parsed form data
        std::stringstream ss;
        ss << "Form data received:\n";
        for (std::map<std::string, std::string>::const_iterator it = form_data.begin(); 
             it != form_data.end(); ++it) {
            ss << it->first << ": " << it->second << "\n";
        }
        response_body = ss.str();
    }
    else if (MainContentType.find("multipart/form-data") != std::string::npos)
    {
        std::cout << "VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n";
        if (upload_dir.empty())
        {
            std::string filePath = getFilepath("/web/error403.html");
            return get_need_string_that_we_must_be_pass_send_system_call(filePath);
        }
        // Process upload directory path
        std::string root = (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        if (upload_dir[0] != '/')
            upload_dir = root + "/" + upload_dir;
        else
            upload_dir = root + upload_dir.substr(1);
        
        // Remove trailing slash if present
        if (!upload_dir.empty() && upload_dir[upload_dir.size() - 1] == '/')
            upload_dir.erase(upload_dir.size() - 1);
        
        response_body = handle_multipart_upload(upload_dir);
    } 
    else
    {
        // Handle other content types (raw data)
        if (upload_dir.empty())
            upload_dir = (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        std::cout << "======" << upload_dir << std::endl;
        response_body = handle_simple_post(upload_dir);
    }

    // Build HTTP response
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\r\n"
       << "Content-Length: " << response_body.size() << "\r\n"
       << "Content-Type: text/plain\r\n"
       << "\r\n"
       << response_body;

    return ss.str();
}

// Парсинг POST-запроса
void Request::parse_post_request(char *buffer, int bytesRead)
{
    std::cout << "buffer = " << buffer<<std::endl;
    std::cout << "prc buffer\n";
    std::string request_body;
    std::string buf_str = std::string(buffer, bytesRead);
    size_t body_start = buf_str.find("\r\n\r\n");

    // Сбрасываем код ошибки
    error_page_num = 0;

    if (body_start != std::string::npos) {
        // Извлекаем тело запроса (включая все \r\n)
        request_body = buf_str.substr(body_start + 4);
        std::cout << "uzmes ases->" << request_body<<std::endl;
        // Обрабатываем заголовки
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

    // Сохраняем тело для дальнейшей обработки
    this->post_body = request_body;
    std::cout << "Content type is->" << MainContentType << std::endl;
    std::cout << "LENGHT->" << contentLength << std::endl << std::endl;

    std::cout << "POST BODY->" << post_body << std::endl<<std::endl<<std::endl;
}

// Установка Content-Type и границы для multipart
void Request::set_MainContentType(std::string line)
{
    size_t pos = line.find("Content-Type: ");
    if (pos != std::string::npos) {
        MainContentType = line.substr(pos + 14);
        MainContentType = MainContentType.substr(0, MainContentType.find(';'));
        std::cout << "MainContentType = " <<MainContentType<<std::endl;
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
    std::cout << "HELPER->" << file_type<< std::endl;
    std::cout << "BOUNDARY->" << boundary<< std::endl;

}

// Установка Content-Length и проверка на превышение лимита
void Request::set_contentLength(std::string line)
{
    std::stringstream ss(line);
    std::string tmp;
    ss >> tmp >> contentLength;

    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    if (contentLength > locdir[locIndex]->getBodySize()) {
        error_page_num = 413; // Request Entity Too Large
    }
}

std::string Request::handle_multipart_upload(const std::string &upload_dir)
{
    std::string file_content;
    std::string file_path;
    std::string headers;
    std::string filename;
    if (boundary.empty()) {
        return "Error: No boundary found for multipart/form-data";
    }

    std::string body = post_body;
    std::string response = "Files uploaded successfully:\n";
    size_t pos = 0;

    // Создаем директорию, если она не существует
    mkdir(upload_dir.c_str(), 0755);

    while ((pos = body.find(boundary)) != std::string::npos)
    {
        body = body.substr(pos + boundary.length());
        std::cout << "cur body->" << body << std::endl;
        if (body.empty() || body.find("--") == 0)//body pustoy ili body начинается с двух дефисов --.
            break; // Конец multipart
        // Извлекаем заголовки части
        size_t header_end = body.find("\r\n\r\n");
        if (header_end == std::string::npos)
        {
            std::cout << "continue??\n";
            continue;
        }
        headers = body.substr(0, header_end);
        std::cout << "headers->" << headers << std::endl<<std::endl;

        body = body.substr(header_end + 4);
        std::cout << "do body->" << body << std::endl<<std::endl;
        // Находим конец части
        size_t part_end = body.find(boundary);
        if (part_end == std::string::npos)
            part_end = body.length();
        // Ищем имя файла в Content-Disposition
        size_t filename_pos = headers.find("filename=\"");
        if (filename_pos != std::string::npos) {
            filename_pos += 10;
            size_t filename_end = headers.find("\"", filename_pos);
            filename = headers.substr(filename_pos, filename_end - filename_pos);
        }
        std::cout << "FILENAME-> "<< filename << std::endl;
        if (!filename.empty()) {
            // Извлекаем содержимое файла
            file_content = body.substr(0, part_end - 2); // Удаляем \r\n

            file_path = upload_dir + "/" + filename;

            std::cout << "FILE PATH->" << file_path <<std::endl;
            std::string dir_path = file_path.substr(0, file_path.find_last_of('/'));
            create_directories(dir_path);

            int fd = open(file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd == -1) {
                return "Error: Failed to open file for writing";
            }
            std::cout << "new fd = " << fd <<std::endl;
            ssize_t written = write(fd, file_content.c_str(), file_content.size());
            close(fd);

            if (written == -1 || static_cast<size_t>(written) != file_content.size()) {
                return "Error: Failed to write file";
            }

            response += "Uploaded: " + filename + "\n";
        }
        // Обрезаем body для следующей части
        body = body.substr(part_end);
    }
    std::cout << "posle body->" << body << std::endl;
    std::cout << "BOUNDARTY is->" << boundary << std::endl;
    std::cout << "HEADER->" << headers << std::endl;
    std::cout << "FILE-CONTENT-<" << file_content <<std::endl;
    std::cout << "FILE_PATH ->" << file_path <<std::endl;
    std::cout << "RESSSSSSSPONSE->" << response << std::endl;
    return response;
}

// Обработка простых POST-запросов (не multipart)
std::string Request::handle_simple_post(const std::string &upload_dir)
{
    // Сохраняем тело POST в файл post_data.txt в upload_dir
    std::string file_path = upload_dir + "/post_data.txt";
    int fd = open(file_path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        return "Error: Failed to open file for writing";
    }

    ssize_t written = write(fd, post_body.c_str(), post_body.size());
    close(fd);

    if (written == -1 || static_cast<size_t>(written) != post_body.size()) {
        return "Error: Failed to write POST data";
    }

    return "POST data saved successfully";
}

// Вспомогательная функция для рекурсивного создания директорий
void Request::create_directories(const std::string &path)
 {
    std::string current_path;
    for (size_t i = 0; i < path.size(); ++i) {
        if (path[i] == '/') {
            if (!current_path.empty()) {
                mkdir(current_path.c_str(), 0755);
            }
        }
        current_path += path[i];
    }
    if (!current_path.empty()) {
        mkdir(current_path.c_str(), 0755);
    }
}//esi ashxatuma bayc mihat haskanal vonca ashxatum:)))))


//////////////GET
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
        std::cout << "direktoriayi meji faylery->" << name << std::endl;
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
        // std::cout << "cur_loc_index i-n  = " <<cur_loc_index[i]<< std::endl;
        if (cur_loc_index[i] == left_part_of_filePath)
            return i;
    }
    return -1;
}

std::string Request::get_need_string_that_we_must_be_pass_send_system_call(std::string filePath)
{
    std::stringstream ss;
    if (filePath.find("</a>") == std::string::npos)
    {
        std::cout<<filePath<<"    🦔🦔🦔    \n";
        std::ifstream file(filePath.c_str());
        if (!file)
            std::cerr << "Failed to open file" << std::endl;
    
        ss << file.rdbuf(); // read entire content
    }
    else 
        ss << filePath;


    // После обработки можешь отправить ответ:

    std::string header = "HTTP/1.1 200 OK\r\nContent-Length: ";

    std::string whiteSpaces = "\r\n\r\n";

    std::stringstream ss1;
    ss1 << ss.str().size();

    return header + ss1.str() + whiteSpaces + ss.str();
}


std::string Request::uri_is_file(std::string filePath)
{
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex(); 
    std::cout << "locindexxxxxxxxxxx = " <<   locIndex<< std::endl;
    std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
    std::string str = root + locdir[locIndex]->getPath();
    std::cout << "strse havasraaaaa->>>>>>>" << str << std::endl;
    size_t i = 0;
    for (; i < filePath.size() && i < str.size() && filePath[i] == str[i]; ++i)
        i++;
    if (i != str.size())
        i--;
    std::cout << "i ===== " << i << std::endl;
    std::string left_part_of_filePath = filePath.substr(i);
    if (left_part_of_filePath[0] == '/')
        left_part_of_filePath.erase(0,1);
    std::cout << "------------------------------>" << left_part_of_filePath << std::endl;
    if (find_in_index_vectors_this_string(left_part_of_filePath, locdir[locIndex]->getIndex()) < 0)
    {
        std::cout << "error page???????\n";
        // std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        // filePath = root + "/web/error404.html";
        filePath = getFilepath("/web/error404.html");
    }
    //////////
    std::cout << "ete esi tpvela uremn jokela vor fayla\n";
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
    std::cout << "direktoriayi vaxt filepathy ekela->" << filePath << std::endl;
    std::cout << "ete esi tpvela uremn jokela vor DIREKTORIAya\n";
    if (filePath[filePath.size() - 1] != '/')
        filePath += '/';
    ////////krknvouma 
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
    std::cout << "rooooooooooooooot = " << root << std::endl;
    std::string str = root + locdir[locIndex]->getPath();
    // size_t i = 0;
    if (str[str.size() - 1] == '?')
        str = str.substr(0, str.size() - 1);
    if (str[str.size() - 1] != '/')
        str += '/';
    std::cout << "fiiiiiiiiiiiiiile->" << filePath << std::endl;
    std::cout << "strne senc mometa->" << str << std::endl;
    ////////////////
    if (filePath != str)
    {
        // std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        // filePath = root + "/web/error404.html";
        filePath = getFilepath("/web/error404.html");
    }
    else if (getFilesInDirectory(filePath) != "NULL")//filePath == str esi el petq chi stugel,verevy ka
    {
        if (filePath[filePath.size() - 1] != '/')
            filePath += '/' + getFilesInDirectory(filePath);
        else
        filePath += getFilesInDirectory(filePath);
            
        std::cout << "/ enq morace hastat->" << filePath << std::endl;
        return get_need_string_that_we_must_be_pass_send_system_call(filePath);
    }
    std::cout << "ba incha->" << locdir[locIndex]->getAutoindex() << std::endl;
    if (filePath == str && locdir[locIndex]->getAutoindex() == "off")
    {
        // std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        // filePath = root + "/web/error403.html";
        filePath = getFilepath("/web/error403.html");
        return get_need_string_that_we_must_be_pass_send_system_call(filePath);
    }
    else
    {
        std::vector<std::string> files;
        listFiles(filePath, files);

        std::string listedFiles = "";

        std::vector<std::string>::iterator it = files.begin();
        std::cout<<"🎩🎩🎩🎩\n";
        for (; it != files.end(); it++)
        {
            std::cout<<*it<<std::endl;
            std::stringstream ss_num;
            ss_num << servers[servIndex]->getListen().first << ":" << servers[servIndex]->getListen().second;

            std::cout<<"🍄🍄🍄 "<<uri<<" 🍄🍄🍄"<<std::endl;
            std::string uri_to_use = "";
            if (uri != "/") uri_to_use = uri;
            listedFiles += "<a href=\"http://" + ss_num.str() + uri_to_use + "/" + *it + "\">" + *it + "</a><br>";
        }
        std::cout<<"🎩🎩🎩🎩\n";

        std::cout<<listedFiles<<std::endl;
        std::cout<<"🐞🐞🐞\n";
        return get_need_string_that_we_must_be_pass_send_system_call(listedFiles);
    }
    return "";
}

std::string Request::constructingResponce(std::string filePath)
{
    std::cout << "filepath === " << filePath << std::endl;
    if (filePath[filePath.size() - 1] == '?')
        filePath = filePath.substr(0, filePath.size() - 1);
    if (pathExists(filePath) == false)
        throw std::runtime_error(" путь не существует или нет прав на доступ(файл/директория)");
    if (isFile(filePath)) //Если ты проверяешь путь и он:❌ не существует — верни ошибку 404 Not Found
    {
        //stex pti stugenq ardyoq mer locationi index i valuneri mej ka tvyal fayly te che
        //////////
        return uri_is_file(filePath);
    }
    else if (isDirectory(filePath))
        return uri_is_directory(filePath);
    return "";
}
#define SIZE (1 << 14)
void Request::handleClientRequest(int client_fd) {
    std::cout << "SIZEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEee=" << SIZE<<std::endl;
    char buffer[SIZE] = {0};
    std::cout << "vvvvv\n";
    ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer));
    std::cout << "mda = " << bytesRead<<std::endl;
    if (bytesRead == -1) {
        std::cerr << "Error reading from client socket" << std::endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL); // 🔻 Удаляем из epoll
        close(client_fd); // Закрываем сокет при ошибке
        return;
    }
    else if (bytesRead == 0) {
        std::cout << "Client disconnected: fd " << client_fd << std::endl;
        epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, NULL); // 🔻 Удаляем из epoll
        close(client_fd); // Закрываем сокет, если клиент отключился
        return;
    }

    // Здесь ты можешь обработать запрос клиента
    std::cout << "Received request: " << std::string(buffer, bytesRead) << std::endl;

    Request_header_validation request_header_validation(servers);

    //checking if http request header is correct
    std::string method = request_header_validation.if_received_request_valid(buffer);
    uri = request_header_validation.get_uri();
    servIndex = request_header_validation.get_servIndex();
    ////////////////////////////////////    
    // std::string filePath = servers[0]->getRoot() + servers[0]->getLocdir()[0]->getPath() + "/" + servers[0]->getLocdir()[0]->getIndex()[1];
    std::cout << "methoooooooood = " << method <<std::endl;
    // std::string filePath = constructingFilePath();
    if (if_http_is_valid(buffer) < 0)
        std::cout << "505 HTTP Version Not Supported.\n";
    //
    std::cout<<"reached here\n";
    if (method == "GET")
    {
        std::cout<<"SERVINDEX: "<<servIndex<<std::endl;
        // std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
        // int locIndex = servers[servIndex]->get_locIndex();
        // std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
        // std::string filePath = root + uri;
        std::string filePath = getFilepath(uri);
        std::string res = constructingResponce(filePath);
        std::cout<<"-----------------------------------\n";
        std::cout<<res;
        std::cout<<"-----------------------------------\n";
        const char *response = res.c_str();
        send(client_fd, response, strlen(response), 0);
    }
    else if(method == "POST")
    {
        std::cout << "uri = " << uri << std::endl;
        std::cout << "whic =" << servers[servIndex]->get_locIndex()<<std::endl;
        std::string res = post_method_tasovka(buffer, bytesRead);
        const char *response = res.c_str();
        send(client_fd, response, strlen(response), 0);
    }
    // else if (method == "DELETE")
    //
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

    //В HTTP/1.1 он обязателен.
    // Если отсутствует — 400 Bad Request.senc bana asum gpt-n mihat stugel ete nenc request kara ga vor bacakayi et http1-@ lracnenq et masy
    std::cout << "hmis stex pti http_version lini http->" << http_version << std::endl;
    if (http_version != "HTTP/1.1")
        return -20;
    return 1;
}



// std::string Request::getFilepath()
// {
//     std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
//     int locIndex = servers[servIndex]->get_locIndex();
//     std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
//     std::string filePath = root + uri;
//     return filePath;
// }
std::string Request::getFilepath(std::string uri)
{
    std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
    int locIndex = servers[servIndex]->get_locIndex();
    std::string root =  (locdir[locIndex]->getRoot() != "") ? locdir[locIndex]->getRoot() : servers[servIndex]->getRoot();
    std::string filePath = root + uri;
    return filePath;
}