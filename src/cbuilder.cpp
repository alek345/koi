#include "cbuilder.h"
#include <assert.h>
#include <stdlib.h>

CBuilder::CBuilder(BytecodeBuilder *builder) {
	this->builder = builder;
}

void halt(CBuilder *b) {
	fprintf(b->f, "goto end;\n");
}

void _const(CBuilder *b) {
	fprintf(b->f, "stack[++sp] = %d;\n", b->builder->code[b->offset++]);
}

// Add
void iiadd(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal + cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ffadd(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal + cb.fVal; stack[++sp] = cc.iVal;\n");
}

void fiadd(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal + cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ifadd(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.siVal + cb.fVal; stack[++sp]= cc.iVal;\n");
}

// Sub
void iisub(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal - cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ffsub(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal - cb.fVal; stack[++sp] = cc.iVal;\n");
}

void fisub(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal - cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ifsub(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.siVal - cb.fVal; stack[++sp]= cc.iVal;\n");
}

// Mul
void iimul(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal * cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ffmul(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal * cb.fVal; stack[++sp] = cc.iVal;\n");
}

void fimul(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal * cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ifmul(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.siVal * cb.fVal; stack[++sp]= cc.iVal;\n");
}

// Div
void iidiv(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal / cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ffdiv(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal / cb.fVal; stack[++sp] = cc.iVal;\n");
}

void fidiv(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.fVal / cb.siVal; stack[++sp] = cc.iVal;\n");
}

void ifdiv(CBuilder *b) {
	fprintf(b->f, "cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.fVal = ca.siVal / cb.fVal; stack[++sp]= cc.iVal;\n");
}

void pop(CBuilder *b) {
	fprintf(b->f, "sp--;\n");
}

void br(CBuilder *b) {
	fprintf(b->f, "goto l%d;\n", b->builder->code[b->offset++]);
}

void brt(CBuilder *b) {
	fprintf(b->f, "if(stack[sp--] != 0) { goto l%d; }\n", b->builder->code[b->offset++]);
}

void brf(CBuilder *b) {
	fprintf(b->f, "if(stack[sp--] == 0) { goto l%d; }\n", b->builder->code[b->offset++]);
}

void call(CBuilder *b) {
	uint32_t addr = b->builder->code[b->offset++];
	fprintf(b->f, "stack[++sp] = %d; stack[++sp]= fp; stack[++sp]= %d;",
			b->builder->code[b->offset++], b->offset);
	fprintf(b->f, "fp = sp; goto l%d;\n", addr);
}

void ret(CBuilder *b) {
	fprintf(b->f, "retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];");
	fprintf(b->f, "sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];\n");
}

void load(CBuilder *b) {
	fprintf(b->f, "stack[++sp] = stack[fp + %d];\n", (int32_t)b->builder->code[b->offset++]);
}

void store(CBuilder *b) {
	fprintf(b->f, "stack[fp + %d] = stack[sp--];\n", (int32_t)b->builder->code[b->offset++]);
}

void gstore(CBuilder *b) {
	fprintf(b->f, "data[%d] = stack[sp--];\n", b->builder->code[b->offset++]);
}

void gload(CBuilder *b) {
	fprintf(b->f, "stack[++sp] = data[%d];\n", b->builder->code[b->offset++]);
}

typedef void(*cFunc)(CBuilder*);

static cFunc ops[] = {
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

	br, brt, brf,
	call, ret,
	load, store,
	gstore, gload,
};

const char *boiler_top =
"#include <stdlib.h>\n"
"#include <stdint.h>\n"
"#include <stdio.h>\n"
"#include <string.h>\n"
"uint32_t stack[4096];\n"
"int32_t sp = -1;\n"
"int32_t fp = -1;\n"
"int32_t a, b;\n"
"float fa, fb;\n"
"union conv { int32_t siVal; uint32_t iVal; float fVal; } ca, cb, cc;\n"
"int retval;\n"
"uint32_t ip;\n"
"int nargs;\n"
;

void CBuilder::Write(const char *path) {

	f = fopen(path, "wb");
	if(!f) {
		assert(!"Failed to open file!");
	}

	fprintf(f, "%s", boiler_top);

	fprintf(f, "uint32_t data[%d];\n", builder->data_size);

	fprintf(f, "int main(int argc, char **argv) {\n");
	
	fprintf(f, "void *jump_table[%d] = {};\n", builder->code_size);
	fprintf(f, "goto jump_table_setup;\n");
	fprintf(f, "jump_table_done:\n");

	fprintf(f, "goto l%d;\n", builder->start_ip);

	int num_labels = 0;
	int *labels = NULL;

	uint32_t opcode = builder->code[0];
	for(offset = 0; offset < builder->code_size;) {
		fprintf(f, "l%d: ", offset);

		num_labels++;
		labels = (int*) realloc(labels, sizeof(int)*num_labels);
		labels[num_labels-1]= offset;

		offset++;

		printf("code[%d]= %d;\n", offset, builder->code[offset]);
		printf("op: %s\n", op_to_string((Operand)opcode));
		ops[opcode](this);

		opcode = builder->code[offset];
	}
	
	fprintf(f, "goto end;\n");
	fprintf(f, "jump_table_setup:\n");

	for(int i = 0; i < num_labels; i++) {
		fprintf(f, "jump_table[%d] = &&l%d;\n", labels[i], labels[i]);
	}

	fprintf(f, "goto jump_table_done;\n");

	fprintf(f, "end: if(sp>=0) printf(\"ret: %%d\\n\", stack[sp]);\n");
	fprintf(f, "}\n");

	fflush(f);
	fclose(f);
}
