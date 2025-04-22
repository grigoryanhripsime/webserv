#include "Servers.hpp"
#include <dirent.h>

Servers::Servers(DirectiveConfig &dirConf)
{
    std::cout << "Server ctor is called\n";
    config = &dirConf;

    connectingServerToSocket();

    runningProcess();

}

void Servers::connectingServerToSocket()
{
    
    std::vector<ServerSocket> helper(config->get_servers().size(), ServerSocket());
    servSock = helper;//helpery zut datark vektora mer serverni qanakov, mez petqer vor fiqsvac serverni qanakov vektor sarqeinq vor el insert,erase chaneinq zut et null-i vra arjeqavoreinq


    std::map<std::pair<std::string, int>, std::vector<int> > unique_listens = config->get_unique_listens();
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it  = unique_listens.begin();
    std::cout<<"🦔🦔🦔 "<<unique_listens.size()<<std::endl;
    for(; it != unique_listens.end(); ++it)
    {
        ServerSocket sock = ServerSocket(AF_INET, SOCK_STREAM, 0, (it)->first.second, (it)->first.first, 10);
        for (size_t i = 0; i < it->second.size(); i++)
        {
            std::cout<<"🦝🦝🦝  "<<i<< " " << it->second[i] <<std::endl;
            servSock[it->second[i]] = sock;
        }
    }

    //ensuring that everything is right
    for (size_t k = 0; k < servSock.size(); k++)
    {
        std::cout<<"☀️☀️☀️   "<<servSock[k].get_socket()<<std::endl;
    }
}


void Servers::runningProcess()
{
    setupEpoll();
    runLoop();
}

void Servers::setupEpoll()
{
    epfd = epoll_create(1);//создаёт новый epoll instance (дескриптор). Он нужен, чтобы отслеживать события на сокетах (например, кто-то подключился).
    if (epfd == -1)
        throw std::runtime_error("epoll_create failed");
    std::cout << "epfd = " << epfd << std::endl;
    // Регистрируем каждый server socket в epoll
    std::map<std::pair<std::string, int>, std::vector<int> > unique_listens = config->get_unique_listens();
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it = unique_listens.begin();
    for (; it != unique_listens.end(); ++it)
    {
        int fd = servSock[it->second[0]].get_socket();// Получаем сокет server_fd
        std::cout << "fdddddddddddd = " << fd << std::endl;

        struct epoll_event ev;// Создаём epoll_event, чтобы указать, какие события мы хотим слушать.
        ev.events = EPOLLIN;// мы хотим знать, когда на сокете появятся входящие данные (например, клиент хочет подключиться).
        ev.data.fd = fd;// сохраняем сам сокет, чтобы потом понять, на каком сокете произошло событие.

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        {//Регистрируем серверный сокет в epoll, чтобы он следил за событиями на нём.
            throw std::runtime_error("epoll_ctl ADD failed");
        }
        std::cout << "Registered server socket fd: " << fd << std::endl;
    }
}

void Servers::runLoop()
{
    struct epoll_event events[MAX_EVENTS];
    size_t j;
    while (true)
    {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n == -1)
            throw std::runtime_error("epoll_wait failed");
        for (int i = 0; i < n; ++i)
        {
            int sockfd = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                bool isServer = false;
                for (j = 0; j < servSock.size(); ++j)
                {
                    if (sockfd == servSock[j].get_socket())
                    {
                        acceptClient(sockfd);
                        isServer = true;
                        break;
                    }
                }
                if (!isServer)
                {
                    try{
                        std::cout << "kanchvav" << std::endl;
                        handleClientRequest(sockfd);
                    }
                    catch(std::exception& e)
                    {
                        std::cout << "EXception: " << e.what() << std::endl;
                    }
                }
            }
            else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
            {
                epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                close(sockfd);
                throw std::runtime_error("Error or hangup detected on socket " + sockfd);
            }
        }
    }
}

void Servers::acceptClient(int server_fd)
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int client_fd = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);//Принимаем входящее соединение. Возвращается новый сокет для общения с этим клиентом (отдельно от серверного).
    
    if (client_fd == -1) {
        std::cerr << "Failed to accept client connection" << std::endl;
        return;
    }

    // Зарегистрируем новый клиентский сокет в epoll
    // Регистрируем новый клиентский сокет в epoll, чтобы следить за его входящими данными.
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        close(client_fd);
    } else {
        std::cout << "Accepted new client connection: fd " << client_fd << std::endl;
    }
}


int Servers::getServerThatWeConnectTo(std::string buffer)
{
    std::stringstream ss(buffer);
    std::string line;
    while (std::getline(ss, line) && line.find("Host: ") == std::string::npos)
        continue;
    
    std::string serverName = line.substr(6);//karevor atributa, requesti meji Host: -i dimaci grvacna(minchev porty(:8080))
    serverName = serverName.substr(0, serverName.find(":"));
    std::cout<<"⛵️⛵️⛵️⛵️"<<serverName<<std::endl;

    std::vector<ServerDirective *> servers = config->get_servers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i]->getServer_name() == serverName)
        //    || serverName == servers[i]->getListen().first)
        {
            std::cout << "chishta Vrds\n";
            return i;
        }
    }
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (serverName == servers[i]->getListen().first)
            return i;
    }//arandzin for-erov em grel nra hamar vortev`
    //ete unenq nuyn ip-ov ev port-ov serverner bayc tvelenq server_name apa zaprosty etalua et name-ov serverin
    //bayc ete unenq nuyn ip-ov ev port-ov serverner bayc chenq tvel server_name apa zaprosty etalua arajin et ip-ov u prot-ov serverin
    std::cout << "en vat depqna\n";
    return 0;
}

