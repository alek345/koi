#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "bytecode.h"

void print_indents(int length) {
    for(int i = 0; i < length; i++) {
        printf("  ");
    }
}

void print_node(Node *n) {
    static int indent_size = 0;
    switch(n->type) {
        case NODE_RETURN: {
            print_indents(indent_size);
            printf("Return\nExpr:\n");
            indent_size++;
            print_node(n->ret.expr);
            indent_size--;
        } break;
        
        case NODE_LITERAL: {
            print_indents(indent_size);
            printf("Literal: ");
            switch(n->literal.type) {
                case LITERAL_INTEGER: {
                    printf("%d\n", n->literal.intVal);
                } break;
                case LITERAL_FLOAT: {
                    printf("%f\n", n->literal.floatVal);
                } break;
                case LITERAL_STRING: {
                    printf("\"%s\"\n", n->literal.strVal);
                } break;
                
                default: printf("What?\n");
            }
        } break;
        
        case NODE_BINOP: {
            print_indents(indent_size);
            printf("Binop: ");
            
            switch(n->binop.type) {
                case BINOP_ADD: {
                    printf("+\n");
                } break;
                case BINOP_SUB: {
                    printf("-\n");
                } break;
                case BINOP_MUL: {
                    printf("*\n");
                } break;
                case BINOP_DIV: {
                    printf("/\n");
                } break;
            }
            
            print_indents(indent_size);
            printf("lhs:\n");
            indent_size++;
            print_node(n->binop.lhs);
            indent_size--;
            print_indents(indent_size);
            printf("rhs:\n");
            indent_size++;
            print_node(n->binop.rhs);
            indent_size--;
        } break;
        
        case NODE_VARIABLE: {
            print_indents(indent_size);
            printf("Variable: %s\n", n->variable.name);
        } break;
        
        case NODE_VARDECL: {
            print_indents(indent_size);
            printf("Vardecl: %s\n", n->vardecl.name);
        } break;
        
        case NODE_VARDECLASSIGN: {
            print_indents(indent_size);
            printf("Vardeclassign: %s\nExpr:\n", n->vardeclassign.name);
            indent_size++;
            print_node(n->vardeclassign.expr);
            indent_size--;
        } break;
        
        case NODE_ASSIGNMENT: {
            printf("Assignment: %s\nExpr:\n", n->assignment.name);
            indent_size++;
            print_node(n->assignment.expr);
            indent_size--;
        } break;
        
        case NODE_FUNCCALL: {
            printf("Funccall: %s\nArguments (%d):\n", 
                n->funccall.name, n->funccall.num_arguments);
            for(int i = 0; i < n->funccall.num_arguments; i++) {
                printf("Arg %d\nExpr:\n", i+1);
                indent_size++;
                print_node(n->funccall.arguments[i]);
                indent_size--;
            }
        } break;
        
        case NODE_FUNCDEF: {
            printf("Funcdef: %s\nArguments (%d):\n", 
                n->funcdef.name, n->funcdef.num_arguments);
            for(int i = 0; i < n->funcdef.num_arguments; i++) {
                printf("Arg %d: %s\n", i+1, n->funcdef.arguments[i]);
            }
            printf("Body:\n");
            indent_size++;
            Node *nn = n->funcdef.stmts;
            while(nn != NULL) {
                print_node(nn);
                nn = nn->next;
            }
            indent_size--;
        } break;
    }
}
    
void print_bits(uint32_t v) {
    for(int i = 31; i >= 0; i--) {
        putchar('0' + ((v >> i) & 1));
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: koi <source>\n");
        return 0;
    }
    
    Parser p(argv[1]);
    printf("Starting parsing!\n");
    Node *nodes = p.Parse();
    printf("Done parsing!\n");
    
    Node *n = nodes;
    while(n != NULL) {
        print_node(n);
        n = n->next;
    }
    
    printf("\n\nBytecode:\n\n");
    
    conversion a, b; 
    a.floatVal = 3.1415f;
    b.floatVal = 2.5f;
    
    uint32_t test[] = {
        OP_CONST, 4,
        OP_CONST, 5,
        OP_CALL, 8, 2,
        OP_HALT,
        
        // func
        OP_LOAD, (int32_t)-4,
        OP_LOAD, (int32_t)-3,
        OP_IISUB,
        OP_RET,
    };
    
    Node *expr = new Node();
    expr->type = NODE_BINOP;
    expr->binop.type = BINOP_ADD;
    expr->binop.lhs = new Node();
    expr->binop.lhs->type = NODE_LITERAL;
    expr->binop.lhs->literal.type = LITERAL_INTEGER;
    expr->binop.lhs->literal.intVal = 5;
    
    Node *rhs = new Node();
    expr->binop.rhs = rhs;
    rhs->type = NODE_BINOP;
    rhs->binop.type = BINOP_MUL;
    
    rhs->binop.lhs = new Node();
    rhs->binop.lhs->type = NODE_LITERAL;
    rhs->binop.lhs->literal.type = LITERAL_INTEGER;
    rhs->binop.lhs->literal.intVal = 3;
    
    rhs->binop.rhs = new Node();
    rhs->binop.rhs->type = NODE_LITERAL;
    rhs->binop.rhs->literal.type = LITERAL_INTEGER;
    rhs->binop.rhs->literal.intVal = 2;
    
    Node *retn = new Node();
    retn->type = NODE_RETURN;
    retn->ret.expr = expr;
    
    BytecodeBuilder builder;
    builder.Add(OP_CALL);
    builder.Add(4);
    builder.Add(0);
    builder.Add(OP_HALT);
    builder.Generate(retn);
    
    //VirtualMachine vm(test, sizeof(test)/sizeof(test[0]), 0);
    VirtualMachine vm(builder.data, builder.data_size, 0);
    int32_t ret = vm.Run(true);
    printf("\nvm ret: %d\n", ret);
    
    return 0;
}