#ifndef CONTEXT_H
#define CONTEXT_H
#include "ast.h"
#include <stdint.h>

// All Variables should be zero-initalized
struct Variable {
    Node *node;
};

// TODO: Save arguments, types and a function that
// will figure out if a variable exists, is a local variable
// global variable or argument. Probably make a simple
// GetVariable(char *name) that figures this out
struct Function {
    Node *node;
    Variable **local_variables;
    int num_locals;
    
    // Used by the BytecodeBuilder
    uint32_t code_offset;
    
    bool Declared(char *name);
    int GetIndexOfLocal(char *name);
};

struct Context {
    Variable **global_variables;
    int num_globals;
    bool GlobalDeclared(char *name);
    int GetIndexOfGlobal(char *name);
    
    Function **functions;
    int num_functions;
    bool FunctionExists(char *name);
    Function* GetFunction(char *name);
    
    Node *stmts;
    
	Type GetExprType(Node *expr);

    void TypeCheck(Node *nodes);
    void Analyse(Node *n);
};

#endif /* CONTEXT_H */
