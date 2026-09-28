#include <iostream>

#include "../include/server.hpp"
#include "../include/lex.hpp"
#include "../include/parse.hpp"

struct ParseError : std::runtime_error {
    size_t line, col;
    ParseError(const std::string& msg, const Token& at)
        : std::runtime_error(msg), line(at.line), col(at.col) {}
};

struct Cursor{
    std::vector<Token> tokens;
    size_t pos;
};

Token peek(Cursor& cursor){
    return cursor.tokens[cursor.pos];
}


Token advance(Cursor& cursor){
    const Token& t = cursor.tokens.at(cursor.pos);
    if(t.type != TT_EOF) cursor.pos++;
    return t;
}


Token expect(Cursor& c, TokenType type){
    if(peek(c).type != type) throw ParseError("Expected '" + token_type_str(peek(c).type) + "'", peek(c));
    return advance(c);
}

Token expect(Cursor& c, const std::string str){
    if(peek(c).lexeme != str) throw ParseError("Expected '" + str + "'", peek(c));
    return advance(c);
}

bool match(Cursor& c, TokenType type){
    if(peek(c).type != type) return false;
    advance(c);
    return true;
} 

bool match(Cursor& c, const std::string str){
    if(peek(c).lexeme != str) return false;
    advance(c);
    return true;
}



std::unique_ptr<ColumnDef> parseColumnDef(Cursor& c){
    
    auto ret = std::make_unique<ColumnDef>();

    // get column name
    ret->name = expect(c, TT_IDENT).lexeme;

    // get column type
    std::string type = expect(c, TT_KEYWORD).lexeme;
    if(type == "INT"){
        ret->type = ColumnType::Int;
    }else if(type == "TEXT"){
        ret->type = ColumnType::Text;
    }else{
        throw ParseError("Unsupported column type '" + type + "'", peek(c));
    }

    // test for primary key and not null
    if(match(c, "PRIMARY")){
        expect(c, "KEY");
        ret->primaryKey = true;
    }else if(match(c, "NOT")){
        expect(c, "NULL");
        ret->notNull = true;
    }else{
        throw ParseError("Unsupported constraint '" + peek(c).lexeme + "'", peek(c));
    }

    return ret;

}

std::unique_ptr<CreateStmt> parseCreateStmt(Cursor& c){
    
    // check the necessary
    expect(c, "CREATE");
    expect(c, "TABLE");


    auto ret = std::make_unique<CreateStmt>();
    
    // get table name
    ret->table = expect(c, TT_IDENT).lexeme;

    expect(c, TT_LPAREN);
    
    // parse first column definition
    ret->columns.push_back(parseColumnDef(c));
    
    while(match(c, TT_COMMA)){
        ret->columns.push_back(parseColumnDef(c));
    }

    expect(c, TT_RPAREN);

    return ret;
}


StmtPtr parseStmt(Cursor& cursor){

    Token current = peek(cursor);
    
    if (current.type != TT_KEYWORD)
        throw ParseError("Expected Statement", current);

    if (current.lexeme == "CREATE")     return parseCreateStmt(cursor);
    // if (current.lexeme == "DELETE")     return parseDeleteStmt(cursor);
    // if (current.lexeme == "DROP")       return parseDropStmt(cursor);
    // if (current.lexeme == "INSERT")     return parseInsertStmt(cursor);
    // if (current.lexeme == "UPDATE")     return parseUpdateStmt(cursor);
    
    throw ParseError("Invalid Statement", current);
}

std::vector<StmtPtr> parseProgram(Cursor& cursor){
    std::vector<StmtPtr> statements;



    while(cursor.pos < cursor.tokens.size() && peek(cursor).type != TT_EOF){

        StmtPtr stmt = parseStmt(cursor);
        statements.push_back(std::move(stmt));

        cursor.pos++;
    }

    return statements;
};


int main(){


    // int sock = init_server("127.0.0.1", 8080);

    // while(1){
    //     std::string msg = recieve_str(sock);
    //     if(msg.empty()) break;

    //     std::cout << msg << std::endl;
    // }

    std::string query = "CREATE TABLE users_2 (id INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, age INT, nick TEXT);"

                        "INSERT INTO users_2 (id, name, age, nick) VALUES (1, 'Anna', 30, NULL);"
                        "INSERT INTO users_2 VALUES (2, 'Bob', -5, 'bobby');"
                         
                        "SELECT * FROM users_2;"
                        "SELECT id, name FROM users_2"
                        "  WHERE (age >= 18 AND age <= 65) OR NOT name <> 'Anna'"
                        "  ORDER BY age DESC;"
                        "SELECT name FROM users_2 WHERE id = 1 OR age < 0 AND age > -10 ORDER BY name ASC;"
                        "SELECT name FROM users_2 ORDER BY id;"
                        ""
                        "UPDATE users_2 SET age = 31, nick = name WHERE id = 1;"
                        "UPDATE users_2 SET nick = NULL;"
                        "" 
                        "DELETE FROM users_2 WHERE id = 2;"
                        "DELETE FROM users_2;"
                        ""
                        "DROP TABLE users_2;";

    
    query = "CREATE TABLE Kunden1 ( ID INT PRIMARY KEY, Name TEXT NOT NULL );";

    std::vector<Token> tokens = lex(query);

    for(int i=0; i<tokens.size(); i++){
        std::cout << token_str(tokens.at(i)) << std::endl;
    }

    try{
        
        Cursor cursor{tokens, 0};
        std::vector<StmtPtr> program = parseProgram(cursor);



    } catch (const ParseError& e){
        std::cerr << e.what() << std::endl;
    }

    
    return 0;
}