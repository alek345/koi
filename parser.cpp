#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
   
TokenList::TokenList() {
    num_tokens = 0;
    tokens = NULL;
}
    
TokenList::~TokenList() {
    free(tokens);
}
    
void TokenList::Add(Token *t) {
    num_tokens++;
    tokens = (Token**) realloc(tokens, sizeof(Token*) * num_tokens);
    tokens[num_tokens-1] = t;
}

NodeList::NodeList() {
    num_nodes = 0;
    nodes = NULL;
}
    
NodeList::~NodeList() {
    free(nodes);
}
    
void NodeList::Add(Node *n) {
    num_nodes++;
    nodes = (Node**) realloc(nodes, sizeof(Node*) * num_nodes);
    nodes[num_nodes-1] = n;
}

void Parser::Error(Token *t, char *msg, ...) {
    printf("L%d:%d Error: ", t->line_number, t->line_offset);
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    printf("\n");
    exit(-1);
}

Parser::Parser(const char *path) {
    lexer = new Lexer(path);
}

template<typename T> struct Stack {
    T *data;
    int top;
    
    Stack() {
        data = NULL;
        top = -1;
    }
    
    ~Stack() {
        free(data);
    }
    
    void Push(T item) {
        top++;
        data = (T*) realloc(data, sizeof(T)*(top+1));
        data[top] = item;
    }
    
    T Peek() {
        if(top == -1) {
            return NULL;
        }
        return data[top];
    }
    
    T Pop() {
        if(top == -1) {
            return NULL;
        }
        return data [top--];
    }
};

int get_precedence(Token *t) {
    switch(t->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        return 2;
        case TOKEN_ASTRIX:
        case TOKEN_SLASH:
        return 3;
        case TOKEN_LESSTHAN:
        case TOKEN_GREATERTHAN:
        return 1;
    }
}

bool is_rightassoc(Token *t) {
    switch(t->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_ASTRIX:
        case TOKEN_SLASH:
        return true;
    }
    
    return false;
}

bool is_operator(Token *t) {
    switch(t->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_ASTRIX:
        case TOKEN_SLASH:
        case TOKEN_LESSTHAN:
        case TOKEN_GREATERTHAN:
        return true;
    }
    
    return false;
}

