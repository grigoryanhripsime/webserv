#include "Servers.hpp"

Servers::Servers(DirectiveConfig &dirConf)
{
    // std::cout << "Server ctor is called\n";
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
    for(; it != unique_listens.end(); ++it)
    {
        ServerSocket sock = ServerSocket(AF_INET, SOCK_STREAM, 0, (it)->first.second, (it)->first.first, 10);
        for (size_t i = 0; i < it->second.size(); i++)
            servSock[it->second[i]] = sock;
    }
    Logger::printStatus("INFO", "Initalizing server sockets with values :)");
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
    std::stringstream ss;
    ss << "Current epoll fd is "<<epfd;
    Logger::printStatus("INFO", ss.str());
    // Регистрируем каждый server socket в epoll
    std::map<std::pair<std::string, int>, std::vector<int> > unique_listens = config->get_unique_listens();
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it = unique_listens.begin();
    for (; it != unique_listens.end(); ++it)
    {
        int fd = servSock[it->second[0]].get_socket();// Получаем сокет server_fd
        struct epoll_event ev;// Создаём epoll_event, чтобы указать, какие события мы хотим слушать.
        ev.events = EPOLLIN;// мы хотим знать, когда на сокете появятся входящие данные (например, клиент хочет подключиться).
        ev.data.fd = fd;// сохраняем сам сокет, чтобы потом понять, на каком сокете произошло событие.

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        {//Регистрируем серверный сокет в epoll, чтобы он следил за событиями на нём.
            throw std::runtime_error("epoll_ctl ADD failed");
        }
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
                        Request request(config->get_servers());
                        request.handleClientRequest(sockfd);
                        requests.push_back(request);
                    }
                    catch(std::exception& e)
                    {
                        std::cout << "EXception: " << e.what() << std::endl; //TODO: maybe just open an error page or remove catcho, so if the server isnot valid programm stoped
                    }
                }
            }
            else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP)
            {
                epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                close(sockfd);
                std::ostringstream oss;
                oss << sockfd;
                throw std::runtime_error("Error or hangup detected on socket " + oss.str());
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
        close(client_fd); // TODO: maybe we could throw an exception?
    } else {
        std::stringstream ss;
        ss <<"Accepted new client connection: fd "<<client_fd;
        Logger::printStatus("INFO", ss.str());
    }
}

Servers::~Servers()
{
    std::cout << "Servers dtor is called\n";
}
