<program>     ::= { <statement> ";" }

<statement>   ::= <create> | <drop> | <insert> | <select> | <update> | <delete>

(* DDL *)
<create>      ::= "CREATE" "TABLE" <ident> "(" <col-def> { "," <col-def> } ")"
<col-def>     ::= <ident> <type> [ "PRIMARY" "KEY" ] [ "NOT" "NULL" ]
<type>        ::= "INT" | "TEXT"
<drop>        ::= "DROP" "TABLE" <ident>

(* DML *)
<insert>      ::= "INSERT" "INTO" <ident> [ "(" <ident-list> ")" ]
                  "VALUES" "(" <value-list> ")"
<select>      ::= "SELECT" <select-list> "FROM" <ident>
                  [ "WHERE" <cond> ] [ "ORDER" "BY" <ident> [ "ASC" | "DESC" ] ]
<update>      ::= "UPDATE" <ident> "SET" <assign> { "," <assign> } [ "WHERE" <cond> ]
<delete>      ::= "DELETE" "FROM" <ident> [ "WHERE" <cond> ]

<select-list> ::= "*" | <ident-list>
<ident-list>  ::= <ident> { "," <ident> }
<value-list>  ::= <value> { "," <value> }
<assign>      ::= <ident> "=" <expr>

(* Bedingungen *)
<cond>        ::= <term> { "OR" <term> }
<term>        ::= <factor> { "AND" <factor> }
<factor>      ::= [ "NOT" ] ( <expr> <op> <expr> | "(" <cond> ")" )
<op>          ::= "=" | "<>" | "<" | ">" | "<=" | ">="

<expr>        ::= <ident> | <value>
<value>       ::= <number> | <string> | "NULL"

(* Lexik *)
<ident>       ::= <letter> { <letter> | <digit> | "_" }
<number>      ::= [ "-" ] <digit> { <digit> }
<string>      ::= "'" { <char> } "'"


std::string query =     "CREATE TABLE users_2 (id INT PRIMARY KEY NOT NULL, name TEXT NOT NULL, age INT, nick TEXT);"

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