Node* Parser::Expr(TokenList *tokens) {
    Stack<Token*> opStack;
    Stack<Token*> output;
    
    for(int i = 0; i < tokens->num_tokens; i++) {
        Token *t = tokens->tokens[i];
        if(t->type == TOKEN_END) break;
        
        if(t->type == TOKEN_INTEGER || t->type == TOKEN_FLOAT
        || t->type == TOKEN_STRING) {
            // Should strings be put here?
            output.Push(t);
            continue;
        }
        
        if(t->type == TOKEN_IDENT) {
            // Can be a function call if succeeded by ':'
            output.Push(t);
            continue;
        }
        
        if(is_operator(t)) {
            Token *t2 = NULL;
            while(t2 = opStack.Peek()) {
                bool t1assoc = is_rightassoc(t);
                int t1prec = get_precedence(t);
                int t2prec = get_precedence(t2);
                
                if(t1assoc) {
                    if(t1prec < t2prec) {
                        opStack.Pop();
                        output.Push(t2);
                    } else {
                        break;
                    }
                } else {
                    if(t1prec <= t2prec) {
                        opStack.Pop();
                        output.Push(t2);
                    }else {
                        break;
                    }
                }
            }
            opStack.Push(t);
            continue;
        }
        
        if(t->type == TOKEN_LEFTPAR) {
            opStack.Push(t);
            continue;
        }
        
        if(t->type == TOKEN_RIGHTPAR) {
            Token *t2 = NULL;
            while(t2 = opStack.Peek()) {
                if(t2 == NULL) {
                    Error(t2, "Unmatched parenthesis in expression!");
                }
                if(t2->type == TOKEN_LEFTPAR) break;
                
                output.Push(opStack.Pop());
            }
            opStack.Pop();
            
            // TODO: If function token pop to output, look at "parser"
        }
    }
    
    while(opStack.Peek()) {
        Token *t2 = opStack.Peek();
        
        if(t2->type == TOKEN_LEFTPAR || t2->type == TOKEN_RIGHTPAR) {
            Error(t2, "Unmatched parenthesis in expression!");
        }
        
        output.Push(opStack.Pop());
    }
    
    for(int i = 0; i < output.top+1; i++) {
        printf("%s\n", token_type_to_string(output.data[i]->type));
    }
    
    Stack<Node*> nodes;
    for(int i = 0; i < output.top+1; i++) {
        Token *t = output.data[i];
        
        switch(t->type) {
            case TOKEN_INTEGER: {
                Node *n = new Node();
                n->type = NODE_LITERAL;
                n->literal.type = LITERAL_INTEGER;
                n->literal.intVal = t->intVal;
                nodes.Push(n);
            } break;
            
            case TOKEN_FLOAT: {
                Node *n = new Node();
                n->type = NODE_LITERAL;
                n->literal.type = LITERAL_FLOAT;
                n->literal.floatVal = t->floatVal;
                nodes.Push(n);
            } break;
            
            case TOKEN_STRING: {
                Node *n = new Node();
                n->type = NODE_LITERAL;
                n->literal.type = LITERAL_STRING;
                n->literal.strVal = t->strVal;
                nodes.Push(n);
            } break;
            
            case TOKEN_IDENT: {
                Node *n = new Node();
                n->type = NODE_VARIABLE;
                n->variable.name = t->strVal;
                nodes.Push(n);
            } break;
            
            case TOKEN_PLUS: {
                Node *n = new Node();
                n->type = NODE_BINOP;
                n->binop.type = BINOP_ADD;
                n->binop.lhs = nodes.Pop();
                n->binop.rhs = nodes.Pop();
                nodes.Push(n);
            } break;
            case TOKEN_MINUS: {
                Node *n = new Node();
                n->type = NODE_BINOP;
                n->binop.type = BINOP_SUB;
                n->binop.lhs = nodes.Pop();
                n->binop.rhs = nodes.Pop();
                nodes.Push(n);
            } break;
            case TOKEN_ASTRIX: {
                Node *n = new Node();
                n->type = NODE_BINOP;
                n->binop.type = BINOP_MUL;
                n->binop.lhs = nodes.Pop();
                n->binop.rhs = nodes.Pop();
                nodes.Push(n);
            } break;
            case TOKEN_SLASH: {
                Node *n = new Node();
                n->type = NODE_BINOP;
                n->binop.type = BINOP_DIV;
                n->binop.lhs = nodes.Pop();
                n->binop.rhs = nodes.Pop();
                nodes.Push(n);
            } break;
        }
    }
    
    bool first_done = false;
    Node *n;
    for(int i = 0; i < nodes.top+1; i++) {
        n = nodes.data[i];
        
        if(first_done) {
            n->prev = nodes.data[i-1];
            if(i < nodes.top) {
                n->next = nodes.data[i+1];
            } else {
                n->next = NULL;
            }
        } else {
            n->prev = NULL;
            if(i < nodes.top) {
                n->next = nodes.data[i+1];
            } else {
                n->next = NULL;
            }
            first_done = true;
        }
    }
    
    return nodes.data[0];
}

