#include <iostream>

#include "../include/parse.hpp"
#include "../include/lex.hpp"


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

    ret->type = STMT_CREATE;
    
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

ExprPtr parseExpr(Cursor& c){

    const Token& t = peek(c);
    
    switch(t.type){
        default: 
            throw ParseError("Expected fieldname or value. Got: '" + token_type_str(t.type) + "' ", t);

        case TT_IDENT:{
            auto col = std::make_unique<ColumnExpr>();
            col->type = EXPR_COLUMN;
            col->name = advance(c).lexeme;
            return col;
        }

        case TT_INT:{
            auto num = std::make_unique<LiteralExpr>();
            num->type = EXPR_LITERAL;
            num->kind = LiteralExpr::Kind::Int;
            num->i = std::stoll(advance(c).lexeme);
            return num;
        }

        case TT_STRING:{
            auto str = std::make_unique<LiteralExpr>();
            str->type = EXPR_LITERAL;
            str->kind = LiteralExpr::Kind::String;
            str->s = advance(c).lexeme;
            return str;
        }

    }

}

Op parseOp(Cursor& c){
    

    if(match(c, TT_EQUAL)){
        return Op::Eq;
    }
    else if(match(c, TT_LESS)){
        
        if(match(c, TT_EQUAL)){
            return Op::Le;
        }
        else if(match(c, TT_GREATER)){
            return Op::Ne;
        }
        
        return Op::Lt;
    }
    else if(match(c, TT_GREATER)){

        if(match(c, TT_EQUAL)){
            return Op::Ge;
        }

        return Op::Gt;

    }

    throw ParseError("Expected Operator, got '" + token_str(peek(c)) + "'", peek(c));
}

ExprPtr makeBinary(Op op, ExprPtr lhs, ExprPtr rhs){
    auto expr = std::make_unique<BinaryExpr>();
    expr->type = EXPR_BINARY;
    expr->op = op;
    expr->lhs = std::move(lhs);
    expr->rhs = std::move(rhs);
    return expr;
}

ExprPtr makeUnary(Op op, ExprPtr operand){
    auto expr = std::make_unique<UnaryExpr>();
    expr->type = EXPR_UNARY;
    expr->op = op;
    expr->operand = std::move(operand);
    return expr;
}

// <factor>      ::= [ "NOT" ] ( <expr> <op> <expr> | "(" <cond> ")" )
ExprPtr parseFactor(Cursor& c){

    bool negated = match(c, "NOT");


    ExprPtr expr;
    if(match(c, TT_LPAREN)){
        expr = parseCondition(c);
        expect(c, TT_RPAREN);
    }else{
        ExprPtr lhs = parseExpr(c);
        Op op = parseOp(c);
        ExprPtr rhs = parseExpr(c);

        expr = makeBinary(op, std::move(lhs), std::move(rhs));
    }

    if(negated)
        return makeUnary(Op::Not, std::move(expr));

    return expr;

}

// <term>        ::= <factor> { "AND" <factor> }
ExprPtr parseTerm(Cursor& c){

    ExprPtr lhs = parseFactor(c);

    while(match(c, "AND")){
        ExprPtr rhs = parseFactor(c);
        lhs = makeBinary(Op::And, std::move(lhs), std::move(rhs));
    }

    return lhs;

}

// <cond>        ::= <term> { "OR" <term> }
ExprPtr parseCondition(Cursor& c){
    
    ExprPtr lhs = parseTerm(c);

    while(match(c, "OR")){
        ExprPtr rhs = parseTerm(c);
        lhs = makeBinary(Op::Or, std::move(lhs), std::move(rhs));
    }

    return lhs;

}

// <delete>      ::= "DELETE" "FROM" <ident> [ "WHERE" <cond> ]
std::unique_ptr<DeleteStmt> parseDeleteStmt(Cursor& c){
    expect(c, "DELETE");
    expect(c, "FROM");

    auto ret = std::make_unique<DeleteStmt>();

    ret->type = STMT_DELETE;

    // get table name
    ret->table = expect(c, TT_IDENT).lexeme;

    if(match(c, "WHERE")){
        ret->where = parseCondition(c);
    }
    
    return ret;
}


StmtPtr parseStmt(Cursor& cursor){

    Token current = peek(cursor);
    
    if (current.type != TT_KEYWORD)
        throw ParseError("Expected Statement", current);

    if (current.lexeme == "CREATE")     return parseCreateStmt(cursor);
    if (current.lexeme == "DELETE")     return parseDeleteStmt(cursor);
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


void indent(std::ostream& os, int depth) {
    os << std::string(depth * 2, ' ');
}

void printExpr(std::ostream& os, const Expr& e, int depth) {
    indent(os, depth);
    switch (e.type) {
        case EXPR_BINARY: {
            auto& b = static_cast<const BinaryExpr&>(e);
            os << "Binary " << Op_str(b.op) << "\n";
            printExpr(os, *b.lhs, depth + 1);
            printExpr(os, *b.rhs, depth + 1);
            break;
        }
        case EXPR_UNARY: {
            auto& u = static_cast<const UnaryExpr&>(e);
            os << "Unary " << Op_str(u.op) << "\n";
            printExpr(os, *u.operand, depth + 1);
            break;
        }
        case EXPR_COLUMN:
            os << "Column " << static_cast<const ColumnExpr&>(e).name << "\n";
            break;
        case EXPR_LITERAL: {
            auto& l = static_cast<const LiteralExpr&>(e);
            os << "Literal ";
            switch (l.kind) {
                case LiteralExpr::Kind::Int:    os << l.i;              break;
                case LiteralExpr::Kind::String: os << "'" << l.s << "'"; break;
                case LiteralExpr::Kind::Null:   os << "NULL";           break;
            }
            os << "\n";
            break;
        }
        default:
            os << "???\n";
    }
}

void printStmt(std::ostream& os, const Stmt& s, int depth) {
    indent(os, depth);
    switch (s.type) {
        case STMT_CREATE: {
            auto& c = static_cast<const CreateStmt&>(s);
            os << "CREATE " << c.table << "\n";
            for (const auto& col : c.columns) {
                indent(os, depth + 1);
                os << col->name << " " << (col->type == ColumnType::Int ? "INT" : "TEXT")
                   << (col->primaryKey ? " PRIMARY KEY" : "")
                   << (col->notNull    ? " NOT NULL"    : "") << "\n";
            }
            break;
        }
        case STMT_DELETE: {
            auto& d = static_cast<const DeleteStmt&>(s);
            os << "DELETE " << d.table << "\n";
            if (d.where) printExpr(os, *d.where, depth + 1);
            break;
        }
        // STMT_DROP, STMT_INSERT, STMT_SELECT, STMT_UPDATE nach demselben Muster
        default:
            os << "???\n";
    }
}