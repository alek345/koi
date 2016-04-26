#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
uint32_t stack[4096];
int32_t sp = -1;
int32_t fp = -1;
int32_t a, b;
float fa, fb;
union conv { int32_t siVal; uint32_t iVal; float fVal; } ca, cb, cc;
int retval;
uint32_t ip;
int nargs;
uint32_t data[2];
int main(int argc, char **argv) {
void *jump_table[20] = {};
goto jump_table_setup;
jump_table_done:
goto l0;
l0: stack[++sp] = 1;
l2: data[1] = stack[sp--];
l4: stack[++sp] = data[1];
l6: if(stack[sp--] == 0) { goto l14; }
l8: stack[++sp] = data[1];
l10: stack[++sp] = 1;
l12: cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal + cb.siVal; stack[++sp] = cc.iVal;
l13: goto end;
l14: stack[++sp] = data[1];
l16: stack[++sp] = 1;
l18: cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal - cb.siVal; stack[++sp] = cc.iVal;
l19: goto end;
goto end;
jump_table_setup:
jump_table[0] = &&l0;
jump_table[2] = &&l2;
jump_table[4] = &&l4;
jump_table[6] = &&l6;
jump_table[8] = &&l8;
jump_table[10] = &&l10;
jump_table[12] = &&l12;
jump_table[13] = &&l13;
jump_table[14] = &&l14;
jump_table[16] = &&l16;
jump_table[18] = &&l18;
jump_table[19] = &&l19;
goto jump_table_done;
end: if(sp>=0) printf("ret: %d\n", stack[sp]);
}
