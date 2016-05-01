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
    NODE_WHILE,
	NODE_CFUNCDEF,
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

enum TypeType {
	TYPE_UNKNOWN = -1,
	TYPE_INTEGER = 0,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_STRUCT,
};

struct Type {
	TypeType type;
	char *struct_name;
};

struct Node {
    NodeType type;
    Node *prev;
    Node *next;
    
    union {
		struct {
			Node *condition;
			Node *nodes;
		} whilestmt;
		struct {
            char *name;
            int num_arguments;
            char **arguments;
            char **argument_types;
		} cfuncdef;
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
			Type type;
        } vardecl;
        struct {
            char *name;
            Node *expr;
        } assignment;
        struct {
            char *name;
			Type type;
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
            Node *condition;
            Node *stmts;
            Node *elsestmt;
        } ifstmt;
        struct {
            BooleanOpType type;
            Node *expr;
        } boolop;
    };
};

#endif /* AST_H */
