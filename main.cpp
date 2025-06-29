#include <iostream>
#include <vector>
#include "Tokenizer.hpp"

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

        for (size_t i = 0; i < tokens.size(); ++i)
        {
            std::cout << "Type: " << type_to_text(tokens[i].type)
                    << " | Text: \"" << tokens[i].text << "\"" << std::endl;
        } 
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
