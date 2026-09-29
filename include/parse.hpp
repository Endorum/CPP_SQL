#pragma once
#include <memory>
#include <string>
#include <vector>

#include "lex.hpp"


enum NodeType{
    

    STMT_CREATE,
    STMT_DROP,
    STMT_INSERT,
    STMT_SELECT,
    STMT_UPDATE,
    STMT_DELETE,

    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_COLUMN,
    EXPR_LITERAL,

};

inline std::string NodeType_str(NodeType type){
    switch(type){
        default: return "Unknown Node Type";
        case STMT_CREATE:   return "CREATE";
        case STMT_DROP:     return "DROP";
        case STMT_INSERT:   return "INSERT";
        case STMT_SELECT:   return "SELECT";
        case STMT_UPDATE:   return "UPDATE";
        case STMT_DELETE:   return "DELETE";
        case EXPR_BINARY:   return "BINARY";
        case EXPR_UNARY:    return "UNARY";
        case EXPR_COLUMN:   return "COLUMN";
        case EXPR_LITERAL:  return "LITERAL";
    }
}

enum class Op { Eq, Ne, Lt, Gt, Le, Ge, And, Or, Not };

inline std::string Op_str(Op op){
    int idx = static_cast<int>(op);
    std::string t[] = {"Eq", "Ne", "Lt", "Gt", "Le", "Ge", "And", "Or", "Not"};
    return t[idx];
}

enum class ColumnType { Int, Text };

// ---- Base Types ----
struct Expr { NodeType type; virtual ~Expr() = default; };
struct Stmt { NodeType type; virtual ~Stmt() = default; };
using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

// ---- Expressions ----
struct BinaryExpr  : Expr { Op op; ExprPtr lhs, rhs; };          // =, <, AND, OR, ...
struct UnaryExpr   : Expr { Op op; ExprPtr operand; };           // NOT
struct ColumnExpr  : Expr { std::string name; };                 // age
struct LiteralExpr : Expr { enum class Kind { Int, String, Null } kind;     // 5, 'Anna', NULL
                            int64_t i; std::string s; };

// ---- Basic structs ----
struct ColumnDef { std::string name; ColumnType type; bool primaryKey, notNull; };
struct Assign    { std::string column; ExprPtr value; };

// ---- Statements ----
struct CreateStmt : Stmt { std::string table; std::vector<std::unique_ptr<ColumnDef>> columns; };
struct DropStmt   : Stmt { std::string table; };

struct InsertStmt : Stmt { std::string table;
                           std::vector<std::string> columns;     // empty = no list
                           std::vector<ExprPtr>     values; };

struct SelectStmt : Stmt { std::vector<std::string> columns;     // empty = *
                           std::string table;
                           ExprPtr where;                        // nullptr = no WHERE
                           std::string orderBy;                  // leer = no ORDER BY
                           bool desc = false; };

struct UpdateStmt : Stmt { std::string table; std::vector<Assign> assigns; ExprPtr where; };
struct DeleteStmt : Stmt { std::string table; ExprPtr where; };


struct ParseError : std::runtime_error {
    size_t line, col;
    ParseError(const std::string& msg, const Token& at)
        : std::runtime_error(msg), line(at.line), col(at.col) {}
};

struct Cursor{
    std::vector<Token> tokens;
    size_t pos;
};

ExprPtr makeBinary(Op op, ExprPtr lhs, ExprPtr rhs);
std::unique_ptr<ColumnDef> parseColumnDef(Cursor& c);
std::unique_ptr<CreateStmt> parseCreateStmt(Cursor& c);
ExprPtr parseExpr(Cursor& c);
Op parseOp(Cursor& c);
ExprPtr parseFactor(Cursor& c);
ExprPtr parseTerm(Cursor& c);
ExprPtr parseCondition(Cursor& c);
std::unique_ptr<DeleteStmt> parseDeleteStmt(Cursor& c);
StmtPtr parseStmt(Cursor& cursor);
std::vector<StmtPtr> parseProgram(Cursor& cursor);

void printExpr(std::ostream& os, const Expr& e, int depth = 0);
void printStmt(std::ostream& os, const Stmt& s, int depth = 0);