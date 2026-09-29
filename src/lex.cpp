#include "../include/lex.hpp"
#include <iostream>


bool isKeyword(const std::string& lexeme){
    std::string keywords[] = {
        "AND",
        "ASC",
        "BY",
        "CREATE",
        "DELETE",
        "DESC",
        "DROP",
        "FROM",
        "INSERT",
        "INT",
        "INTO",
        "KEY",
        "NOT",
        "NULL",
        "OR",
        "ORDER",
        "PRIMARY",
        "SELECT",
        "SET",
        "TABLE",
        "TEXT",
        "UPDATE",
        "VALUES",
        "WHERE"
    };

    return std::find(std::begin(keywords), std::end(keywords), lexeme) != std::end(keywords);
}

std::vector<Token> lex(const std::string& query){
    
    char c;
    Token token;
    std::vector<Token> tokens;
    int pos = 0;

    while(pos < query.length()){
        c = query.at(pos);

        if(c == EOF)
            break;

        else if(isspace(c)){
            pos++;
            continue;
        }

        // ( ) , ; * = <> < > <= >= - '
        else if(c == '(') {
            tokens.push_back({TT_LPAREN, "("}); 
            
        }

        else if(c == ')') {
            tokens.push_back({TT_RPAREN, ")"});
            
            
        }

        else if(c == ',') {
            tokens.push_back({TT_COMMA, ","});
            
            
        }

        else if(c == ';') {
            tokens.push_back({TT_SEMICOLON, ";"});
            
            
        }

        else if(c == '*') {
            tokens.push_back({TT_STAR, "*"});
            
            
        }

        else if(c == '=') {
            tokens.push_back({TT_EQUAL, "="});
            
            
        }

        else if(c == '<') {
            tokens.push_back({TT_LESS, "<"});
            
            
        }

        else if(c == '>') {
            tokens.push_back({TT_GREATER, ">"});
            
            
        }

        else if(c == '\''){
            pos++; // past '

            std::string lexeme;
            size_t start = pos;

            while(pos < query.length()){
                c = query.at(pos);
            
                if(c == '\'')
                    break;

                pos++;
            }
            
            

            lexeme = query.substr(start, pos - start);
            std::cout << "lexeme: " << lexeme << std::endl;

            
            pos++; // past '

            token.lexeme = lexeme;
            token.type = TT_STRING;
            tokens.push_back(token);
            continue;
        }

        else if(isalpha(c)){
            std::string lexeme;

            size_t start = pos;
            
            while(pos < query.length()){
                c = query.at(pos);
            
                if(!isalpha(c) && !isdigit(c) && c != '_')
                    break;

                pos ++;
            }

            lexeme = query.substr(start, pos - start);


            if(isKeyword(lexeme)){
                token.type = TT_KEYWORD;
            }else{
                token.type = TT_IDENT;
            }

            token.lexeme = lexeme;
            tokens.push_back(token);
            continue;
        }

        else if(isdigit(c) || c == '-'){
            std::string lexeme;

            size_t start = pos;

            while(pos < query.length()){
                c = query.at(pos);

                if(!isdigit(c) && c != '-')
                    break;

                pos++;
            }

            lexeme = query.substr(start, pos - start);

            tokens.push_back({TT_INT, lexeme});

            continue;
            
        }


        pos++;
    }

    tokens.push_back({TT_EOF, ""});

    return tokens;

}

std::string token_type_str(TokenType type){
    
    switch(type){
        case TT_KEYWORD: return "KEYWORD";
        case TT_IDENT: return "IDENT";
        case TT_LPAREN: return "LPAREN";
        case TT_RPAREN: return "RPAREN";
        case TT_COMMA: return "COMMA";
        case TT_SEMICOLON: return "SEMICOLON";
        case TT_STAR: return "STAR";
        case TT_EQUAL: return "EQUAL";
        case TT_LESS: return "LESS";
        case TT_GREATER: return "GREATER";
        case TT_INT: return "INT";
        case TT_STRING: return "STRING";
        case TT_EOF: return "EOF";
        default: return "UNKNOWN";
    }

}


std::string token_str(const Token& token){
    
    return "Token{" + token_type_str(token.type) + ", " + token.lexeme + "}";

}