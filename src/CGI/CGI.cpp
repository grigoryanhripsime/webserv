#include "CGI.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

CGI::CGI(Request * const request) : request(request) {}

CGI::~CGI() {}
// TODO: change to real parameters
std::string CGI::CGI_handler()
{
    // this->request = request;         /* In place of this need class/struct/strings for method, content length and type, query string, body and headers */
    int serv_index = request->get_servIndex();
    std::vector<ServerDirective *> servers = request->get_servers();
    ServerDirective * server = servers[serv_index];
    if (server == NULL) {
        return "";
    }
    std::vector<LocationDirective *> locdir = server->getLocdir();
    int locIndex = server->get_locIndex();
    this->interpreter = locdir[locIndex]->getCgi_path(); /* example: "/usr/bin/php-cgi", maybe get all vector of paths and dynamicly set it */
    char buff[1024] = {0};
    getcwd(buff, 1024);
    this->script_path = std::string(buff) + locdir[locIndex]->getPath() + '/' + locdir[locIndex]->getIndex()[0]; /* example: "/path/to/script.php" */
    
    this->script_name = std::string(buff) + locdir[locIndex]->getPath() + '/' + locdir[locIndex]->getIndex()[0]; /* example: "/cgi-bin/script.php" */
    this->path_info = request->get_uri();     /* example: "" */
    this->server_name = server->getServer_name(); /* example: "example.com" */
    this->server_port = server->getListen().second; /* example: "80" */
    this->remote_addr = server->getListen().first; /* example: "127.0.0.1" */
    this->output.clear();            /* this string may go to some getter for error/exception message */
    
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
    #if 1 // TODO: get real info
    env.push_back("REDIRECT_STATUS=200");
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("REQUEST_METHOD=" + request->get_method());
    env.push_back("SCRIPT_NAME=" + script_name);
    env.push_back("SCRIPT_FILENAME=" + script_name); // Why?
	std::ostringstream oss;
    oss << request->get_content_length();
    env.push_back("CONTENT_LENGTH=" + oss.str());
    env.push_back("CONTENT_TYPE=" + request->get_content_type());
    env.push_back("PATH_INFO=" + path_info); // URI
    env.push_back("QUERY_STRING=" + path_info + request->get_query()); // URI+QUERY
    env.push_back("SERVER_NAME=" + server_name);
    env.push_back("SERVER_PORT=" + server_port);
    env.push_back("REMOTE_ADDR=" + remote_addr);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=Weebserv/1.0");
    #endif
    // for (std::map<std::string, std::string>::const_iterator it = request.headers.begin();
    //      it != request.headers.end(); ++it) {
    //     std::string header_name = it->first;
    //     std::string cgi_name = "HTTP_";
    //     for (size_t i = 0; i < header_name.size(); ++i) {
    //         char c = header_name[i];
    //         cgi_name += (c == '-' ? '_' : std::toupper(c));
    //     }
    //     env.push_back(cgi_name + "=" + it->second);
    // }
}

void CGI::CGI_exec() {
    int stdin_pipe[2];
    int stdout_pipe[2];
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
    if (pid == 0) { // Child process
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        dup2(stdin_pipe[0], 0);
        dup2(stdout_pipe[1], 1);
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);

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

        std::string error = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to execute script";
        write(1, error.c_str(), error.size());
        _exit(1);
    } else { // Parent process
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

        // Parse the script output to form a proper HTTP response
        std::string status_line = "200 OK";
        std::map<std::string, std::string> headers;
        std::string body;
        size_t header_end = output.find("\r\n\r\n");
        if (header_end == std::string::npos) {
            header_end = output.find("\n\n");
            if (header_end == std::string::npos) {
                output.clear(); // Signal error
                std::cout << "FAILD SCRIPT OUTPUT\n\n";

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
            output.clear(); // Signal error
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
// // Example usage (replace with your server's integration logic)
// int main() {
//     Request req;
//     req.method = "POST";
//     req.query_string = "id=123";
//     req.content_type = "application/x-www-form-urlencoded";
//     req.content_length = 7;
//     req.body = "foo=bar";
//     req.headers["User-Agent"] = "CustomClient/1.0";
//     handle_cgi(req, "/usr/bin/php-cgi", "/path/to/script.php", "/cgi-bin/script.php", "",
//                "example.com", "80", "127.0.0.1");
//     return 0;
// }