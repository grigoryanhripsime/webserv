#include "Request_header_validation.hpp"

Request_header_validation::Request_header_validation(std::vector<ServerDirective *> servers)
{
    this->servers = servers;
    status = STATIC;
}

std::string Request_header_validation::get_uri() const { return uri; }

int Request_header_validation::get_servIndex() const { return servIndex; }

std::string Request_header_validation::get_method() const { return method; }

std::string    Request_header_validation::if_received_request_valid(Request &req, char *c_buffer)
{
    // std::cout << "REQUEST:" << c_buffer<<std::endl;
    std::stringstream ss(c_buffer);
    std::string line;
    std::getline(ss, firstLine);
    std::map<std::string, std::string> pairs;
    while (std::getline(ss, line))
    {
        std::stringstream ss1(line);
        std::string title, value;
        ss1 >> title;

        ss1 >> value;
        pairs.insert(std::pair<std::string, std::string>(title.substr(0, title.size() - 1), value));
        lines.push_back(line);
    }
    if (lines.size() < 1)
    {
        req.set_error_page_num(400);
        throw std::runtime_error("header cant contain less than 2 lines.");
    }
    std::map<std::string, std::string>::iterator it = pairs.find("Host");
    if (it == pairs.end())
    {
        req.set_error_page_num(400);
        throw std::runtime_error("There is no host");
    }
    servIndex = getServerThatWeConnectTo(it->second);
    std::clog<<servIndex<<std::endl;
    req.set_servIndex(servIndex);
    if (servIndex < 0 || static_cast<size_t>(servIndex) >= servers.size()) {
        req.set_error_page_num(400);
        throw std::runtime_error("Invalid server index");
    }
    method = validation_of_the_first_line(req, firstLine);
    if (!method.empty())
        Logger::printStatus("INFO", "Method of the request is: " + method);
    req.set_method(method);
    if (method != "GET" && method != "POST" && method != "DELETE")
    {
        req.set_error_page_num(405);
        throw std::runtime_error("senc method chunenq mer allow_methods-um-> 405 Method Not Allowed.\n");//return 77;
    }

    return method;
}

void Request_header_validation::fill_headers_map(headers_map &headers)
{
    headers.clear();

    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string line = *it;
        std::string::size_type colonPos = line.find(':');

        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            while (!key.empty() && key[0] == ' ') key.erase(0, 1);
            while (!key.empty() && key[key.size() - 1] == ' ') key.erase(key.size() - 1, 1);
            while (!value.empty() && value[0] == ' ') value.erase(0, 1);
            while (!value.empty() && value[value.size() - 1] == ' ') value.erase(value.size() - 1, 1);

            headers[key] = value;
        }
    }
}


std::string    Request_header_validation::validation_of_the_first_line(Request &req, std::string line)
{
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string word;

    while (iss >> word)
        result.push_back(word);
    if (result.size() < 3)
    {
        req.set_error_page_num(400);
        throw std::runtime_error("error page piti bacvi, headeri error a");
    }
    uri = result[1];
    Logger::printStatus("INFO", "URI used to connect to server: " + uri);
    req.set_uri(uri);
    if (uri == "/favicon.ico")
    {
        req.set_error_page_num(404);
        throw std::runtime_error("favicon is missing");
    }
    size_t harcakanInd = uri.find('?');
    if (harcakanInd != std::string::npos)
        req.set_query(uri.substr(harcakanInd + 1));
    if (!(result[0] == "DELETE" && servers[servIndex]->get_locIndex() > 0))
    {
        int locIndex = have_this_uri_in_our_current_server(servIndex);
        if (locIndex < 0)
        {
            req.set_error_page_num(404);
            throw std::runtime_error("error page pti bacvi browser-um");
        }
        servers[servIndex]->setLocIndex(locIndex);
    }
    if (result[2] != "HTTP/1.1" && result[2] != "HTTP/2")
    {
        req.set_error_page_num(505);
        throw std::runtime_error("error page:: headery sxal a");
    }

    if (check_this_metdod_has_in_appropriate_server(result[0], servers[servIndex]->get_locIndex()) < 0)
    {
        req.set_error_page_num(405);
        throw std::runtime_error("this method not allowed in appropriate location");
    }
    return result[0];
}

int Request_header_validation::have_this_uri_in_our_current_server(int servIndex)
{
    std::vector<LocationDirective*> vec_locations = servers[servIndex]->getLocdir();
    int which_location = -1;
    std::string path;
    size_t length = 0;
    if (uri[uri.size() - 1] == '/' && uri.size() != 1)
        uri.erase(uri.size() - 1);
    for(size_t i = 0; i < vec_locations.size(); ++i)
    {
        path = vec_locations[i]->getPath();
        if (path[path.size() - 1] == '/' && path.size() != 1)
            path.erase(path.size() - 1);
        if (path.size() > uri.size())
            continue ;
        size_t tmpLength = 0;
        size_t j;
        for (j = 1; j < uri.size() && path[j] == uri[j]; ++j)
            tmpLength++;
        if (uri.size() < path.size())
            continue ;
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
    if (which_location == -1)
    {
        Logger::printStatus("WARNING", "Oops, the location is not located in this server");
        return -1;
    }
    return which_location;
}

int Request_header_validation::check_this_metdod_has_in_appropriate_server(std::string method, int which_location)
{
    LocationDirective* locdir;
    locdir = servers[servIndex]->getLocdir()[which_location];
    std::vector<std::string> allow_methods = locdir->getAllow_methods();
    for(size_t i = 0; i < allow_methods.size(); ++i)
    {
        if (allow_methods[i] == method)
            return 1;
    }
    return -1;
}

int Request_header_validation::getServerThatWeConnectTo(std::string line)
{
std::clog<<line<<"\n";
    std::string serverName, port;
    serverName = line.substr(0, line.find(":"));
    port = line.substr(line.find(":") + 1);
    std::stringstream ss(port);
    int port_num;
    ss >> port_num;

        std::clog << "num = " << port_num << std::endl;
    Logger::printStatus("INFO", "Specified servername: " + serverName);
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (servers[i]->getServer_name() == serverName && servers[i]->getListen().second == port_num)
            return i;
    }
    for (size_t i = 0; i < servers.size(); i++)
    {
        if (serverName == servers[i]->getListen().first && servers[i]->getListen().second == port_num)
            return i;
    }
    return -1;
}


void    Request_header_validation::status_handler()
{
    std::vector<std::string> uri_share;
    std::istringstream iss(uri);
    std::string token;

    while (std::getline(iss, token, '/')) {
        if (!token.empty()) {
            uri_share.push_back(token);
        }
    }
    if (!uri_share.empty() && uri_share[0] == "cgi-bin")
    {
        std::vector<LocationDirective*> locdir = servers[servIndex]->getLocdir();
        status = DYNAMIC;
        Logger::printStatus("INFO", "The request is DYNAMIC");

    }
    else
    {
        status = STATIC;
        Logger::printStatus("INFO", "The request is STATIC");
    }
 }