void Servers::handleClientRequest(int client_fd) {
    char buffer[1024];
    ssize_t bytesRead = read(client_fd, buffer, sizeof(buffer));

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
    
    //checking if http request header is correct
    std::string method = if_received_request_valid(buffer);
    ////////////////////////////////////    
    // std::string filePath = config->get_servers()[0]->getRoot() + config->get_servers()[0]->getLocdir()[0]->getPath() + "/" + config->get_servers()[0]->getLocdir()[0]->getIndex()[1];
    std::cout << "methoooooooood = " << method <<std::endl;
    // std::string filePath = constructingFilePath();
    if (method == "invalid")
    {
        std::cout << "senc method chunenq mer allow_methods-um-> 405 Method Not Allowed.\n";//return 77;
        std::string filePath = config->get_servers()[servIndex]->getRoot() + "/web/error405.html";
        std::string res = get_need_string_that_we_must_be_pass_send_system_call(filePath);
        const char *response = res.c_str();
        send(client_fd, response, strlen(response), 0);
        //en fileparhy= i erku toxyba + send-@
    }
    else
    {
        if (if_http_is_valid(buffer) < 0)
            std::cout << "505 HTTP Version Not Supported.\n";
        //
        if (method == "GET")
        {
            std::string filePath = config->get_servers()[servIndex]->getRoot() + uri;
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
            std::cout << "whic =" << config->get_servers()[servIndex]->get_locIndex()<<std::endl;

            std::vector<LocationDirective*> locdir = config->get_servers()[servIndex]->getLocdir();
            int locIndex = config->get_servers()[servIndex]->get_locIndex();
             std::string upload_dir = locdir[locIndex]->getUpload_dir();
            std::cout << "opload->" << upload_dir << std::endl;
            if (upload_dir[0] == '/')
                std::cout << "eee\n";
                //uremn inqy absalute patha ,ira mejenqw stexcveliq fayly avelacnelu
        //hakarak depqum root-um?
        }
        // else if (method == "DELETE")
        //
    }

}

std::string Servers::uri_is_file(std::string filePath)
{
    std::vector<LocationDirective*> locdir = config->get_servers()[servIndex]->getLocdir();
    int locIndex = config->get_servers()[servIndex]->get_locIndex();    
    std::string str = config->get_servers()[servIndex]->getRoot() + locdir[locIndex]->getPath();
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
        filePath = config->get_servers()[servIndex]->getRoot() + "/web/error404.html";
    }
    //////////
    std::cout << "ete esi tpvela uremn jokela vor fayla\n";
    return get_need_string_that_we_must_be_pass_send_system_call(filePath);
}

void Servers::listFiles(std::string path, std::vector<std::string> &files)
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