Node* Parser::FunctionDef() {
    lexer->Next();
    
    if(lexer->Current()->type != TOKEN_IDENT) {
        Error(lexer->Current(), "Expected function name after function token!");
    }
    Token *name = lexer->Current();
    
    int num_arguments = 0;
    char **arguments = NULL;
    char **argument_types = NULL;
    Token *next = lexer->Next();
    while(next->type != TOKEN_COLON) {
        
        if(next->type == TOKEN_IDENT) {
            
            num_arguments++;
            argument_types = (char**) realloc(argument_types, sizeof(char*)*num_arguments);
            argument_types[num_arguments-1] = next->strVal;
            next = lexer->Next();
            
            if(next->type != TOKEN_IDENT) {
                Error(next, "Expected argument name after type!");
            }
            
            arguments = (char**) realloc(arguments, sizeof(char*)*num_arguments);
            arguments[num_arguments-1] = next->strVal;
            
            next = lexer->Next();
            if(next->type == TOKEN_COLON) {
                break;
            } else if(next->type == TOKEN_COMMA) {
                next = lexer->Next();
                continue;
            } else {
                Error(next, "Expected ',' or ':'");
            }
            
            
        } else {
            Error(next, "Expected identifier, got '%s'!", token_type_to_string(next->type));
        }
        
    }
    lexer->Next(); // Consume colon
    
    Node *stmts = Stmt();
    
    if(stmts == NULL) {
        lexer->Next(); // Consume endfunction
        
        Node *n = new Node();
        n->type = NODE_FUNCDEF;
        n->funcdef.name = name->strVal;
        n->funcdef.num_arguments = num_arguments;
        n->funcdef.arguments = arguments;
        n->funcdef.argument_types = argument_types;
        n->funcdef.stmts = stmts;
    }
    
    Node *stmt = stmts;
    while(stmt != NULL) {
        Node *newstmt = Stmt();
        if(newstmt == NULL) break;
        
        stmt->next = newstmt;
        newstmt->prev = stmt;
        stmt = newstmt;
    }
    
    lexer->Next(); // Consume endfunction
    
    Node *n = new Node();
    n->type = NODE_FUNCDEF;
    n->funcdef.name = name->strVal;
    n->funcdef.num_arguments = num_arguments;
    n->funcdef.arguments = arguments;
    n->funcdef.stmts = stmts;
    return n;
}

Node* Parser::VarDecl() {
    lexer->Next();
    
    if(lexer->Current()->type != TOKEN_IDENT) {
        Error(lexer->Current(), "Expected variable name after var keyword!");
    }
    
    Token *name = lexer->Current();
    
    Token *next = lexer->Next();
    
    if(next->type != TOKEN_COLON) {
        Error(next, "Expected ':' after variable name");
    }
    
    next = lexer->Next();
    if(next->type != TOKEN_IDENT) {
        Error(next, "Expected variable type after ':'");
    }
    Token *type = name;
    
    next = lexer->Next();
    if(next->type == TOKEN_EQUALS) {
        
        if(lexer->Peek()->type == TOKEN_SEMICOLON) {
            Error(lexer->Peek(), "Expected expression after '='!");
        }
        
        TokenList *list = new TokenList();
        next = lexer->Next();
        while(next->type != TOKEN_SEMICOLON) {
            list->Add(next);
            next = lexer->Next();
        }
        
        Node *expr = Expr(list);
        delete list;
        
        lexer->Next();
        
        Node *n = new Node();
        n->type = NODE_VARDECLASSIGN;
        n->vardeclassign.name = name->strVal;
        n->vardeclassign.type = type->strVal;
        n->vardeclassign.expr = expr;
        return n;
        
    } else if(next->type == TOKEN_SEMICOLON) {
        
        lexer->Next();
        
        Node *n = new Node();
        n->type = NODE_VARDECL;
        n->vardecl.name = name->strVal;
        n->vardecl.type = type->strVal;
        return n;
        
    } else {
        Error(next, "Expected '=' or ';'");
    }
}

