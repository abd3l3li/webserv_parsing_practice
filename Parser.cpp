#include "Parser.hpp"
// #include <stdexcept>
#include <cstdlib>

Parser::Parser(const std::vector<Token>& tokens)
    : _tokens(tokens), _index(0)
{}

Token Parser::peek()
{
    if (_index < _tokens.size())
        return _tokens[_index];
    return Token(END_OF_FILE, "");
}

Token Parser::get()
{
    if (_index < _tokens.size())
        return _tokens[_index++];
    return Token(END_OF_FILE, "");
}

//PARSING SECTION
// Parses the entire configuration file and returns a Config object

Config Parser::parse() {
    Config config;
    
    while (peek().type != END_OF_FILE) {
        parseServer(config);
    }
    
    return config;
}

void Parser::parseServer(Config& config) {
    Token t = get();
    if (t.type != KEYWORD || t.text != "server")
        throw std::runtime_error("Expected 'server' keyword");
    
    if (get().type != BRACE_OPEN)
        throw std::runtime_error("Expected '{' after server");
    
    ServerConfig server;
    
    while (peek().type != BRACE_CLOSE && peek().type != END_OF_FILE) {
        Token key = get();
        
        if (key.type != KEYWORD)
            throw std::runtime_error("Expected directive inside server block");
        
        if (key.text == "listen") {
            parseListen(server);
        } else if (key.text == "server_name") {
            parseServerName(server);
        } else if (key.text == "location") {
            parseLocation(server);
        } else {
            throw std::runtime_error("Unknown server directive: " + key.text);
        }
    }
    
    if (get().type != BRACE_CLOSE)
        throw std::runtime_error("Expected '}' at end of server block");
    
    config.servers.push_back(server);
}

void Parser::parseListen(ServerConfig& server) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for listen directive");
    size_t colon = val.text.find(':');
    if (colon == std::string::npos)
        throw std::runtime_error("Invalid listen format. Use IP:PORT");
    
    server.listen_host = val.text.substr(0, colon);

    std::string port_str = val.text.substr(colon + 1);
    int port = std::atoi(port_str.c_str());
    
    if (port < 1 || port > 65535)  // 65535 is max port number (2^16 - 1)
        throw std::runtime_error("Port must be between 1 and 65535");
    
    server.listen_port = port;
    
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after listen");
}

void Parser::parseServerName(ServerConfig& server) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for server_name directive");
    server.server_name = val.text;
    
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after server_name");
}

void Parser::parseLocation(ServerConfig& server) {
    LocationConfig loc;
    
    Token path = get();
    if (path.type != VALUE)
        throw std::runtime_error("Expected value for location path");
    loc.path = path.text;
    
    if (get().type != BRACE_OPEN)
        throw std::runtime_error("Expected '{' after location path");
    
    while (peek().type != BRACE_CLOSE && peek().type != END_OF_FILE) {
        Token lkey = get();
        
        if (lkey.text == "root") {
            parseLocationRoot(loc);
        } else if (lkey.text == "index") {
            parseLocationIndex(loc);
        } else if (lkey.text == "autoindex") {
            parseLocationAutoindex(loc);
        } else if (lkey.text == "methods") {
            parseLocationMethods(loc);
        } else {
            throw std::runtime_error("Unknown location directive: " + lkey.text);
        }
    }
    
    if (get().type != BRACE_CLOSE)
        throw std::runtime_error("Expected '}' at end of location block");
    
    server.locations.push_back(loc);
}

void Parser::parseLocationRoot(LocationConfig& loc) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for root directive");
    loc.root = val.text;
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after root");
}

void Parser::parseLocationIndex(LocationConfig& loc) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for index directive");
    loc.index = val.text;
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after index");
}

void Parser::parseLocationAutoindex(LocationConfig& loc) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for autoindex directive");
    if (val.text == "on") {
        loc.autoindex = true;
    } else if (val.text == "off") {
        loc.autoindex = false;
    } else {
        throw std::runtime_error("autoindex must be 'on' or 'off'");
    }
    
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after autoindex");
}

void Parser::parseLocationMethods(LocationConfig& loc) {

    while (peek().type == VALUE) {
        Token method = get();
        if (method.text != "GET" && method.text != "POST" && method.text != "DELETE")
            throw std::runtime_error("Invalid method: " + method.text);
        
        loc.methods.push_back(method.text);
    }
    if (loc.methods.empty())
        throw std::runtime_error("At least one method must be specified");
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after methods");
}