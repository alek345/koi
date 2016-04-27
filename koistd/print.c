#include <stdio.h>
#include <stdint.h>
#include "macros.h"

FUNCTION(print_int) {
	uint32_t val = pop();
	printf("%d", val);
}

FUNCTION(print_hex) {
	uint32_t val = pop();
	printf("0x%04X", val);
}

FUNCTION(println) {
	printf("\n");
}
