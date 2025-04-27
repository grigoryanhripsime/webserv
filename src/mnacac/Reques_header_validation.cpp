#include "Reques_header_validation.hpp"

Reques_header_validation::Reques_header_validation(std::vector<ServerDirective *> servers)
{
    this->servers = servers;
}

std::string Reques_header_validation::get_uri() const { return uri; }

int Reques_header_validation::get_servIndex() const { return servIndex; }

std::string Reques_header_validation::get_method() const { return method; }

void Reques_header_validation::get_validation(std::vector<std::string> lines)
{
    (void) lines;
    
}

void Reques_header_validation::post_validation(std::vector<std::string> lines)
{
    (void) lines;
    
}

void Reques_header_validation::delete_validation(std::vector<std::string> lines)
{
    (void) lines;
    
}


std::string    Reques_header_validation::if_received_request_valid(char *c_buffer)
{
    servIndex = getServerThatWeConnectTo(c_buffer);//e si unenq vor serverna
    std::cout<<"SERVINDEX: "<<servIndex<<std::endl;

    std::stringstream ss(c_buffer);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ss, line))
        lines.push_back(line);
        
    std::string method = validation_of_the_first_line(lines[0]);

    if (method == "GET")
        get_validation(lines);
    else if (method == "POST")
        post_validation(lines);
    else if (method == "DELETE")
        delete_validation(lines);
    else 
    {
        throw std::runtime_error("senc method chunenq mer allow_methods-um-> 405 Method Not Allowed.\n");//return 77;
        // std::string filePath = config->get_servers()[servIndex]->getRoot() + "/web/error405.html";
        // std::string res = get_need_string_that_we_must_be_pass_send_system_call(filePath);
        // const char *response = res.c_str();
        // send(client_fd, response, strlen(response), 0);
        // //en fileparhy= i erku toxyba + send-@
    }
    return method;
}


std::string    Reques_header_validation::validation_of_the_first_line(std::string line)
{
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string word;

    while (iss >> word)
        result.push_back(word);
    if (result.size() < 3)
        throw std::runtime_error("error page piti bacvi, headeri error a");
    uri = result[1];
    if (uri == "/favicon.ico")
        return "";
    int locIndex = have_this_uri_in_our_current_server(servIndex);//esi arajin toxi uri masi pahna
    servers[servIndex]->setLocIndex(locIndex);//set locIndex
    if (locIndex < 0)
    {
        std::cout << "yavni bacaskaanaaaaaa\n";
        throw std::runtime_error("error page pti bacvi browser-um");//es hmi exception em qcum vor segfault chta,bayc heto pti zut error page-@ bacenq
    }
    //TODO: add check for server's index
    //////////////////////////////

    if (result[2] != "HTTP/1.1" && result[2] != "HTTP/2")
        throw std::runtime_error("error page:: headery     sxal a");
    
    ///////////////////////////////
    if (check_this_metdod_has_in_appropriate_server(result[0], locIndex) < 0)
        std::runtime_error("error page: header");

    ////////////////////////////////////////
    return result[0];
}

int Reques_header_validation::have_this_uri_in_our_current_server(int servIndex)
{
    std::cout << "servIndex->" << servIndex << std::endl;
    std::vector<LocationDirective*> vec_locations = servers[servIndex]->getLocdir();
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
    std::cout << "vitoge which_locastin = " << which_location << std::endl;
    if (which_location == -1)
    {
        std::cout << "CHfind location path\n\n\n";
        return -1;
    }
    return which_location;
}

int Reques_header_validation::check_this_metdod_has_in_appropriate_server(std::string method, int which_location)
{
    // this->method1 = method;
    // // std::cout<<"☔️☔️☔️☔️  "<<method<<std::endl;
    LocationDirective* locdir;
    std::cout << "hasav stxe\n";
    std::cout << "which_location = " << which_location << std::endl;
    locdir = servers[servIndex]->getLocdir()[which_location];
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

int Reques_header_validation::getServerThatWeConnectTo(std::string buffer)
{
    std::stringstream ss(buffer);
    std::string line;
    while (std::getline(ss, line) && line.find("Host: ") == std::string::npos)
        continue;
    
    std::string serverName = line.substr(6);//karevor atributa, requesti meji Host: -i dimaci grvacna(minchev porty(:8080))
    serverName = serverName.substr(0, serverName.find(":"));
    std::cout<<"⛵️⛵️⛵️⛵️"<<serverName<<std::endl;

    if (servers.size() == 0)
        std::cout<<"blbl\n";

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

