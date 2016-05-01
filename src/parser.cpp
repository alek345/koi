#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
   
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

void Parser::Error(Token *t, const char *msg, ...) {
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
	//tokens->Add(TOKEN_END);

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
            // Can be a function call if succeeded by '|'
			/*
			if(tokens->tokens[i+1]->type == TOKEN_PIPE) {
				Token *call = t;
				t->type = TOKEN_FUNCCALL;
				
				i+=2;

				Token *mt = tokens->tokens[i];
				if(mt->type == TOKEN_END) {
					Error(mt, "Unexpected end of file!");
				}

				if(mt->type == TOKEN_PIPE) {
					// No arguments;
					t->funccall.num_arguments = 0;
					t->funccall.arguments = NULL;
					output.Push(t);
					continue;
				}
				
				while(mt->type != TOKEN_COMMA && mt->type != TOKEN_PIPE) {
					
				}

			} else {*/
				output.Push(t);
			//}

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

			default: {
				assert(!"NOOOOPEEEE!");
			}
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

	if (lexer->Next()->type != TOKEN_PIPE) {
		Error(lexer->Current(), "Expected '|' !");
	}

    Token *next = lexer->Next();
    while(next->type != TOKEN_PIPE) {
        
        if(next->type == TOKEN_IDENT) {
            
			num_arguments++;
			arguments = (char**)realloc(arguments, sizeof(char*)*num_arguments);
			arguments[num_arguments - 1] = next->strVal;
            
			next = lexer->Next();
			if (next->type != TOKEN_COLON) {
				Error(next, "Expected ':' after argument name!");
			}

			next = lexer->Next();
            if(next->type != TOKEN_IDENT) {
                Error(next, "Expected argument type after ':'!");
            }

			argument_types = (char**)realloc(argument_types, sizeof(char*)*num_arguments);
			argument_types[num_arguments - 1] = next->strVal;
            
            next = lexer->Next();
            if(next->type == TOKEN_PIPE) {
                break;
            } else if(next->type == TOKEN_COMMA) {
                next = lexer->Next();
                continue;
            } else {
                Error(next, "Expected ',' or '|'");
            }
            
            
        } else {
            Error(next, "Expected identifier, got '%s'!", token_type_to_string(next->type));
        }
        
    }
	if (lexer->Next()->type != TOKEN_LEFTBRACKET) {
		Error(lexer->Current(), "Expected '{'!");
	}
	lexer->Next();

    Node *stmts = Stmt();
    
    if(stmts == NULL) {
		if (lexer->Current()->type != TOKEN_RIGHTBRACKET) {
			Error(lexer->Current(), "Expected '}' at the end of a function, got '%s'!", token_type_to_string(lexer->Current()->type));
		}
		lexer->Next();
        
        Node *n = new Node();
        n->type = NODE_FUNCDEF;
        n->funcdef.name = name->strVal;
        n->funcdef.num_arguments = num_arguments;
        n->funcdef.arguments = arguments;
        n->funcdef.argument_types = argument_types;
        n->funcdef.stmts = stmts;
		
		return n;
    }
    
    Node *stmt = stmts;
    while(stmt != NULL) {
        Node *newstmt = Stmt();
        if(newstmt == NULL) break;
        
        stmt->next = newstmt;
        newstmt->prev = stmt;
        stmt = newstmt;
    }
    
	if (lexer->Current()->type != TOKEN_RIGHTBRACKET) {
		Error(lexer->Current(), "Expected '}' at the end of a function, got '%s'!", token_type_to_string(lexer->Current()->type));
	}
	lexer->Next();
    
    Node *n = new Node();
    n->type = NODE_FUNCDEF;
    n->funcdef.name = name->strVal;
    n->funcdef.num_arguments = num_arguments;
    n->funcdef.arguments = arguments;
    n->funcdef.stmts = stmts;
    return n;
}

TypeType type_name_to_type(char *type_name) {

	if (strcmp(type_name, "int") == 0) {
		return TYPE_INTEGER;
	}
	else if (strcmp(type_name, "float") == 0) {
		return TYPE_FLOAT;
	}
	else if (strcmp(type_name, "string") == 0) {
		return TYPE_STRING;
	}

	return TYPE_UNKNOWN;
}

