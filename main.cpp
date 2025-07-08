#include <iostream>
#include <vector>
#include "Parser.hpp"
#include "Router.hpp"

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
            RoutingResult result = requestedRout(config, "localhost", 8080, "/docs/index.html");
            std::cout << "Matched Server: " << result.server->server_name[0] << std::endl;
            std::cout << "Matched Location: " << result.location->path << std::endl;
            if (result.is_redirect)
            {
                std::cout << "Redirect URL: " << result.redirect_url << std::endl;
            }
            else
            {
                std::cout << "File Path: " << result.file_path << std::endl;
            }
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
