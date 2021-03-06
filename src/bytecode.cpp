#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifdef __linux__
#include <dlfcn.h>
#elif _WIN32
#include <windows.h>
#endif

typedef void(*cfuncdef)(uint32_t*,uint32_t*);

// Test function
void print_int(uint32_t *stack, uint32_t *ip) {
	uint32_t a = stack[*ip--];
	printf("%d\n", a);
}

typedef void(*vmFunc)(VirtualMachine*);

void halt(VirtualMachine *vm) {
    vm->halt = true;
}

void _const(VirtualMachine *vm) {
    vm->sp++;
    vm->stack[vm->sp] = vm->code[vm->ip++];
}

void iiadd(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    vm->stack[vm->sp] = (int32_t)(a + b);
}

void ffadd(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a + b;
    vm->stack[vm->sp] = c.intVal;
}

void fiadd(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a + b;
    vm->stack[vm->sp] = c.intVal;
}

void ifadd(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    conversion c;
    c.floatVal = a + b;
    vm->stack[vm->sp] = c.intVal;
}

// sub start

void iisub(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    vm->stack[vm->sp] = (int32_t)(a - b);
}

void ffsub(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a - b;
    vm->stack[vm->sp] = c.intVal;
}

void fisub(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a - b;
    vm->stack[vm->sp] = c.intVal;
}

void ifsub(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    conversion c;
    c.floatVal = a - b;
    vm->stack[vm->sp] = c.intVal;
}

// mul start

void iimul(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    vm->stack[vm->sp] = (int32_t)(a * b);
}

void ffmul(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a * b;
    vm->stack[vm->sp] = c.intVal;
}

void fimul(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a * b;
    vm->stack[vm->sp] = c.intVal;
}

void ifmul(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    conversion c;
    c.floatVal = a * b;
    vm->stack[vm->sp] = c.intVal;
}

// div start

void iidiv(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    vm->stack[vm->sp] = (int32_t)(a / b);
}

void ffdiv(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a / b;
    vm->stack[vm->sp] = c.intVal;
}

void fidiv(VirtualMachine *vm) {
    int32_t b = (int32_t) vm->stack[vm->sp--];
    float a = *((float*)&vm->stack[vm->sp--]);
    vm->sp++;
    conversion c;
    c.floatVal = a / b;
    vm->stack[vm->sp] = c.intVal;
}

void ifdiv(VirtualMachine *vm) {
    float b = *((float*)&vm->stack[vm->sp--]);
    int32_t a = (int32_t) vm->stack[vm->sp--];
    vm->sp++;
    conversion c;
    c.floatVal = a / b;
    vm->stack[vm->sp] = c.intVal;
}

void pop(VirtualMachine *vm) {
    vm->sp--;
}

void br(VirtualMachine *vm) {
    vm->ip = vm->code[vm->ip++];
}

void brt(VirtualMachine *vm) {
    uint32_t addr = vm->code[vm->ip++];
    if(vm->stack[vm->sp--] != 0) vm->ip = addr;
}

void brf(VirtualMachine *vm) {
    uint32_t addr = vm->code[vm->ip++];
    if(vm->stack[vm->sp--] == 0) vm->ip = addr;
}

void call(VirtualMachine *vm) {
    uint32_t addr = vm->code[vm->ip++];
    int nargs = vm->code[vm->ip++];
    vm->stack[++vm->sp] = nargs;
    vm->stack[++vm->sp] = vm->fp;
    vm->stack[++vm->sp] = vm->ip;
    vm->fp = vm->sp;
    vm->ip = addr;
}

void ret(VirtualMachine *vm) {
    int retval = vm->stack[vm->sp--];
    vm->sp = vm->fp;
    vm->ip = vm->stack[vm->sp--];
    vm->fp = vm->stack[vm->sp--];
    int nargs = vm->stack[vm->sp--];
    vm->sp -= nargs;
    vm->stack[++vm->sp] = retval;
}

void load(VirtualMachine *vm) {
    int32_t val = (int32_t) vm->code[vm->ip++];
    vm->stack[++vm->sp] = vm->stack[vm->fp + val];
}

void store(VirtualMachine *vm) {
    int32_t val = (int32_t) vm->code[vm->ip++];
    vm->stack[vm->fp + val] = vm->stack[vm->sp--];
}

void gstore(VirtualMachine *vm) {
    int32_t val = (int32_t) vm->code[vm->ip++];
    vm->data[val] = vm->stack[vm->sp--];
}

void gload(VirtualMachine *vm) {
	int32_t val = vm->code[vm->ip++];
	vm->stack[++vm->sp] = vm->data[val];
}

