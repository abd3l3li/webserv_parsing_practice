#pragma once

#include <vector>
#include <string>
#include "Tokenizer.hpp"
#include "Config.hpp"

class Parser
{
public:
    Parser(const std::vector<Token>& tokens);
    Config parse();

private:
    std::vector<Token> _tokens;
    size_t _index;

    Token get();
    Token peek();
};