std::string Servers::uri_is_directory(std::string filePath)
{
    std::cout << "direktoriayi vaxt filepathy ekela->" << filePath << std::endl;
    std::cout << "ete esi tpvela uremn jokela vor DIREKTORIAya\n";
    if (filePath[filePath.size() - 1] != '/')
        filePath += '/';
    ////////krknvouma 
    std::vector<LocationDirective*> locdir = config->get_servers()[servIndex]->getLocdir();
    int locIndex = config->get_servers()[servIndex]->get_locIndex();
    // int locIndex = config->get_servers()[servIndex]->get_locIndex();
    std::string str = config->get_servers()[servIndex]->getRoot() + locdir[locIndex]->getPath();
    // size_t i = 0;
    if (str[str.size() - 1] != '/')
        str += '/';
    std::cout << "fiiiiiiiiiiiiiile->" << filePath << std::endl;
    std::cout << "strne senc mometa->" << str << std::endl;
    ////////////////
    if (filePath != str)
        filePath = config->get_servers()[servIndex]->getRoot() + "/web/error404.html";
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
        filePath = config->get_servers()[servIndex]->getRoot() + "/web/error403.html";
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
            ss_num << config->get_servers()[servIndex]->getListen().first << ":" << config->get_servers()[servIndex]->getListen().second;

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

std::string Servers::constructingResponce(std::string filePath)
{

    std::cout << "filepath === " << filePath << std::endl;
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
    
std::string Servers::get_location(char *buffer)
{
    std::string strbuffer(buffer);
    size_t pos = strbuffer.find('\n');
    std::string firstLineInBuffer = strbuffer.substr(0, pos - 1);
    std::cout << "harcum-> " << firstLineInBuffer << std::endl;
    size_t pos_slash = firstLineInBuffer.find(' ');
    firstLineInBuffer.erase(0,pos_slash + 1);
    size_t pos_loc_end = firstLineInBuffer.find(' ');
    std::string uri = firstLineInBuffer.substr(0, pos_loc_end);
    return uri;
}

int Servers::have_this_uri_in_our_current_server(int servIndex)
{
    std::cout << "servIndex->" << servIndex << std::endl;
    std::vector<LocationDirective*> vec_locations = config->get_servers()[servIndex]->getLocdir();
    int which_location = -1;
    std::string path;
    size_t length = 0;
    std::cout << "mihat joekqn hastat chisht uri a->" << uri <<  std::endl;
    for(size_t i = 0; i < vec_locations.size(); ++i)
    {
        path = vec_locations[i]->getPath();
        std::cout << "yntacik locpath = " << path << std::endl;
        if (path.size() > uri.size())
            continue ;
        size_t tmpLength = 0;
        size_t j;
        for (j = 1; j < uri.size() && path[j] == uri[j]; ++j)//1ic em sksum vortevdemi simvoli saxi mot /a linelu
            tmpLength++;
        if (j == uri.size() && j == path.size())
        {
            length = tmpLength;
            which_location = i;
        }
        else if (tmpLength > length)
        {
            length = tmpLength;
            which_location = i;
        }
    }
    std::cout << "vitoge which_locastin = " << which_location << std::endl;
    if (which_location == -1)
    {
        std::cout << "CHfind location path\n\n\n";
        return -1;
    }
    return which_location;
}

std::string    Servers::if_received_request_valid(char *c_buffer)
{
    std::string method;
    validation_of_the_first_line(c_buffer, method);
    //TODO: add other lines
    return method;
}

std::string    Servers::validation_of_the_first_line(char *c_buffer, std::string& method)
{
    servIndex = getServerThatWeConnectTo(c_buffer);//esi unenq vor serverna
    //////////////////////////////
    uri = get_location(c_buffer);
    // int locIndex = config->get_servers()[servIndex]->get_locIndex();
    int locIndex = have_this_uri_in_our_current_server(servIndex);//esi arajin toxi locationi masi pahna
    config->get_servers()[servIndex]->setLocIndex(locIndex);//set locIndex
    if (locIndex < 0)
    {
        std::cout << "yavni bacaskaanaaaaaa\n";
        throw std::runtime_error("error page pti bacvi browser-um");//es hmi exception em qcum vor segfault chta,bayc heto pti zut error page-@ bacenq
    }
    //////////////////////////////
    
    ///////////////////////////////
    std::stringstream ss(c_buffer);
    std::string first_line;
    getline(ss, first_line);
std::cout << "incha vorrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr " << method_is_valid(first_line, locIndex, method)<<std::endl;
    if (method_is_valid(first_line, locIndex, method) < 0)
    {
        method = "invalid";
        std::cout << "first line is not validdddddddddddddddddddddd\n";
    }
    return method;
    ////////////////////////////////////////
}


int Servers::if_http_is_valid(char *c_buffer)
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

int Servers::method_is_valid(std::string first_line, int which_location, std::string& method)
{
    std::stringstream ss(first_line);
    // std::string method;
    ss >> method;
    std::cout << "method->" << method  <<std::endl;
    if (check_this_metdod_has_in_appropriate_server(method, which_location) < 0)
        return (-777);
    return 1;

}

int Servers::check_this_metdod_has_in_appropriate_server(std::string method, int which_location)
{
    LocationDirective* locdir;
    std::cout << "hasav stxe\n";
    std::cout << "which_location = " << which_location << std::endl;
    locdir = config->get_servers()[servIndex]->getLocdir()[which_location];
    // std::cout << "tvyal locdir" << locdir->getPath() << std::endl;
    std::vector<std::string> allow_methods = locdir->getAllow_methods();
    for(size_t i = 0; i < allow_methods.size(); ++i)
    {
        std::cout << "curent metodne->" << allow_methods[i] << std::endl;
        if (allow_methods[i] == method)
            return 1;//ka
    }
    return -1;//chkar tenc metod
}


Servers::~Servers(){
    std::cout << "Servers dtor is called\n";

}



bool Servers::pathExists(const std::string& path)
{
    return (access(path.c_str(), F_OK) != -1);
}

bool Servers::isFile(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

bool Servers::isDirectory(const std::string& path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

std::string Servers::getFilesInDirectory(std::string &path)
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
    std::vector<LocationDirective*> locdir = config->get_servers()[servIndex]->getLocdir();
    int locIndex = config->get_servers()[servIndex]->get_locIndex();
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

int Servers::find_in_index_vectors_this_string(std::string left_part_of_filePath, std::vector<std::string> cur_loc_index)
{
    for(size_t i = 0; i < cur_loc_index.size(); ++i)
    {
        // std::cout << "cur_loc_index i-n  = " <<cur_loc_index[i]<< std::endl;
        if (cur_loc_index[i] == left_part_of_filePath)
            return i;
    }
    return -1;
}


std::string Servers::get_need_string_that_we_must_be_pass_send_system_call(std::string filePath)
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
