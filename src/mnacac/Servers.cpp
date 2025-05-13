#include "Servers.hpp"

Servers::Servers(DirectiveConfig &dirConf)
{
    config = &dirConf;
    connectingServerToSocket();

    runningProcess();

}

void Servers::connectingServerToSocket()
{
    std::vector<ServerSocket> helper(config->get_servers().size(), ServerSocket());
    servSock = helper;

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
    epfd = epoll_create(1);
    if (epfd == -1)
        throw std::runtime_error("epoll_create failed");
    std::stringstream ss;
    ss << "Current epoll fd is "<<epfd;
    Logger::printStatus("INFO", ss.str());

    std::map<std::pair<std::string, int>, std::vector<int> > unique_listens = config->get_unique_listens();
    std::map<std::pair<std::string, int>, std::vector<int> >::iterator it = unique_listens.begin();
    for (; it != unique_listens.end(); ++it)
    {
        int fd = servSock[it->second[0]].get_socket();
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;

        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
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
                        Request request(config->get_servers());
                        request.handleClientRequest(sockfd);
                        requests.push_back(request);
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
    int client_fd = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);
    
    if (client_fd == -1) {
        std::cerr << "Failed to accept client connection" << std::endl;
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        close(client_fd);
    } else {
        std::stringstream ss;
        ss <<"Accepted new client connection: fd "<<client_fd;
        Logger::printStatus("INFO", ss.str());
    }
}

Servers::~Servers()
{
    // std::cout << "Servers dtor is called\n";
}