Node* Parser::VarDecl() {
    lexer->Next();
    
	Token *next = lexer->Current();
	Token *name;
	bool is_struct = false;

	if (next->type == TOKEN_STRUCT) {	
		next = lexer->Next();
		is_struct = true;
		if (next->type != TOKEN_IDENT) {
			Error(next, "Expected identifier after 'struct'!");
		}
		name = next;
	}
	else if (next->type == TOKEN_IDENT) {
		name = next;
	}
	else {
		Error(next, "Exected 'struct' or identifier!");
	}

    next = lexer->Next();
    
    if(next->type != TOKEN_COLON) {
        Error(next, "Expected ':' after variable name");
    }
    
    next = lexer->Next();
    if(next->type != TOKEN_IDENT) {
        Error(next, "Expected variable type after ':'");
    }
    Token *type = next;
    
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
        n->vardeclassign.expr = expr;
        
		Type t;

		if (is_struct) {
			t.type = TYPE_STRUCT;
			t.struct_name = type->strVal;
		}
		else {
			TypeType tt = type_name_to_type(type->strVal);
			if (tt == TYPE_UNKNOWN) {
				Error(type, "Unknown type '%s'!", type->strVal);
			}

			t.type = tt;
		}

		n->vardeclassign.type = t;
        return n;
        
    } else if(next->type == TOKEN_SEMICOLON) {
        
        lexer->Next();
        
        Node *n = new Node();
        n->type = NODE_VARDECL;
        n->vardecl.name = name->strVal;
        
		Type t;

		if (is_struct) {
			t.type = TYPE_STRUCT;
			t.struct_name = type->strVal;
		}
		else {
			TypeType tt = type_name_to_type(type->strVal);
			if (tt == TYPE_UNKNOWN) {
				Error(type, "Unknown type '%s'!", type->strVal);
			}

			t.type = tt;
		}

		n->vardeclassign.type = t;

        return n;
        
    } else {
        Error(next, "Expected '=' or ';'!");
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
        
    } else if(next->type == TOKEN_PIPE) {
        
        Token *next = lexer->Next();
        if(next->type == TOKEN_PIPE) {
			if (lexer->Next()->type != TOKEN_SEMICOLON) {
				Error(lexer->Current(), "Expected ';' at the end of function call!");
			}
			lexer->Next();
            
            Node *n = new Node();
            n->type = NODE_FUNCCALL;
            n->funccall.name = name->strVal;
            n->funccall.num_arguments = 0;
            n->funccall.arguments = NULL;
            return n;
        }
        
        TokenList *list = new TokenList();
        while(next->type != TOKEN_COMMA && next->type != TOKEN_PIPE) {
            list->Add(next);
            next = lexer->Next();
        }

        NodeList *nlist = new NodeList();
        Node *expr = Expr(list);
        nlist->Add(expr);
        delete list;
        
        if(next->type == TOKEN_PIPE) {
			next = lexer->Next();
			if (next->type != TOKEN_SEMICOLON) {
				Error(lexer->Current(), "Expected ';' at the end of function call!");
			}
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
            while(next->type != TOKEN_COMMA && next->type != TOKEN_PIPE) {
                list->Add(next);
                next = lexer->Next();
            }
            
            Node *newexpr = Expr(list);
            delete list;
            expr->next = newexpr;
            newexpr->prev = expr;
            expr = newexpr;
            nlist->Add(newexpr);
            
            if(next->type == TOKEN_PIPE) {
				next = lexer->Next();
				if (next->type != TOKEN_SEMICOLON) {
					Error(lexer->Current(), "Expected ';' at the end of function call!");
				}
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
        Error(next, "Expected '=' or '|' after identifier!");
    }
}

Node* Parser::If() {
	Token *next = lexer->Next();

	if(next->type == TOKEN_LEFTBRACKET) {
		Error(next, "Expected expressopn before '{'\n");
	}

	TokenList *list = new TokenList();
	while(next->type != TOKEN_LEFTBRACKET) {
		list->Add(next);
		next = lexer->Next();
	}
	lexer->Next();

	Node *expr = Expr(list);
	assert(expr); // Expression failed to parser
	delete list;

	Node *stmts = Stmt();

	if(stmts == NULL) {
		// TODO: Expect the correct tokens such as } and endif or else
		next = lexer->Current();
		if(next->type != TOKEN_RIGHTBRACKET){
			Error(next, "Expected '}' after if statement, got '%s'!", token_type_to_string(next->type));
		}

		next = lexer->Next();
		if(next->type == TOKEN_ENDIF) {
			assert(!"not done"); // FIXME	
		} else if(next->type == TOKEN_ELSE) {
			assert(!"not done"); // FIXME
		}  else {
			Error(next, "Expected 'else' or 'endif'\n");
		}

		// No statements to execute
		Node *ifn = new Node();
		ifn->type = NODE_IF;
		ifn->ifstmt.condition = expr;
		ifn->ifstmt.stmts = NULL;

		ifn->ifstmt.elsestmt = NULL; // FIXME:
		return ifn;
	}

    Node *stmt = stmts;
    while(stmt != NULL) {
        Node *newstmt = Stmt();
        if(newstmt == NULL) break;
        
        stmt->next = newstmt;
        newstmt->prev = stmt;
        stmt = newstmt;
    }

	next = lexer->Current();
	if(next->type != TOKEN_RIGHTBRACKET) {
		Error(next, "Expected '}' after if statement, got '%s'!", token_type_to_string(next->type));
	}
	
	next = lexer->Next();
	if(next->type == TOKEN_ENDIF) {
		lexer->Next();

		printf("token after endif: %s\n", token_type_to_string(lexer->Current()->type));

		Node *ifn = new Node();
		ifn->type = NODE_IF;
		ifn->ifstmt.condition = expr;
		ifn->ifstmt.stmts = stmts;
		ifn->ifstmt.elsestmt = NULL; // FIXME
		return ifn;
	} else if (next->type == TOKEN_ELSE) {	
		// TODO: Complete check for another Stmt();
		Node *ifn = new Node();
		ifn->type = NODE_IF;
		ifn->ifstmt.condition = expr;
		ifn->ifstmt.stmts = stmts;
		ifn->ifstmt.elsestmt = NULL; // FIXME
		return ifn;
	} else {
		Error(next, "Expected 'else' or 'endif'!");
	}

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

// cfunc add|a: int, b: int|;
Node* Parser::CFunc() {
	Token *next = lexer->Next();

	if(next->type != TOKEN_IDENT) {
		Error(next, "Expected function name after 'cfunc'!");
	}
	Token *name = next;
	next = lexer->Next();

	if(next->type != TOKEN_PIPE) {
		Error(next, "Expected '|' after function name!");
	}
	next = lexer->Next();

    int num_arguments = 0;
    char **arguments = NULL;
    char **argument_types = NULL;

    while(next->type != TOKEN_PIPE) {
        
        if(next->type == TOKEN_IDENT) {
            
			num_arguments++;
			arguments = (char**)realloc(arguments, sizeof(char*)*num_arguments);
			arguments[num_arguments - 1] = next->strVal;
            
			next = lexer->Next();
			if (next->type != TOKEN_COLON) {
				Error(next, "Expected ':' after argument name!");
			}

			next = lexer->Next();
            if(next->type != TOKEN_IDENT) {
                Error(next, "Expected argument type after ':'!");
            }

			argument_types = (char**)realloc(argument_types, sizeof(char*)*num_arguments);
			argument_types[num_arguments - 1] = next->strVal;
            
            next = lexer->Next();
            if(next->type == TOKEN_PIPE) {
                break;
            } else if(next->type == TOKEN_COMMA) {
                next = lexer->Next();
                continue;
            } else {
                Error(next, "Expected ',' or '|'");
            }
            
            
        } else {
            Error(next, "Expected identifier, got '%s'!", token_type_to_string(next->type));
        }
        
    }

	next = lexer->Next();
	if(next->type != TOKEN_SEMICOLON) {
		Error(next, "Expected ';' after cfunc decleration!");
	}
	lexer->Next();

	Node *cfunc = new Node();
	cfunc->type = NODE_CFUNCDEF;
	cfunc->cfuncdef.name = name->strVal;
	cfunc->cfuncdef.num_arguments = num_arguments;
	cfunc->cfuncdef.arguments = arguments;
	cfunc->cfuncdef.argument_types = argument_types;

	return cfunc;
}

Node* Parser::While() {	
	Token *next = lexer->Next();
			
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
	case TOKEN_WHILE: {
	    return While();
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
		case TOKEN_CFUNC: {
			return CFunc();
		} break;
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
    
