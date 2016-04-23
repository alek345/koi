#ifndef CBUILDER_H
#define CBUILDER_H
#include <stdio.h>
#include "bytecode.h"

struct CBuilder {
	uint32_t offset = 0;
	FILE *f;
	BytecodeBuilder *builder;

	CBuilder(BytecodeBuilder *builder);
	void Write(const char *path);
};

#endif /* CBUILDER_H */
