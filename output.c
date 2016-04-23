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
#define ip_label(lab) l ## #lab
uint32_t data[2];
int main(int argc, char **argv) {
goto l0;
l0: stack[++sp] = 16;
l2: data[1] = stack[sp--];
l4: stack[++sp] = data[1];
l6: stack[++sp] = 16;
l8: cb.iVal = stack[sp--]; ca.iVal = stack[sp--]; cc.siVal = ca.siVal * cb.siVal; stack[++sp] = cc.iVal;
l9: goto end;
end: if(sp>=0) printf("ret: %d\n", stack[sp]);
}
