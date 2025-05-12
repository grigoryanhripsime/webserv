#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstring>

CGI::CGI(Request * const request) : request(request) {}

std::string CGI::_get_index(const std::vector<std::string> &index, const std::string& path)
{
    for (std::size_t i = 0; i != index.size(); ++i) {
        if (path_info == path + '/' + index[i]) {
            return index[i];
        }
        
    }
    request->set_error_page_num(502);
    throw std::runtime_error("Invalid index, no such file");
}

static const std::string _get_extension(const std::string& str)
{
    std::string ext = str.substr(str.find_last_of('.', str.length()));
    return ext;
}

CGI::~CGI() {}

std::string CGI::CGI_handler()
{
    root = request->get_cwd();
    int serv_index = request->get_servIndex();
    std::vector<ServerDirective *> servers = request->get_servers();
    ServerDirective * server = servers[serv_index];
    this->path_info = request->geturi();
    std::vector<LocationDirective *> locdir = server->getLocdir();
    int locIndex = server->get_locIndex();
    std::string index = _get_index(locdir[locIndex]->getIndex(), locdir[locIndex]->getPath());
    std::string ext = _get_extension(index);
    this->interpreter = locdir[locIndex]->getCgi_path(ext);
    this->script_path = root + locdir[locIndex]->getPath() + '/' + index;
    this->script_name = locdir[locIndex]->getPath() + '/' + index;
    std::ostringstream oss;
    oss << server->getListen().second;
    this->server_name = server->getServer_name();
    this->server_port = oss.str();
    this->remote_addr = server->getListen().first;
    this->output.clear();
    
    CGI_parse();

    CGI_exec();

    if (output.empty()) {
        CGI_err("HTTP/1.1 500 Internal Server Error\r\n", "Content-Type: text/plain\r\n", "CGI script failed");
    } else {
        Logger::printStatus("INFO", "CGI has completed it's mission successfully!");
    }
    return output;
}

void CGI::CGI_parse() {
    env.clear();
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("REQUEST_METHOD=" + request->get_method());
    env.push_back("SCRIPT_NAME=" + script_name);
    env.push_back("SCRIPT_FILENAME=" + root + script_name);
    std::ostringstream oss;
    std::string content_type = request->get_content_type();
    oss << (content_type == "POST" ? request->get_content_length() : 0);
    env.push_back("CONTENT_LENGTH=" + oss.str());
    env.push_back("CONTENT_TYPE=" + content_type);
    env.push_back("PATH_INFO=" + path_info);
    env.push_back("QUERY_STRING=" + request->get_query());
    env.push_back("REQUEST_URI=" + request->get_uri() + request->get_query());
    env.push_back("SERVER_NAME=" + server_name);
    env.push_back("SERVER_PORT=" + server_port);
    env.push_back("REMOTE_ADDR=" + remote_addr);
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("SERVER_SOFTWARE=Weebserv/1.0");
    
    headers_map headers = request->get_headers();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it) {
        std::string header_name = it->first;
        std::string cgi_name = "HTTP_";
        for (size_t i = 0; i < header_name.size(); ++i) {
            char c = header_name[i];
            cgi_name += (c == '-' ? '_' : std::toupper(c));
        }
        env.push_back(cgi_name + "=" + it->second);
    }
}

extern "C" void handle_timeout(int)
{
    const char* error = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\nScript timed out";
    write(1, error, strlen(error));
    _exit(1);
}

