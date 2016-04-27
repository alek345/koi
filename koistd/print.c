#include <stdio.h>
#include <stdint.h>
#include "macros.h"

void print_int(uint32_t *stack, uint32_t *sp) {
	uint32_t val = pop();
	printf("%d", val);
}

void println(uint32_t *stack, uint32_t *sp) {
	printf("\n");
}
