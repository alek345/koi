#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "bytecode.h"
#include "context.h"
#include "cbuilder.h"

void print_indents(int length) {
    for(int i = 0; i < length; i++) {
        printf("  ");
    }
}

void print_node(Node *n) {
	static int indent_size = 0;
	
	if(n == NULL) {
		print_indents(indent_size);
		printf("(null)\n");
		return;
	}

    switch(n->type) {
		case NODE_CFUNCDEF: {	
			print_indents(indent_size);
            printf("CFuncdef: %s\n", n->cfuncdef.name);
			print_indents(indent_size);
			printf("Arguments (%d):\n", n->cfuncdef.num_arguments);
            for(int i = 0; i < n->cfuncdef.num_arguments; i++) {
                printf("Arg %d: %s\n", i+1, n->cfuncdef.arguments[i]);
            }
		} break;

		case NODE_RETURN: {
            print_indents(indent_size);
            printf("Return\n");
			print_indents(indent_size);
			printf("Expr: \n");
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
       
		case NODE_IF: {
			print_indents(indent_size);
			printf("if expr:\n");
			indent_size++;
			print_node(n->ifstmt.condition);
			indent_size--;
			print_indents(indent_size);
			printf("stmts:\n");
			indent_size++;
            Node *nn = n->ifstmt.stmts;
            while(nn != NULL) {
                print_node(nn);
                nn = nn->next;
            }
			indent_size--;
			print_indents(indent_size);
			printf("end of if stmts\n");
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
            print_indents(indent_size);
			printf("Assignment: %s\nExpr:\n", n->assignment.name);
            indent_size++;
            print_node(n->assignment.expr);
            indent_size--;
        } break;
        
        case NODE_FUNCCALL: {
            print_indents(indent_size);
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
			print_indents(indent_size);
            printf("Funcdef: %s\nArguments (%d):\n", 
                n->funcdef.name, n->funcdef.num_arguments);
            for(int i = 0; i < n->funcdef.num_arguments; i++) {
                printf("Arg %d: %s\n", i+1, n->funcdef.arguments[i]);
            }
            print_indents(indent_size);
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
    
	Context *context = new Context();
    context->Analyse(nodes);
    
    BytecodeBuilder builder;
    builder.Generate(context);
    builder.Write("output.koic");

    printf("Code size: %d\n", builder.code_size);
    
    VirtualMachine vm(&builder);
    int32_t ret_code = vm.Run(true);
    printf("return code: %d/%f\n",
        ret_code, *((float*)&ret_code)
    );
    
	printf("\nGlobals:\n");
	vm.PrintData();

	CBuilder cbuilder(&builder);
	cbuilder.Write("output.c");

	system("gcc -g output.c -o cbuild");

    return 0;
}
