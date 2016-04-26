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
void *jump_table[44] = {};
goto jump_table_setup;
jump_table_done:
goto l31;
l0: stack[++sp] = stack[fp + -3];
l2: if(stack[sp--] == 0) { goto l15; }
l4: stack[++sp] = 256;
l6: data[1] = stack[sp--];
l8: stack[++sp] = data[1];
l10: data[0] = stack[sp--];
l12: stack[++sp] = data[0];
l14: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l15: stack[++sp] = 1337;
l17: data[1] = stack[sp--];
l19: stack[++sp] = data[1];
l21: data[0] = stack[sp--];
l23: stack[++sp] = data[0];
l25: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l26: data[0] = stack[sp--];
l28: stack[++sp] = data[0];
l30: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l31: stack[++sp] = 1;
l33: data[1] = stack[sp--];
l35: stack[++sp] = 0;
l37: stack[++sp] = 1; stack[++sp]= fp; stack[++sp]= 39;fp = sp; goto l0;
l40: sp--;
l41: stack[++sp] = data[1];
l43: goto end;
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
jump_table[15] = &&l15;
jump_table[17] = &&l17;
jump_table[19] = &&l19;
jump_table[21] = &&l21;
jump_table[23] = &&l23;
jump_table[25] = &&l25;
jump_table[26] = &&l26;
jump_table[28] = &&l28;
jump_table[30] = &&l30;
jump_table[31] = &&l31;
jump_table[33] = &&l33;
jump_table[35] = &&l35;
jump_table[37] = &&l37;
jump_table[40] = &&l40;
jump_table[41] = &&l41;
jump_table[43] = &&l43;
goto jump_table_done;
end: if(sp>=0) printf("ret: %d\n", stack[sp]);
}
