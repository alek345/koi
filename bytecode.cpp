#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    vm->code[vm->fp + val] = vm->stack[vm->sp--];
}

char *op_to_string(Operand op) {
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
    
    iimul,
    ffmul,
    fimul,
    ifmul,
    
    pop,
    
    br,
    brt,
    brf,
    
    call,
    ret,
    
    load,
    store,
};

int32_t VirtualMachine::Run(bool trace) {
    printf("ip: %d, code_size: %d\n", ip, code_size);
    while((!halt) && ip < code_size) {
        uint32_t opcode = code[ip++];
        
        if(trace) {
            printf("%04X: %s", ip-1, op_to_string((Operand)opcode));
        }
        
        ops[opcode](this);
        
        if(trace) {
            printf("\t\tstack = [");
            for(int i = 0; i <= sp; i++) {
                printf(" %d/%f", stack[i], *((float*)&stack[i]));
            }
            printf(" ]\n");
        }
    }
    
    if(sp >= 0) return (int32_t) stack[sp];
    return 0;
}

BytecodeBuilder::BytecodeBuilder() {
    code = NULL;
    data_size = 0;
    code_size = 0;
}

void BytecodeBuilder::Add(uint32_t val) {
    code_size++;
    code = (uint32_t*) realloc(code, sizeof(uint32_t)*code_size);
    code[code_size-1] = val;
}

void BytecodeBuilder::GenerateExpr(Context *context, Node *ast) {
    
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
                
                //
                Add(OP_CONST);
                Add(5);
                return;
                
            } break;
            
            case NODE_BINOP: {
                // Binop requires info about types
                // but i quess i will get that when i do
                // semantics and parse symbols, functions etc.
                // for now everything is an int
                switch(n->binop.type) {
                    case BINOP_ADD: {
                        GenerateExpr(context, n->binop.lhs);
                        GenerateExpr(context, n->binop.rhs);
                        Add(OP_IIADD);
                    } break;
                    
                    case BINOP_SUB: {
                        GenerateExpr(context, n->binop.lhs);
                        GenerateExpr(context, n->binop.rhs);
                        Add(OP_IISUB);
                    } break;
                    
                    case BINOP_MUL: {
                        GenerateExpr(context, n->binop.lhs);
                        GenerateExpr(context, n->binop.rhs);
                        Add(OP_IIMUL);
                    } break;
                    
                    case BINOP_DIV: {
                        GenerateExpr(context, n->binop.lhs);
                        GenerateExpr(context, n->binop.rhs);
                        Add(OP_IIDIV);
                    } break;
                }
                
                return;
                n = n->next;
            } break;
        }
    }
    
}

void BytecodeBuilder::GenerateFunction(Context *context, Function *function) {
    
    Node *n = function->node;
    while(n != NULL) {
        switch(n->type) {
            case NODE_RETURN: {
                GenerateExpr(context, n->ret.expr);
                Add(OP_RET);
            } break;
            
            case NODE_ASSIGNMENT: {
                // This may be a argument
                GenerateExpr(context, n->assignment.expr);
                
                // Check if the variable is declared
                // if not check global variables
                // if its not there then report
                // that the variable is not declared int this scope
                
                int index = function->GetIndexOfLocal(n->assignment.name);
                Add(OP_STORE);
                Add(index);
            } break;
            
            case NODE_FUNCCALL: {
                
                if(!context->FunctionExists(n->funccall.name)) {
                    assert(!"Functions does not exist!");
                }
                Function *func = context->GetFunction(n->funccall.name);
                
                // TODO: Check if argument count and types are correct
                
                for(int i = 0; i < n->funccall.num_arguments; i++) {
                    GenerateExpr(context, n->funccall.arguments[i]);
                }
                
                Add(OP_CALL);
                Add(func->code_offset);
                Add(n->funccall.num_arguments);
                Add(OP_POP); // Since the value is not assigned anywhere
            } break;
        }
        
        n = n->next;
    }
    
}

void BytecodeBuilder::Generate(Context *context, Node *n) {
    switch(n->type) {
        
        case NODE_RETURN: {
            GenerateExpr(context, n->ret.expr);
            // When in global scope we halt the program
            // instead of a op_ret
            Add(OP_HALT);
            return;
        } break;
            
        case NODE_ASSIGNMENT: {
            // This may be a argument
            GenerateExpr(context, n->assignment.expr);
            
            int index = context->GetIndexOfGlobal(n->assignment.name);
            Add(OP_STORE);
            Add(index);
            return;
        } break;
        
        case NODE_FUNCCALL: {
            
            if(!context->FunctionExists(n->funccall.name)) {
                assert(!"Functions does not exist!");
            }
            Function *func = context->GetFunction(n->funccall.name);
                
            // TODO: Check if argument count and types are correct
            
            for(int i = 0; i < n->funccall.num_arguments; i++) {
                GenerateExpr(context, n->funccall.arguments[i]);
            }
                
            Add(OP_CALL);
            Add(func->code_offset);
            Add(n->funccall.num_arguments);
            Add(OP_POP); // Since the value is not assigned anywhere
            return;
        } break;
        
        default: {
            printf("bcbuilder: type %d\n", n->type);
            printf("BytecodeBuilder::Generate(Context*, Node*): Wops?\n");
        } break;
    }
}

void BytecodeBuilder::Generate(Context *context) {
    
    data_size = context->num_globals;
    
    for(int i = 0; i < context->num_functions; i++) {
        Function *func = context->functions[i];
        func->code_offset = this->code_size;
        
        // Generate bytecode for the function nodes
        GenerateFunction(context, func);
        
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
        Generate(context, n);
        n = n->next;
    }
    
    // Print 
}