#include "Parser.hpp"

struct RoutingResult
{
    const ServerConfig* server;
    const LocationConfig* location;
    std::string file_path;
    bool is_redirect;
    std::string redirect_url;
};


const ServerConfig& matchServer(const Config& config, const std::string& host, int port);
const LocationConfig& matchLocation(const ServerConfig& server, const std::string& uri);
std::string finalPath(const LocationConfig& location, const std::string& uri);
RoutingResult routingResult(const Config& config, const std::string& host,
                        int port, const std::string& uri);