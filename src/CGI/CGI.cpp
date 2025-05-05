#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

CGI::CGI(Request * const request) : request(request) {root = request->get_cwd();}

std::string CGI::_get_index(const std::vector<std::string> &index, const std::string& path)
{
    for (std::size_t i = 0; i != index.size(); ++i) {
        if (path_info == path + '/' + index[i]) {
            return index[i];
        }
    }
    throw std::runtime_error("Location: autoindex must be 'on' or 'off'");
}

static const std::string _get_extension(const std::string& str)
{

    std::string ext = str.substr(str.find_last_of('.', str.length()));
    return ext;
}

CGI::~CGI() {}

std::string CGI::CGI_handler()
{
    int serv_index = request->get_servIndex();
    std::vector<ServerDirective *> servers = request->get_servers();
    ServerDirective * server = servers[serv_index];
    if (server == NULL) {
        return "";
    }
    this->path_info = request->get_uri();
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
        CGI_err();
    }
    std::cout << output << " = OUTPUT\n\n";
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

void handle_timeout(int)
{
    std::cerr << "Execution timed out" << std::endl;
	
    exit(EXIT_FAILURE);
}

void CGI::CGI_exec() {
    int stdin_pipe[2];
    int stdout_pipe[2];
    unsigned int timeout = 0;

    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
        return;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        std::cout << "FAILD FORK\n\n";
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
            std::clog << env[i] << std::endl;
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

        std::string error = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to execute script";
        write(1, error.c_str(), error.size());
        _exit(1);
    } else {
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);

        if (request->get_method() == "POST" && request->get_content_length() > 0) {
            write(stdin_pipe[1], request->get_body().c_str(), request->get_body().size());
        }
        close(stdin_pipe[1]);

        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(stdout_pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            output += buffer;
        }
        close(stdout_pipe[0]);

        int status;
        waitpid(pid, &status, 0);

        std::string status_line = "200 OK";
        std::map<std::string, std::string> headers;
        std::string body;
        size_t header_end = output.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            header_end = output.find("\n\n");
            if (header_end == std::string::npos) {
                output.clear();
                return;
            } else {
                header_end += 2;
            }
        } else {
            header_end += 4;
        }

        std::string header_str = output.substr(0, header_end - (output[header_end-2] == '\r' ? 4 : 2));
        body = output.substr(header_end);

        std::istringstream header_stream(header_str);
        std::string line;
        while (std::getline(header_stream, line)) {
            size_t colon = line.find(": ");
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 2);
                if (key == "Status") {
                    status_line = value;
                } else {
                    headers[key] = value;
                }
            }
        }

        if (headers.find("Content-Type") == headers.end() && !body.empty()) {
            output.clear();
            return;
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

void CGI::CGI_err() {
    std::ostringstream response;
    response << "HTTP/1.1 500 Internal Server Error\r\n";
    response << "Content-Type: text/plain\r\n";
    response << "\r\n";
    response << "Internal Server Error: CGI execution failed";
    output = response.str();
}