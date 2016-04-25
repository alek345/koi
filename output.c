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
void *jump_table[31] = {};
goto jump_table_setup;
jump_table_done:
goto l15;
l0: stack[++sp] = stack[fp + -3];
l2: stack[++sp] = stack[fp + -3];
l4: cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal * cb.siVal; stack[++sp] = cc.iVal;
l5: data[0] = stack[sp--];
l7: stack[++sp] = data[0];
l9: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l10: data[0] = stack[sp--];
l12: stack[++sp] = data[0];
l14: retval = stack[sp--]; sp = fp; ip = stack[sp--]; fp = stack[sp--]; nargs = stack[sp--];sp -= nargs; stack[++sp] = retval; goto *jump_table[ip+1];
l15: stack[++sp] = 32;
l17: data[1] = stack[sp--];
l19: stack[++sp] = 16;
l21: stack[++sp] = 1; stack[++sp]= fp; stack[++sp]= 23;fp = sp; goto l0;
l24: sp--;
l25: stack[++sp] = data[1];
l27: stack[++sp] = data[1];
l29: cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal * cb.siVal; stack[++sp] = cc.iVal;
l30: goto end;
goto end;
jump_table_setup:
jump_table[0] = &&l0;
jump_table[2] = &&l2;
jump_table[4] = &&l4;
jump_table[5] = &&l5;
jump_table[7] = &&l7;
jump_table[9] = &&l9;
jump_table[10] = &&l10;
jump_table[12] = &&l12;
jump_table[14] = &&l14;
jump_table[15] = &&l15;
jump_table[17] = &&l17;
jump_table[19] = &&l19;
jump_table[21] = &&l21;
jump_table[24] = &&l24;
jump_table[25] = &&l25;
jump_table[27] = &&l27;
jump_table[29] = &&l29;
jump_table[30] = &&l30;
goto jump_table_done;
end: if(sp>=0) printf("ret: %d\n", stack[sp]);
}