void CGI::CGI_exec() {
    int stdin_pipe[2];
    int stdout_pipe[2];
    unsigned int timeout = 1; // 2-second timeout

    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        std::clog << "Error: pipe failed: " << strerror(errno) << std::endl;
        return;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        std::clog << "Error: fork failed: " << strerror(errno) << std::endl;
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        return;
    }
    if (pid == 0) {
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        dup2(stdin_pipe[0], 0);
        dup2(stdout_pipe[1], 1);
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        
        signal(SIGALRM, handle_timeout);
        alarm(timeout);

        std::vector<char*> envp;
        for (size_t i = 0; i < env.size(); ++i) {
            envp.push_back(const_cast<char*>(env[i].c_str()));
        }
        envp.push_back(0);

        char* argv[3];
        if (!interpreter.empty()) {
            argv[0] = const_cast<char*>(interpreter.c_str());
            argv[1] = const_cast<char*>(script_path.c_str());
            argv[2] = 0;
            execve(interpreter.c_str(), argv, envp.data());
        } else {
            argv[0] = const_cast<char*>(script_path.c_str());
            argv[1] = 0;
            execve(script_path.c_str(), argv, envp.data());
        }

        const char* error = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to execute script";
        write(1, error, strlen(error));
        _exit(1);
    } else { 
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);

        if (request->get_method() == "POST" && request->get_content_length() > 0) {
            write(stdin_pipe[1], request->get_body().c_str(), request->get_body().size());
        }
        close(stdin_pipe[1]);

        std::string buffer;
        char temp[1024];
        bool timed_out = false;
        while (true) {
            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(stdout_pipe[0], &read_fds);
            struct timeval tv;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            int ret = select(stdout_pipe[0] + 1, &read_fds, NULL, NULL, &tv);
            if (ret > 0 && FD_ISSET(stdout_pipe[0], &read_fds)) {
                ssize_t bytes_read = read(stdout_pipe[0], temp, sizeof(temp) - 1);
                if (bytes_read > 0) {
                    temp[bytes_read] = '\0';
                    buffer += temp;
                } else if (bytes_read == 0) {
                    break;
                } else {
                    std::clog << "Error: read failed: " << strerror(errno) << std::endl;
                    break;
                }
            } else if (ret == 0) {
                std::clog << "Error: CGI script timed out" << std::endl;
                kill(pid, SIGTERM);
                timed_out = true;
                break;
            } else {
                std::clog << "Error: select failed: " << strerror(errno) << std::endl;
                kill(pid, SIGTERM);
                break;
            }
        }
        close(stdout_pipe[0]);

        int status;
        waitpid(pid, &status, 0);

        if (timed_out) {
            return CGI_err("HTTP/1.1 504 Gateway Timeout\r\n", "Content-Type: text/plain\r\n", "CGI script timed out");
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            std::clog << "CGI script exited with status: " << WEXITSTATUS(status) << std::endl;
            return CGI_err("HTTP/1.1 500 Internal Server Error\r\n", "Content-Type: text/plain\r\n", "CGI script failed");
        }

        output = buffer;
        size_t header_end = output.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            header_end = output.find("\n\n");
            if (header_end == std::string::npos) {
                output.clear();
                return CGI_err("HTTP/1.1 500 Internal Server Error\r\n", "Content-Type: text/plain\r\n", "CGI script failed");
            } else {
                header_end += 2;
            }
        } else {
            header_end += 4;
        }

        std::string header_str = output.substr(0, header_end - (output[header_end-2] == '\r' ? 4 : 2));
        std::string body = output.substr(header_end);

        std::istringstream header_stream(header_str);
        std::string line;
        std::string status_line = "200 OK";
        std::map<std::string, std::string> headers;
        bool content_length = false;
        while (std::getline(header_stream, line)) {
            size_t colon = line.find(": ");
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 2);
                if (key == "Status") {
                    status_line = value;
                } else {
                    if (content_length == false && key == "Content-Length") {
                        content_length = true;
                    }
                    headers[key] = value;
                }
            }
        }
        if (content_length == false) {
            std::stringstream ss;
            ss << body.length();
            headers["Content-Length"] = ss.str();
        }

        if (headers.find("Content-Type") == headers.end() && !body.empty()) {
            output.clear();
            return CGI_err("HTTP/1.1 500 Internal Server Error\r\n", "Content-Type: text/plain\r\n", "CGI script failed");
        }

        std::ostringstream response;
        response << "HTTP/1.1 " << status_line << "\r\n";
        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            response << it->first << ": " << it->second << "\r\n";
        }
        response << "\r\n" << body;
        output = response.str();
    }
}

void CGI::CGI_err(const std::string& error, const std::string& cont_type, const std::string& body) {
    std::ostringstream response;
    response << "Content-Length: " << body.length() << "\r\n\r\n";
    output = error + cont_type + response.str() + body;
}