#pragma once

#include <string>
#include <vector>


enum TokenType{
    TT_KEYWORD,
    TT_IDENT,
    TT_LPAREN,
    TT_RPAREN,
    TT_COMMA,
    TT_SEMICOLON,
    TT_STAR,
    TT_EQUAL,
    TT_LESS,
    TT_GREATER,
    TT_INT,
    TT_STRING,


    TT_EOF,
};

struct Token{
    TokenType type;
    std::string lexeme;

    size_t line = 0;
    size_t col = 0;
};


bool isKeyword(const std::string& lexeme);
std::vector<Token> lex(const std::string& query);
std::string token_type_str(TokenType type);
std::string token_str(const Token& token);