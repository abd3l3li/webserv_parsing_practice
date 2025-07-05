#include <iostream>
#include <vector>
#include "Parser.hpp"

// Utility function to print token type as string
std::string type_to_text(TokenType type)
{
    if (type == KEYWORD)      return "KEYWORD";
    if (type == VALUE)        return "VALUE";
    if (type == BRACE_OPEN)   return "BRACE_OPEN";
    if (type == BRACE_CLOSE)  return "BRACE_CLOSE";
    if (type == SEMICOLON)    return "SEMICOLON";
    if (type == END_OF_FILE)  return "END_OF_FILE";
    return "UNKNOWN";
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
    
    try 
    {
        Tokenizer tokenizer(argv[1]);
        std::vector<Token> tokens = tokenizer.tokenize();
        Parser parser(tokens);
        Config config = parser.parse();

        // Access the first server block safely
        if (!config.servers.empty())
        {
            const ServerConfig& server = config.servers[0];

            if (!server.listens.empty())
                std::cout << "Listen: " 
                        << server.listens[1].listen_host << ":" 
                        << server.listens[1].listen_port << std::endl;

            if (server.server_name.size() >= 1)
                std::cout << "Server Name: " 
                        << server.server_name[0] << std::endl;

            std::cout << "methods: " << server.locations[1].methods[0] << std::endl;
            std::cout << "upload_dir: " << server.locations[1].upload_dir << std::endl;
            std::cout << "redirection: " << server.locations[1].redirection << std::endl;
            std::cout << "cgi_extension: " << server.locations[1].cgi_extension << std::endl;
            std::cout << "error_pages: " << server.error_pages.size() << std::endl;
            std::cout << "max_body_size: " << server.max_body_size << std::endl;
        }
        else
        {
            std::cout << "No server blocks found in config." << std::endl;
        }
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
