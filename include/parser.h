#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"

struct TokenList {
    int num_tokens;
    Token **tokens;
    
    TokenList();
    ~TokenList();
    
    void Add(Token *t);
};

struct NodeList {
    int num_nodes;
    Node **nodes;
    
    NodeList();
    ~NodeList();
    
    void Add(Node *n);
};

struct Parser {
    Lexer *lexer;
    
    Parser(const char *path);
    Node* Parse();
    
    void Error(Token *t, const char *msg, ...);
    
    Node* Expr(TokenList* list);
    Node* FunctionDef();
    Node* VarDecl();
    Node* Ident();
    Node* If();
    Node* Return();
    Node* Stmt();
    Node* GlobalStmt();
};

#endif /* PARSER_H */
