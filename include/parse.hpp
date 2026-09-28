#pragma once
#include <memory>
#include <string>
#include <vector>


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

enum class Op { Eq, Ne, Lt, Gt, Le, Ge, And, Or, Not };
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
struct LiteralExpr : Expr { enum { Int, String, Null } kind;     // 5, 'Anna', NULL
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
