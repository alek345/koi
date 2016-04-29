#include <stdio.h>
#include <stdint.h>
#include "macros.h"

FUNCTION(print_int) {
	VARS();

	uint32_t val = pop();
	printf("%d", val);

	push(0);
}

FUNCTION(print_hex) {
	VARS();

	uint32_t val = pop();
	printf("0x%04X", val);

	push(0);
}

FUNCTION(print_char) {
	VARS();

	uint32_t val = pop();
	printf("%c", val);

	push(0);
}

FUNCTION(println) {
	VARS();

	printf("\n");

	push(0);
}
