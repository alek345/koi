#ifndef LLVMBUILDER_H
#define LLVMBUILDER_H
#include "context.h"

struct LLVMBuilder {
    Context *context;
    
    LLVMBuilder(Context *context);
    void Generate(const char *path);
};

#endif /* LLVMBUILDER_H */