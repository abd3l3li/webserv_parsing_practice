#include "Parser.hpp"
#include <set>

void Parser::parseListen(ServerConfig& server) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for listen directive");

    size_t colon = val.text.find(':');
    if (colon == std::string::npos)
        throw std::runtime_error("Invalid listen format. Use IP:PORT");

    HostPort hp;
    hp.listen_host = val.text.substr(0, colon);
    hp.listen_port = std::atoi(val.text.substr(colon + 1).c_str());

    if (hp.listen_port < 1 || hp.listen_port > 65535)  // 65535 is max port number (2^16 - 1)
        throw std::runtime_error("Port must be between 1 and 65535");

    if (hp.listen_host.empty() || hp.listen_port == 0)
        throw std::runtime_error("Listen host and port cannot be empty");

    server.listens.push_back(hp);
    
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after listen");
}

void Parser::parseServerName(ServerConfig& server) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for server_name directive");

    if (val.text.empty())
        throw std::runtime_error("Server name cannot be empty");

    server.server_name.push_back(val.text);
    
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

    bool root_seen = false;
    bool index_seen = false;
    bool autoindex_seen = false;
    bool methods_seen = false;
    
    while (peek().type != BRACE_CLOSE && peek().type != END_OF_FILE) {
        Token lkey = get();
        
        if (lkey.text == "root") {
            if (root_seen)
                throw std::runtime_error("Duplicate root directive in location block");
            root_seen = true;
            parseLocationRoot(loc);
        } else if (lkey.text == "index") {
            if (index_seen)
                throw std::runtime_error("Duplicate index directive in location block");
            index_seen = true;
            parseLocationIndex(loc);
        } else if (lkey.text == "autoindex") {
            if (autoindex_seen)
                throw std::runtime_error("Duplicate autoindex directive in location block");
            autoindex_seen = true;
            parseLocationAutoindex(loc);
        } else if (lkey.text == "methods") {
            if (methods_seen)
                throw std::runtime_error("Duplicate methods directive in location block");
            methods_seen = true;
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
    if (val.text.empty())
        throw std::runtime_error("Root path cannot be empty");
    if (val.text[0] != '/')
        throw std::runtime_error("Root path must start with '/'");
    
    loc.root = val.text;
    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after root");
}

void Parser::parseLocationIndex(LocationConfig& loc) {
    Token val = get();
    if (val.type != VALUE)
        throw std::runtime_error("Expected value for index directive");
    if (val.text.empty())
        throw std::runtime_error("Index cannot be empty");
    if (val.text.find('/') != std::string::npos)
        throw std::runtime_error("Index cannot contain '/' character");
    if (val.text.find('.') == std::string::npos)
        throw std::runtime_error("Index must have an extension (e.g. index.html)");
    
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

    std::set<std::string> methodDup_set;

    while (peek().type == VALUE) {
        Token method = get();
        if (method.text != "GET" && method.text != "POST" && method.text != "DELETE")
            throw std::runtime_error("Invalid method: " + method.text);
            
        if (!methodDup_set.insert(method.text).second)
            throw std::runtime_error("Duplicate method: " + method.text);
        loc.methods.push_back(method.text);
    }
    if (loc.methods.empty())
        throw std::runtime_error("At least one method must be specified");

    if (get().type != SEMICOLON)
        throw std::runtime_error("Expected ';' after methods");
}
