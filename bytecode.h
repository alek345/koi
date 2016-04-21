#ifndef BYTECODE_H
#define BYTECODE_H
#include <stdint.h>
#include "ast.h"
#include "context.h"

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
    OP_LOAD,
    OP_STORE,
    OP_GSTORE,
	OP_GLOAD,
};

// I don't know if this behavior is specified
union conversion {
    float floatVal;
    uint32_t intVal;
    int32_t sintVal;
};

#define STACK_SIZE 4096

char *op_to_string(Operand op);

struct BytecodeBuilder {
    uint32_t *code;
    uint32_t code_size;
    uint32_t data_size;
    uint32_t start_ip;
    
    // Let the bytecode builder control how much data
    // memory to allocate. Check every GSTORE or GLOAD and find the
    // highest index. This is the amount of data memory required
    
    // Find out some way for the bytecode builder to know
    // where functions are located and what theire names are
    // Could add all pass all function start and a name to the builder
    // which in turn calculates the address of the function
    // based on a name given
    
    BytecodeBuilder();
    
    void Add(uint32_t val);
    
    void Write(const char *path);
    
    void GenerateExpr(Context *context, Node *ast);
    void Generate(Context *context, Node *node);
    void Generate(Context *context);
    void GenerateFunction(Context *context, Function *function);
};

struct VirtualMachine {
    uint32_t *code;
    uint32_t *data;
    uint32_t *stack;
    uint32_t ip;
    int32_t sp;
    int32_t fp;
    bool halt;
    
    uint32_t code_size;
    uint32_t data_size;
    
    VirtualMachine(uint32_t *code, uint32_t code_size, uint32_t data_size);
    VirtualMachine(BytecodeBuilder *builder);
	
	// Read in and load a .koic file.
	// VirtualMachine(const char *path);
    
	void PrintData();

    // Retuns the value at the top of the stack at the end
    // of execution
    int32_t Run(bool trace);

	// Functions for easy VirtualMachine interaction
	// void Push(uint32_t val);
	// uint32_t Pop();
};

#endif /* BYTECODE_H */