void cfunc(VirtualMachine *vm) {
	// Read unitil \0 is found, this is the function name
	char *funcname = NULL;
	int name_len = 0;

	while(vm->code[vm->ip]) {
		name_len++;
		funcname = (char*) realloc(funcname, sizeof(char)*name_len);
		funcname[name_len-1] = vm->code[vm->ip++];
	}

	name_len++;
	funcname = (char*) realloc(funcname, sizeof(char)*name_len);
	funcname[name_len-1] = 0;
	vm->ip++;

	uint32_t num_args = vm->code[vm->ip++];
#ifdef __linux__
	void *handle = dlopen("koistd/libkoistd.so", RTLD_LAZY);
	if(!handle) {
		printf("Failed to open this file as handle!\n");
	}
	//printf("funcname: %s\n", funcname);
	cfuncdef function = (cfuncdef) dlsym(handle, funcname);
	if(!function) {
		printf("Cfunc points to NULL\nname: %s\n", funcname);
		printf("dlerror(): %s\n", dlerror());
	}
	function(vm->stack, (uint32_t*)&vm->sp);
#elif _WIN32
#warning "WIN32: CFUNC in virtual machine is not supported yet!"
#else
#warning "We dont support cfunc on this platoform... Yet?"
#endif
}

const char *op_to_string(Operand op) {
    switch(op) {
#define OP(op) case op: { return #op ;}
        OP(OP_HALT);
        OP(OP_CONST);
        OP(OP_IIADD);
        OP(OP_FFADD);
        OP(OP_FIADD);
        OP(OP_IFADD);
        OP(OP_IISUB);
        OP(OP_FFSUB);
        OP(OP_FISUB);
        OP(OP_IFSUB);
        OP(OP_IIMUL);
        OP(OP_FFMUL);
        OP(OP_FIMUL);
        OP(OP_IFMUL);
        OP(OP_IIDIV);
        OP(OP_FFDIV);
        OP(OP_FIDIV);
        OP(OP_IFDIV);
        OP(OP_BR);
        OP(OP_BRT);
        OP(OP_BRF);
        OP(OP_CALL);
        OP(OP_RET);
        OP(OP_POP);
        OP(OP_LOAD);
        OP(OP_STORE);
        OP(OP_GSTORE);
		OP(OP_GLOAD);
		OP(OP_CFUNC);
#undef OP
    }
    
    return "(Unknown)";
}

VirtualMachine::VirtualMachine(
    uint32_t *code, uint32_t code_size, uint32_t data_size
) {
    this->code = code;
    data = (uint32_t*) malloc(sizeof(uint32_t)*data_size);
    stack = (uint32_t*) malloc(sizeof(uint32_t)*STACK_SIZE);
    
    this->code_size = code_size;
    this->data_size = data_size;
    
    ip = 0;
    sp = -1;
    fp = -1;
    halt = false;
}

VirtualMachine::VirtualMachine(BytecodeBuilder *builder) 
: VirtualMachine(builder->code, builder->code_size, builder->data_size)
{
    ip = builder->start_ip;
};

void VirtualMachine::PrintData() {

	for (int i = 0; i < data_size; i++) {

		conversion c;
		c.intVal = data[i];
		printf("gvar[%d]: %d/%f\n", i, c.sintVal, c.floatVal);
	}

}

static vmFunc ops[] = {
    halt,
    _const,
    iiadd,
    ffadd,
    fiadd,
    ifadd,
    
    iisub,
    ffsub,
    fisub,
    ifsub,
    
    iimul,
    ffmul,
    fimul,
    ifmul,
    
    iidiv,
    ffdiv,
    fidiv,
    ifdiv,
    
    pop,
    
    br,
    brt,
    brf,
    
    call,
    ret,
    
    load,
    store,
    
    gstore,
	gload,

	cfunc,
};

int32_t VirtualMachine::Run(bool trace) {
    printf("ip: %d, code_size: %d\n", ip, code_size);
    
    uint32_t opcode = code[ip];
    while((!halt) && ip < code_size) {
        ip++;
        
        if(trace) {
            printf("%04X: %s:%d", ip, op_to_string((Operand)opcode), opcode);
        }
        
        ops[opcode](this);
        
        if(trace) {
            printf("\t\tstack = [");
            for(int i = 0; i <= sp; i++) {
                printf(" %d/%f", stack[i], *((float*)&stack[i]));
            }
            printf(" ]\n");
        }
        
        opcode = code[ip];
    }
    
    if(sp >= 0) return (int32_t) stack[sp];
    return 0;
}

BytecodeBuilder::BytecodeBuilder() {
    code = NULL;
    data_size = 0;
    code_size = 0;
}

uint32_t BytecodeBuilder::Add(uint32_t val) {
    code_size++;
    code = (uint32_t*) realloc(code, sizeof(uint32_t)*code_size);
    code[code_size-1] = val;
	return code_size-1;
}

