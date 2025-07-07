#include "Parser.hpp"

const ServerConfig& matchServer(const Config& config, const std::string& host, int port);
const LocationConfig& matchLocation(const ServerConfig& server, const std::string& uri);