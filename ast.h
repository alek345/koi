#ifndef AST_H
#define AST_H

enum NodeType {
    NODE_LITERAL = 0,
    NODE_VARIABLE,
    NODE_FUNCCALL,
    NODE_VARDECL,
    NODE_ASSIGNMENT,
    NODE_VARDECLASSIGN,
    NODE_RETURN,
    NODE_BINOP,
    NODE_UNARY,
    NODE_FUNCDEF,
    NODE_IF,
    // TODO: NODE_STRUCTDEF,
};

enum LiteralType {
    LITERAL_INTEGER = 0,
    LITERAL_FLOAT,
    LITERAL_STRING,
};

enum BinOpType {
    BINOP_ADD = 0,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
};

enum BooleanOpType {
    BOOLOP_LESSTHAN = 0,
    BOOLOP_GREATERTHAN,
    BOOLOP_OR,
    BOOLOP_AND,
    BOOLOP_NOT,
};

enum UnaryType {
    UNARY_PLUS = 0,
    UNARY_MINUS,
};

enum IfType {
    IF_IF = 0,
    IF_IFELSE,
    IF_ELIF,
    IF_ELIFELSE,
    IF_ELSE,
};

struct Node {
    NodeType type;
    Node *prev;
    Node *next;
    
    union {
        struct {
            LiteralType type;
            union { int intVal; float floatVal; char *strVal; };
        } literal;
        struct {
            char *name;
        } variable;
        struct {
            char *name;
            int num_arguments;
            Node** arguments;
        } funccall;
        struct {
            char *name;
            char *type;
        } vardecl;
        struct {
            char *name;
            Node *expr;
        } assignment;
        struct {
            char *name;
            char *type;
            Node *expr;
        } vardeclassign;
        struct {
            Node *expr;
        } ret;
        struct {
            BinOpType type;
            Node *lhs;
            Node *rhs;
        } binop;
        struct {
            UnaryType type;
            Node *expr;
        } unary;
        struct {
            char *name;
            int num_arguments;
            char **arguments;
            char **argument_types;
            Node *stmts;
        } funcdef;
        struct {
            IfType type;
            Node *condition;
            Node *stmts;
            Node *next;
        } ifstmt;
        struct {
            BooleanOpType type;
            Node *expr;
        } boolop;
    };
};

#endif /* AST_H */