void WriteU32(FILE *f, uint32_t value) {
    uint8_t a = (value>>24)&0xFF;
    uint8_t b = (value>>16)&0xFF;
    uint8_t c = (value>>8)&0xFF;
    uint8_t d = (value)&0xFF;
    
    fwrite(&a, 1, 1, f);
    fwrite(&b, 1, 1, f);
    fwrite(&c, 1, 1, f);
    fwrite(&d, 1, 1, f);
}

void BytecodeBuilder::Write(const char *path) {
    
    FILE *f = fopen(path, "wb");
    
    char magic[] = "KOIC";
    fwrite(magic, 1, 4, f);
    
    WriteU32(f, start_ip);
    WriteU32(f, code_size);
    WriteU32(f, data_size);
    
    for(int i = 0; i < code_size; i++) {
        WriteU32(f, code[i]);
    }
    
    fflush(f);
    fclose(f);
}

void BytecodeBuilder::GenerateExpr(Context *context, Function *function, Node *ast) {
    
    Node *n = ast;
    while(n != NULL) {
        switch(n->type) {
            case NODE_LITERAL: {
                switch(n->literal.type) {
                    case LITERAL_INTEGER: {
                        Add(OP_CONST);
                        Add(n->literal.intVal);
                        return;
                        n = n->next;
                    } break;
                    
                    case LITERAL_FLOAT: {
                        Add(OP_CONST);
                        conversion c;
                        c.floatVal = n->literal.floatVal;
                        Add(c.intVal);
                        return;
                        n = n->next;
                    } break;
                    
                    case LITERAL_STRING: {
                        assert(!"BCBuilder: Strings not implemented yet");
                    } break;
                    
                }
            };
            
            case NODE_VARIABLE: {
                
                // We need a way to what function we are in
				// if any

				// Then check for the variable in local space
				// then in global space

				if(function) {
					// We are in a function scope
					if(function->Declared(n->variable.name)) {
						int offset = function->GetIndexOfLocal(n->variable.name);
						if(offset == -1) assert(!"Not declared");

						Add(OP_LOAD);
						conversion c;
						c.sintVal = offset;
						Add(c.intVal);
					} else {
						// Function is not local, maybe global?
						if(context->GlobalDeclared(n->variable.name)) {
							int offset = context->GetIndexOfGlobal(n->variable.name);
							if(offset == -1) assert(!"Not decalred");
							Add(OP_GLOAD);
							conversion c;
							c.sintVal = offset;
							Add(c.intVal);
						} else {
							assert(!"Variable is not declared!");
						}
					}
				} else {
					// We are in global space
					
					if(context->GlobalDeclared(n->variable.name)) {
						int offset = context->GetIndexOfGlobal(n->variable.name);
						if(offset == -1) assert(!"Nope!");
						Add(OP_GLOAD);
						conversion c;
						c.sintVal = offset;
						Add(c.intVal);
					} else {
						assert(!"Variable is not declared!");
					}
				}

                return;
                
            } break;
            
            case NODE_BINOP: {
                // Binop requires info about types
                // but i quess i will get that when i do
                // semantics and parse symbols, functions etc.
                // for now everything is an int
                switch(n->binop.type) {
                    case BINOP_ADD: {
                        GenerateExpr(context, function, n->binop.rhs);
                        GenerateExpr(context, function, n->binop.lhs);
                        Add(OP_IIADD);
                    } break;
                    
                    case BINOP_SUB: {
                        GenerateExpr(context, function, n->binop.rhs);
                        GenerateExpr(context, function, n->binop.lhs);
                        Add(OP_IISUB);
                    } break;
                    
                    case BINOP_MUL: {
                        GenerateExpr(context, function, n->binop.rhs);
                        GenerateExpr(context, function, n->binop.lhs);
                        Add(OP_IIMUL);
                    } break;
                    
                    case BINOP_DIV: {
                        GenerateExpr(context, function, n->binop.rhs);
                        GenerateExpr(context, function, n->binop.lhs);
                        Add(OP_IIDIV);
                    } break;
                }
                
                return;
                n = n->next;
            } break;
        }
    }
    
}

