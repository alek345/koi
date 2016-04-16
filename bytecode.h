#ifndef BYTECODE_H
#define BYTECODE_H
#include <stdint.h>

enum Operand {
    OP_HALT = 0,
    OP_CONST, // Puts the next word on top of the stack
    OP_IIADD,
    OP_FFADD,
    OP_FIADD, // Result will be stored as a float
    OP_IFADD, // Result will be stored as a float
    OP_IISUB,
    OP_FFSUB,
    OP_FISUB,
    OP_IFSUB,
    OP_IIMUL,
    OP_FFMUL,
    OP_FIMUL,
    OP_IFMUL,
    OP_IIDIV,
    OP_FFDIV,
    OP_FIDIV,
    OP_IFDIV,
    OP_POP,
    OP_BR,
    OP_BRT,
    OP_BRF,
    OP_CALL,
    OP_RET,
};

union conversion {
    float floatVal;
    uint32_t intVal;
};

#define STACK_SIZE 4096

char *op_to_string(Operand op);

struct VirtualMachine {
    uint32_t *code;
    uint32_t *data;
    uint32_t *stack;
    uint32_t ip;
    int32_t sp;
    uint32_t fp;
    bool halt;
    
    uint32_t code_size;
    uint32_t data_size;
    
    VirtualMachine(uint32_t *code, uint32_t code_size, uint32_t data_size);
    
    // Retuns the value at the top of the stack at the end
    // of execution
    int32_t Run(bool trace);
};

#endif /* BYTECODE_H */