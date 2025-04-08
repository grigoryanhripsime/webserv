#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#define PORT 8080
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    setNonBlocking(server_fd);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return 1;
    }

    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0) {
        perror("epoll_create");
        return 1;
    }

    epoll_event ev{}, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl");
        return 1;
    }

    std::cout << "Server started on port " << PORT << "\n";

    while (true) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000); // 1 second timeout
        if (n < 0) {
            perror("epoll_wait");
            break;
        }

        if (n == 0) {
            std::cout << "[timeout] No activity\n";
            continue;
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == server_fd) {
                // New client
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                setNonBlocking(client_fd);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                std::cout << "[+] New client connected: fd " << client_fd << "\n";
            } else {
                // Existing client
                char buffer[BUFFER_SIZE];
                memset(buffer, 0, BUFFER_SIZE);
                int bytes = recv(events[i].data.fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes == 0) {
                    std::cout << "[-] Client disconnected: fd " << events[i].data.fd << "\n";
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                } else if (bytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::cout << "[!] No data from client fd " << events[i].data.fd << ", skipping\n";
                    } else {
                        perror("recv");
                        close(events[i].data.fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    }
                } else {
                    std::cout << "[*] Received from fd " << events[i].data.fd << ": " << buffer << "\n";
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
