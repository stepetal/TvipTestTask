#include <iostream>
#include <string>
#include <fstream>
#include <regex>

#include "server_http.hpp"

using SnapshotVideoServer = SimpleWeb::SnapshotServer<SimpleWeb::HTTP>;

void printUsage()
{
    std::cout << "Usage: [options]\n\n"
              << "Options:\n"
              << "  -c      : config file name\n"
              << "  -h      : display this help\n";
}




int main(int argc, char** argv)
{
    int option;
    std::string config_file_name{"config.txt"};
    while ((option = getopt(argc, argv, "c:h")) != -1)
    {
        switch (option)
        {
        case 'c':
            config_file_name = optarg;
            break;
        case 'h':
            printUsage();
            return 0;
        case '?':
        default:
            std::cerr << "invalid arguments" << std::endl;
            printUsage();
            return -1;
        }
    }
    try
    {
        std::map<std::string,std::string> config_file_map;
        std::unique_ptr<SnapshotVideoServer> server = std::make_unique<SnapshotVideoServer>();
        std::unique_ptr<std::ifstream> config_file = std::make_unique<std::ifstream>(config_file_name);
        if(config_file->is_open())
        {
            std::string config_file_line;
            std::regex config_file_regex("^(.+):(.+)$");
            while(std::getline(*config_file.get(),config_file_line))
            {
                std::smatch matches;
                if(std::regex_search(config_file_line,matches,config_file_regex))
                {
                    if(matches.size() == 3)
                    {
                        config_file_map.insert({matches[1],matches[2]});
                    }
                    else
                    {
                        throw std::invalid_argument("Invalid config file");
                    }
                }
                else
                {
                    throw std::invalid_argument("Invalid config file");
                }
            }
            std::cout << "Config file: " << std::endl;
            for(const auto& [key,value] : config_file_map)
            {
                std::cout << key << " : " << value << std::endl;
            }

            server->config.port = std::stoi(config_file_map["port"]);
            server->set_video_files_dir(config_file_map["video_files_dir"]);
            server->set_thumbs_dir(config_file_map["thumbnails_dir"]);
            server->set_max_thumb_height(std::stoi(config_file_map["max_thumb_height"]));
            server->set_max_thumb_width(std::stoi(config_file_map["max_thumb_width"]));
            server->set_video_format(config_file_map["video_format"]);
            server->init();

            // Start server and receive assigned port when server is listening for requests
            std::promise<unsigned short> server_port;
            std::thread server_thread([&server, &server_port]() 
            {
                server->start([&server_port](unsigned short port)
                {
                  server_port.set_value(port);
                });
            });
            std::cout << "Server listening on port " << server_port.get_future().get() << std::endl << std::endl;

            server_thread.join();
        }
        else
        {
            std::cout << "Unable to open config.txt: " << std::endl;
        }
    }
    catch(const std::exception &e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
}