Node* Parser::Ident() {
    Token *name = lexer->Current();
    
    Token *next = lexer->Next();
    
    if(next->type == TOKEN_EQUALS) {
        next = lexer->Next();
        if(next->type == TOKEN_SEMICOLON) {
            Error(next, "Expected expression after '='");
        }
        
        TokenList *list = new TokenList();
        while(next->type != TOKEN_SEMICOLON) {
            list->Add(next);
            next = lexer->Next();
        }
        lexer->Next();
        
        Node *expr = Expr(list);
        delete list;
        
        Node *n = new Node();
        n->type = NODE_ASSIGNMENT;
        n->assignment.name = name->strVal;
        n->assignment.expr = expr;
        return n;
        
    } else if(next->type == TOKEN_COLON) {
        
        Token *next = lexer->Next();
        if(next->type == TOKEN_SEMICOLON) {
            lexer->Next();
            
            Node *n = new Node();
            n->type = NODE_FUNCCALL;
            n->funccall.name = name->strVal;
            n->funccall.num_arguments = 0;
            n->funccall.arguments = NULL;
            return n;
        }
        
        TokenList *list = new TokenList();
        while(next->type != TOKEN_COMMA && next->type != TOKEN_SEMICOLON) {
            list->Add(next);
            next = lexer->Next();
        }
        
        NodeList *nlist = new NodeList();
        Node *expr = Expr(list);
        nlist->Add(expr);
        delete list;
        
        if(next->type == TOKEN_SEMICOLON) {
            lexer->Next();
            
            Node *n = new Node();
            n->type = NODE_FUNCCALL;
            n->funccall.name = name->strVal;
            n->funccall.num_arguments = nlist->num_nodes;
            n->funccall.arguments = nlist->nodes;
            return n;
        }
        
        Node *start = expr;
        
        while(next->type == TOKEN_COMMA) {
            
            next = lexer->Next(); // Consume comma
            TokenList *list = new TokenList();
            while(next->type != TOKEN_COMMA && next->type != TOKEN_SEMICOLON) {
                list->Add(next);
                next = lexer->Next();
            }
            
            Node *newexpr = Expr(list);
            delete list;
            expr->next = newexpr;
            newexpr->prev = expr;
            expr = newexpr;
            nlist->Add(newexpr);
            
            if(next->type == TOKEN_SEMICOLON) {
                lexer->Next();
                
                Node *n = new Node();
                n->type = NODE_FUNCCALL;
                n->funccall.name = name->strVal;
                n->funccall.num_arguments = nlist->num_nodes;
                n->funccall.arguments = nlist->nodes;
                return n;
            }
        }
        
    } else {
        Error(next, "Expected '=' or '(' after identifier!");
    }
}

Node* Parser::If() {
    // This should handle if, elif, and else

	return NULL;
}

Node* Parser::Return() {
    lexer->Next();
    
    if(lexer->Current()->type == TOKEN_SEMICOLON) {
        Error(lexer->Current(), "Expected expression before semicolon");
    }
    
    TokenList *list = new TokenList();
    while(lexer->Current()->type != TOKEN_SEMICOLON) {
        list->Add(lexer->Current());
        lexer->Next();
    }
    lexer->Next();
    
    Node *expr = Expr(list);
    delete list;
    
    Node *n = new Node();
    n->type = NODE_RETURN;
    n->ret.expr = expr;
    
    return n;
}

Node* Parser::Stmt() {
    switch(lexer->Current()->type) {
        case TOKEN_VAR: {
            return VarDecl();
        } break;
        case TOKEN_IDENT: {
            return Ident();
        } break;
        case TOKEN_IF: {
            return If();
        } break;
        case TOKEN_RETURN: {
            return Return();
        } break;
    }
    
    return NULL;
}

Node* Parser::GlobalStmt() {
    switch(lexer->Current()->type) {
        case TOKEN_FUNCTION: {
            return FunctionDef();
        } break;
        // Implement structdef here maybe?
    }
    
    return Stmt();
}

Node* Parser::Parse() {
    lexer->Lex();
    
    if(lexer->Current()->type == TOKEN_END) {
        assert(!"Program is empty!");
        exit(-1);
    }
    
    Node *start = GlobalStmt();
    if(start == NULL) {
        assert(!"Is this program empty?");
        exit(-1);
    }
    
    start->prev = NULL;
    start->next = NULL;
    
    Node *n = start;
    while(n != NULL) {
        Node *nn = GlobalStmt();
        if(nn == NULL) {
            break;
        }
        
        n->next = nn;
        nn->prev = n;
        nn->next = NULL;
        n = nn;
    }
    
    return start;
}
    