void BytecodeBuilder::GenerateStmt(
		Context *context,
		Function *function,
		Node *stmt
) {
	Node *n = stmt;
	switch(n->type) {
		case NODE_CFUNCDEF: {
			if(function) {
				assert(!"No cfuncdefs in functions");
			}
		} break;
		case NODE_RETURN: {	
			if(function) {
				GenerateExpr(context, function, n->ret.expr);

				Add(OP_GSTORE);
				Add(0); // Reserved global variable

				for (int i = 0; i < function->num_locals; i++) {
					Add(OP_POP);
				}

				Add(OP_GLOAD);
				Add(0); // Reserved global variable

				Add(OP_RET);
			} else {
				GenerateExpr(context, NULL, n->ret.expr);
				// When in global scope we halt the program
				// instead of a op_ret
				Add(OP_HALT);
			}
       	} break;
            
      	case NODE_ASSIGNMENT: {
			if(function) {
				GenerateExpr(context, function, n->assignment.expr);
					
				if(function->Declared(n->assignment.name)) {
					int index = function->GetIndexOfLocal(n->assignment.name);
					if(index == -1) assert(!"Not good");
					Add(OP_STORE);
					Add(*(uint32_t*)&index);
				} else if (context->GlobalDeclared(n->assignment.name)) {
					int index = context->GetIndexOfGlobal(n->assignment.name);
					if(index == -1) assert(!"Not good");
					Add(OP_GSTORE);
					Add(*(uint32_t*)&index);
				} else {
					assert(!"Not local or global");
				}
			} else {
            	GenerateExpr(context, NULL, n->assignment.expr);
            
            	int index = context->GetIndexOfGlobal(n->assignment.name);
            	Add(OP_GSTORE);
            	Add(index);
			}
    	} break;
		case NODE_IF: {
			if(function) {
				GenerateExpr(context, function, n->ifstmt.condition);

				Add(OP_BRF);
				uint32_t jump_address = Add(0); // FIXME: I should be the addresss of the first instruction after the if{}clause
				
				Node *nn = n->ifstmt.stmts;
				while(nn != NULL) {
					GenerateStmt(context, function, nn);
					nn = nn->next;
				}
				uint32_t end_address = Add(0);
				code_size--;
				code[jump_address] = end_address;
			} else {
				GenerateExpr(context, NULL, n->ifstmt.condition);
				Add(OP_BRF);
				uint32_t jump_address = Add(0); // FIXME: Comment above
				Node *nn = n->ifstmt.stmts;
				while(nn != NULL){
					GenerateStmt(context, NULL, nn);
					nn = nn->next;
				}
				uint32_t end_address = Add(0);
				code_size--;
				code[jump_address]= end_address;
			}
		} break;    
       	case NODE_FUNCCALL: { 
         	if(!context->FunctionExists(n->funccall.name)) {
       	        assert(!"Functions does not exist!");
   	        }
         	Function *func = context->GetFunction(n->funccall.name);

			// TODO: Check if argument count and types are correct
                
       		for(int i = 0; i < n->funccall.num_arguments; i++) {
       	      GenerateExpr(context, function, n->funccall.arguments[i]);
    	 	}

			if(func->is_cfunc) {
				Add(OP_CFUNC);
				char *name = n->cfuncdef.name;
				while(*name) {
					Add(*name);
					name++;
				}
				Add(0);
				Add(n->cfuncdef.num_arguments);
			} else {
				Add(OP_CALL);
				Add(func->code_offset);
				Add(n->funccall.num_arguments);
				Add(OP_POP); // Since the value is not assigned anywhere
			}
      	} break;
            
      	default: {
         	printf("node type: %d\n", n->type);
          	assert(!"GenFunc default switch");
      	} break;
	}
}

void BytecodeBuilder::GenerateFunction(Context *context, Function *function) {
    Node *n = function->node->funcdef.stmts;
    while(n != NULL) {
    	// TODO: Replace all this with a call to
		// GenerateStmt(context, function, n)
      	GenerateStmt(context, function, n);
        n = n->next;
    }
    
}

/*
void BytecodeBuilder::Generate(Context *context, Node *n) {
    GenerateStmt(context, NULL, n);
}
*/

void BytecodeBuilder::Generate(Context *context) {
    
    data_size = context->num_globals;
    
    for(int i = 0; i < context->num_functions; i++) {
        Function *func = context->functions[i];
        func->code_offset = this->code_size;
		if(func->is_cfunc) {
			continue;
		}
		for (int i = 0; i < func->num_locals; i++) {
			Add(OP_CONST);
			Add(0);
		}

        // Generate bytecode for the function nodes
        GenerateFunction(context, func);

		Add(OP_GSTORE);
		Add(0); // Reserved global variable

		for (int i = 0; i < func->num_locals; i++) {
			Add(OP_POP);
		}

		Add(OP_GLOAD);
		Add(0);

        // Always add a return at the end
        Add(OP_RET);
    }
    
    // Save the location of where the code has to
    // start execution
    start_ip = code_size;
    
    Node *n = context->stmts;
    int stmts = 0;
    while(n != NULL) {
        printf("stmts: %d\n", ++stmts);
        GenerateStmt(context, NULL, n);
        n = n->next;
    }
    
    // Print 
}
