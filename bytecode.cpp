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
    data = NULL;
    data_size = 0;
}

void BytecodeBuilder::Add(uint32_t val) {
    data_size++;
    data = (uint32_t*) realloc(data, sizeof(uint32_t)*data_size);
    data[data_size-1] = val;
}

void BytecodeBuilder::Generate(Node *ast) {
    
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
            
            case NODE_BINOP: {
                // Binop requires info about types
                // but i quess i will get that when i do
                // semantics and parse symbols, functions etc.
                // for now everything is an int
                switch(n->binop.type) {
                    case BINOP_ADD: {
                        Generate(n->binop.lhs);
                        Generate(n->binop.rhs);
                        Add(OP_IIADD);
                    } break;
                    
                    case BINOP_SUB: {
                        Generate(n->binop.lhs);
                        Generate(n->binop.rhs);
                        Add(OP_IISUB);
                    } break;
                    
                    case BINOP_MUL: {
                        Generate(n->binop.lhs);
                        Generate(n->binop.rhs);
                        Add(OP_IIMUL);
                    } break;
                    
                    case BINOP_DIV: {
                        Generate(n->binop.lhs);
                        Generate(n->binop.rhs);
                        Add(OP_IIDIV);
                    } break;
                }
                
                return;
                n = n->next;
            } break;
            
            case NODE_RETURN: {
                Generate(n->ret.expr);
                Add(OP_RET);
                return;
            } break;
        }
    }
    
}