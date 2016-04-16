#include "bytecode.h"
#include <stdio.h>
#include <stdlib.h>

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
    vm->stack[vm->sp] = (a + b);
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
    vm->stack[vm->sp] = (a - b);
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
    vm->stack[vm->sp] = (a * b);
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
    vm->stack[vm->sp] = (a / b);
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
};

int32_t VirtualMachine::Run(bool trace) {
    while((!halt) && ip < code_size) {
        uint32_t opcode = code[ip++];
        
        if(trace) {
            printf("%04X: %s", ip, op_to_string((Operand)opcode));
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