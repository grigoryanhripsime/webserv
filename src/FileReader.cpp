#include "FileReader.hpp"

FileReader::FileReader(const std::string &fileName) : file(fileName.c_str())
{
    if (!file || !file.is_open())
        throw FileReadingException("Coudn't open config file!");
    std::cout<<"FileReader was created!\n";
}

FileReader::~FileReader()
{
    file.close();
    
    std::cout<<"FileReader was destroyed!\n";
}

FileReader::FileReadingException::FileReadingException(std::string err_msg)
{
    err_message = err_msg;
}

FileReader::FileReadingException::~FileReadingException() throw() {}


const char* FileReader::FileReadingException::what() const throw()
{
    return err_message.c_str();
}

void FileReader::fileToStr()
{
    std::string line;

    while (std::getline(file, line))
    {
        // std::cout << "cur line->" << line << std::endl;
        if (line.find_first_of('#') != std::string::npos)
        {
            // std::cout << "#kar->" << line << std::endl;
            line.erase(line.find_first_of('#'));//esi en pahna vor commentner karananq anenq?, hastat config fayli mej chi lenlu nenc tox(valid hraman)vor # chi parenakelu?
            // std::cout << "#-@ hanac->" << line << std::endl;
        }

        while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
            line.erase(0, 1);//skzbic ktrel white spacery

        while (!line.empty() && (line[line.length() - 1] == ' ' || line[line.length() - 1] == '\t' || line[line.length() - 1] == '\n')) 
            line.erase(line.length() - 1, 1);//verjic ktrel white spacery
        
        if (line.empty() || line[0] == '#')
            continue;//inch case kara lini????

        if (!(line[line.length() - 1] == ';' || line[line.length() - 1] == '{' || line[line.length() - 1] == '}'))
            throw FileReadingException("there is no valid symbol at the end!");

        fileStr += line;//fileStr i mej celi fayly kpcnum enq mi stroki mej
    }
    std::cout << "sreic sksac->\n";
    std::cout<<fileStr<<std::endl;
    std::cout << "stexel prc\n";
}

void rec(std::stringstream &ss, Directive &directives, FileReader *th)
{
    std::string key;
    while (ss >> key)//esi key-i mej lcnelua minchev space-@?
    {
        std::cout << "tenanq esincha->" << key << std::endl;
        while (std::isspace(ss.peek()))//Он смотрит на следующий символ в потоке , но не двигает курсор.
            ss.get();//get() удаляет символ из потока и передвигает курсор вперёд.

        if (ss.peek() == '{')
        {
            std::cout << "ashsssssssssssssssssssssssssssssssssssssssssssssssssssss\n";
            std::cout<<"key: "<<key<<std::endl;
            ss.get();  // Consume '{'
            Directive *dir = new Directive();
            directives.blocks.insert(std::make_pair(key, dir));

            std::string block;
            if (ss.str().find('}') == std::string::npos || !std::getline(ss, block, '}')) // || block.length() < 2)
                throw std::runtime_error("Error: Invalid block structure");
            int cnt = std::count(block.begin(), block.end(), '{');
            std::cout<<"cnt: "<< cnt<<std::endl;
            while (cnt)
            {
                block += '}';
                std::string tmp;
                if (!std::getline(ss, tmp, '}'))
                    throw std::runtime_error("iran hamapatasxan pakox blocky chkar!!");
                cnt += std::count(tmp.begin(), tmp.end(), '{');
                block += tmp;
                cnt--;

            }
            std::cout<<"block: "<<block<<std::endl;
            std::stringstream ss2(block);
            std::cout << "doooooooooooooooooooooooooooooooooooo\n";
            th->printDirective(*dir, 0);
            rec(ss2, *dir, th);
            std::cout << "recursiaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
            th->printDirective(*dir, 0);
            
        }
        else  // Key-value pair case
        {
            std::string values, value;
            if (!std::getline(ss, values, ';'))//inch case kara lini?
                throw std::runtime_error("Error: Invalid directive syntax");
            std::stringstream ss2(values);
            std::vector<std::string> vals;
            // std::cout<<"key = "<<key<<std::endl;
            while (ss2 >> value)
            {
                std::cout<<"value = "<<value<<std::endl;
                vals.push_back(value);
            }
            directives.values.insert(std::make_pair(key, vals));
        }
    }  
}


void FileReader::printDirective(const Directive &directive, int indent = 0) 
{
    (void)directive;
    (void)indent;
    std::string indentation(indent, ' ');
    
    // Print values
    std::cout << "tpelu ban ka vor?\n";
    for (std::multimap<std::string, std::vector<std::string> >::const_iterator it = directive.values.begin(); it != directive.values.end(); ++it) {
        std::cout << indentation << it->first;
        for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
            std::cout << " " << *vit;
        }
        std::cout << ";\n";
    }
    
    // Print blocks
    for (std::multimap<std::string, Directive *>::const_iterator it = directive.blocks.begin(); it != directive.blocks.end(); ++it) {
        std::cout << indentation << it->first << " {\n";
        printDirective(*(it->second), indent + 4);
        std::cout << indentation << "}\n";
    }
    std::cout << "prc\n";
}



void FileReader::parseConfig()
{
    std::stringstream ss(fileStr);
    rec(ss, directives, this);
    std::cout << "stexiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiic\n";
    printDirective(directives, 0);
}




