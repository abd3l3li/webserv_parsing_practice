#include <iostream>
#include <vector>
#include "Parser.hpp"

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

int main()
{
    try 
    {
        Tokenizer tokenizer("config.conf");
        std::vector<Token> tokens = tokenizer.tokenize();
        Parser parser(tokens);
        Config config = parser.parse();

        std::cout << config.servers[0].listen_host << ":" 
                << config.servers[0].listen_port << std::endl;
        std::cout << "Server Name: " << config.servers[0].server_name << std::endl;
        std::cout << "Max Body Size: " << config.servers[0].max_body_size << " bytes" << std::endl;
 
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
