#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

// Represents one location block (inside a server block)
struct LocationConfig
{
    std::string path;                  // location /this_path
    std::string root;                  // root /some/dir
    std::string index;                 // index.html
    std::vector<std::string> methods; // GET, POST, DELETE
    bool autoindex;                   // on or off
    std::string upload_dir;           // where to store uploaded files
    std::string redirection;          // optional: redirect to another URL
    std::string cgi_extension;        // e.g. ".php", ".py"

    LocationConfig() : autoindex(false) {}
};

// Represents one server block
struct ServerConfig
{
    std::string listen_host;          // IP address, default: 0.0.0.0
    int listen_port;                  // Port, default: 80
    std::string server_name;          // e.g. "localhost"
    size_t max_body_size;             // limit request body (e.g. 1MB)
    std::map<int, std::string> error_pages; // 404 => "/404.html"
    std::vector<LocationConfig> locations;  // List of locations

    ServerConfig() : listen_host("0.0.0.0"), listen_port(80),
        max_body_size(1024 * 1024) {} //* more info
};

// Holds the full parsed config file
struct Config
{
    std::vector<ServerConfig> servers;
};

