#include "Server.hpp"

Server::Server(DirectiveConfig &dirConf)
{
    std::cout << "Server ctor is called\n";
    config = &dirConf;
    std::map<std::pair<std::string, int>, std::vector<int> > unique_listens = config->get_unique_listens();
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it  = unique_listens.begin();

    std::cout<<"🦔🦔🦔 "<<unique_listens.size()<<std::endl;
    for(; it != unique_listens.end(); ++it)
    {
        ServerSocket sock = ServerSocket(AF_INET, SOCK_STREAM, 0, (it)->first.second, (it)->first.first, 10);
        // servSock.push_back(sock);

        for (size_t i = 0; i < it->second.size(); i++)
        {
            std::cout<<"🦝🦝🦝  "<<i<<std::endl;
            config->get_servers()[it->second[i]]->setServSock(sock);
        }
    }



    //ensuring that everything is right
    for (size_t k = 0; k < config->get_servers().size(); k++)
    {
        std::cout<<"☀️☀️☀️   "<<config->get_servers()[k]->getServSock().get_socket()<<std::endl;
    }


    setupEpoll();
    runLoop();
}

void Server::setupEpoll() {
    epfd = epoll_create(1);//создаёт новый epoll instance (дескриптор). Он нужен, чтобы отслеживать события на сокетах (например, кто-то подключился).
    if (epfd == -1)
        throw std::runtime_error("epoll_create failed");
    std::cout << "epfd = " << epfd << std::endl;
    // Регистрируем каждый server socket в epoll
    for (size_t i = 0; i < servSock.size(); ++i) {
        int fd = servSock[i].get_socket(); // Получаем сокет server_fd
        std::cout << "fd = " << fd  << std::endl;
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

void Server::runLoop()
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
                    handleClientRequest(sockfd);
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

void Server::acceptClient(int server_fd)
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


int Server::getServerThatWeConnectTo(std::string buffer)
{
    std::stringstream ss(buffer);
    std::string line;
    while (std::getline(ss, line) && line.find("Host: ") == std::string::npos)
        continue;
    std::cout<<"yoohooooo        "<<line<<std::endl; 
    
    std::string serverName = line.substr(6);
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
        // if (servers[i]->getServer_name() == serverName)
        if (serverName == servers[i]->getListen().first)
        {
            std::cout << "chishta Vrds\n";
            return i;
        }
    }//arandzin for-erov em grel nra hamar vortev`
    //ete unenq nuyn ip-ov ev port-ov serverner bayc tvelenq server_name apa zaprosty etalua et name-ov serverin
    //bayc ete unenq nuyn ip-ov ev port-ov serverner bayc chenq tvel server_name apa zaprosty etalua arajin et ip-ov u prot-ov serverin
    std::cout << "en vat depqna\n";
    return 0;
}

void Server::handleClientRequest(int client_fd) {
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

    int servIndex = getServerThatWeConnectTo(buffer);

    location = get_location(buffer);
    std::cout << "vatara->" << location << std::endl;
    if (have_this_location_in_our_current_server(servIndex) < 0)
        std::cout << "error page pti bacvi browser-um" << std::endl;
    ////////////////////////////////////
    // std::cout<<config->get_servers()[0]->getLocdir()[0]->getIndex()[1].c_str()<<std::endl;
    
    std::string filePath = config->get_servers()[0]->getRoot() + config->get_servers()[0]->getLocdir()[0]->getPath() + "/" + config->get_servers()[0]->getLocdir()[0]->getIndex()[1];
    
    // std::cout<<filePath<<std::endl;

    std::ifstream file(filePath.c_str());
    if (!file)
        std::cerr << "Failed to open file" << std::endl;
    
    std::stringstream ss;
    ss << file.rdbuf(); // read entire content
    
    
    // После обработки можешь отправить ответ:

    std::string header = "HTTP/1.1 200 OK\r\nContent-Length: ";
    
    std::string whiteSpaces = "\r\n\r\n";
    
    std::stringstream ss1;
    ss1 << ss.str().size();

    std::string res = header + ss1.str() + whiteSpaces + ss.str();
    std::cout<<"-----------------------------------------\n";
    std::cout<<res<<std::endl;
    std::cout<<"-----------------------------------------\n";
    const char *response = res.c_str();
    send(client_fd, response, strlen(response), 0);
}

std::string Server::get_location(char *buffer)
{
    std::string strbuffer(buffer);
    size_t pos = strbuffer.find('\n');
    std::string firstLineInBuffer = strbuffer.substr(0, pos - 1);
    std::cout << "harcum-> " << firstLineInBuffer << std::endl;
    // std::string location = get_location(firstLineInBuffer);
    size_t pos_slash = firstLineInBuffer.find(' ');
    firstLineInBuffer.erase(0,pos_slash + 1);
    size_t pos_loc_end = firstLineInBuffer.find(' ');
    std::string location = firstLineInBuffer.substr(0, pos_loc_end);
    return location;
}

int Server::have_this_location_in_our_current_server(int serverInd)
{
    std::cout << "serverInd->" << serverInd << std::endl;
    std::vector<LocationDirective*> vec_locations = config->get_servers()[serverInd]->getLocdir();
    std::vector<LocationDirective*>::iterator it  = vec_locations.begin();

    for(; it != vec_locations.end(); ++it)
    {
        std::cout << "----" << (*it)->getPath()<< std::endl;
        if (location == (*it)->getPath())
        {
            std::cout << "molodecccc\n\n\n\n";
            return 1;
        }
    }
    std::cout << "stexic\n\n\n";
    return -1;
}


Server::~Server(){}



// Серверные сокеты добавляются в epoll instance, а events[MAX_EVENTS] — это просто буфер, куда epoll запишет активные события (серверные или клиентские сокеты, на которых что-то произошло).
// a soket aktivniy ili net mi uznayem spomoshyu epoll_wait?
// i vitoge v events-e dobavlyayutsya tolko aktivnie soketi(a epoll_wait na instance epollaepfd,tuda smotirt fse soketi i vibirayet iz nix tolko aktivnie soketi i dobavlyayet na events)?
// Когда ты делаешь:epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
// Ты добавляешь сокет fd в epoll instance (внутреннюю таблицу ядра, связанную с epfd).
// Эти сокеты не попадают в events[MAX_EVENTS], они просто хранятся внутри ядра, привязанные к epfd.
// epoll_wait ждёт, пока на одном или нескольких из добавленных сокетов (через epoll_ctl) не произойдёт нужное событие (например, EPOLLIN — данные для чтения).
// ➡️ Когда такое событие произошло — оно записывается в events[i].
// Что находится в events[i]?
// Это только активные сокеты, на которых произошло событие.

// В epfd хранятся ВСЕ сокеты, добавленные через epoll_ctl.

// В events[] — ТОЛЬКО те, на которых произошло событие.

// events[i] — возвращает только активные сокеты, где произошло событие (например, клиент подключился или прислал данные)

// epoll_wait смотрит на ВСЕ сокеты, которые были добавлены через epoll_ctl,
// и добавляет в events[] только те, на которых произошло событие.
