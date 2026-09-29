#include <iostream>

#include "../include/server.hpp"
#include "../include/lex.hpp"
#include "../include/parse.hpp"



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

    
    query = "DELETE FROM Kunden1 WHERE age >= 5 AND name = 'hans';";

    std::vector<Token> tokens = lex(query);

    for(int i=0; i<tokens.size(); i++){
        std::cout << token_str(tokens.at(i)) << std::endl;
    }

    try{
        
        Cursor cursor{tokens, 0};
        std::vector<StmtPtr> program = parseProgram(cursor);

        
        for(const auto& stmt : program){
            printStmt(std::cout, *stmt);
        }



    } catch (const ParseError& e){
        std::cerr << e.what() << std::endl;
    }

    
    return 0;
}