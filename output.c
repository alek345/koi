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
void *jump_table[37] = {};
goto jump_table_setup;
jump_table_done:
goto l24;
l0: stack[++sp] = stack[fp + -3];
l2: if(stack[sp--] == 0) { goto l8; }
l4: stack[++sp] = 256;
l6: data[1] = stack[sp--];
l8: stack[++sp] = 1337;
l10: stack[fp + -1] = stack[sp--];
l12: stack[++sp] = data[1];
l14: data[0] = stack[sp--];
l16: stack[++sp] = data[0];
l18: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l19: data[0] = stack[sp--];
l21: stack[++sp] = data[0];
l23: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l24: stack[++sp] = 1;
l26: data[1] = stack[sp--];
l28: stack[++sp] = 1;
l30: stack[++sp] = 1; stack[++sp]= fp; stack[++sp]= 32;fp = sp; goto l0;
l33: sp--;
l34: stack[++sp] = data[1];
l36: goto end;
goto end;
jump_table_setup:
jump_table[0] = &&l0;
jump_table[2] = &&l2;
jump_table[4] = &&l4;
jump_table[6] = &&l6;
jump_table[8] = &&l8;
jump_table[10] = &&l10;
jump_table[12] = &&l12;
jump_table[14] = &&l14;
jump_table[16] = &&l16;
jump_table[18] = &&l18;
jump_table[19] = &&l19;
jump_table[21] = &&l21;
jump_table[23] = &&l23;
jump_table[24] = &&l24;
jump_table[26] = &&l26;
jump_table[28] = &&l28;
jump_table[30] = &&l30;
jump_table[33] = &&l33;
jump_table[34] = &&l34;
jump_table[36] = &&l36;
goto jump_table_done;
end: if(sp>=0) printf("ret: %d\n", stack[sp]);
}
