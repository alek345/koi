#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "include/bytecode.h"

uint32_t read_u32(uint8_t **buf) {
	uint8_t *buffer = *buf;
	uint8_t a = *buffer;
	(*buf)++;
	buffer++;
	uint8_t b = *buffer;
	(*buf)++;
	buffer++;
	uint8_t c = *buffer;
	(*buf)++;
	buffer++;
	uint8_t d = *buffer;
	(*buf)++;
	buffer++;

	return (a<<24)|(b<<16)|(c<<8)|(d);
}

void fancy_print_code(uint32_t *code, uint32_t code_size) {

	for(int i = 0; i < code_size;) {
	
		switch((Operand)code[i]) {
			case OP_HALT:
				printf("HALT\n");
				i++;
				break;
			case OP_CONST:
				printf("CONST: ");
				i++;
				printf("%d\n", code[i++]);
				break;
				
			case OP_IIADD:
				printf("IIADD\n");
				i++;
				break;
			case OP_FFADD:
				printf("FFADD\n");
				i++;
				break;
			case OP_FIADD:
				printf("FIADD\n");
				i++;
				break;
			case OP_IFADD:
				printf("IFADD\n");
				i++;
				break;

			case OP_IISUB:
				printf("IISUB\n");
				i++;
				break;
			case OP_FFSUB:
				printf("FFSUB\n");
				i++;
				break;
			case OP_FISUB:
				printf("FISUB\n");
				i++;
				break;
			case OP_IFSUB:
				printf("IFSUB\n");
				i++;
				break;

			case OP_IIMUL:
				printf("IIMUL\n");
				i++;
				break;
			case OP_FFMUL:
				printf("FFMUL\n");
				i++;
				break;
			case OP_FIMUL:
				printf("FIMUL\n");
				i++;
				break;
			case OP_IFMUL:
				printf("IFMUL\n");
				i++;
				break;

			case OP_IIDIV:
				printf("IIDIV\n");
				i++;
				break;
			case OP_FFDIV:
				printf("FFDIV\n");
				i++;
				break;
			case OP_FIDIV:
				printf("FIDIV\n");
				i++;
				break;
			case OP_IFDIV:
				printf("IFDIV\n");
				i++;
				break;

			case OP_POP:
				printf("POP\n");
				i++;
				break;
			case OP_BR:
				printf("BR: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_BRT:
				printf("BRT: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_BRF:
				printf("BRF: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_CALL:
				printf("CALL: ");
				i++;
				printf("addr: %d, nargs %d\n", code[i++],code[i++]);
				break;
			case OP_RET:
				printf("RET\n");
				i++;
				break;
			case OP_LOAD:
				printf("LOAD: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_STORE:
				printf("STORE: ");
				i++;
				printf("%d\n", code[i++]);
				break;	
			case OP_GLOAD:
				printf("GLOAD: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_GSTORE:
				printf("GSTORE: ");
				i++;
				printf("%d\n", code[i++]);
				break;
			case OP_CFUNC:
				printf("CFUNC: ");
				i++;
				char *name = NULL;
				int len = 0;
				while(code[i]) {
					len++;
					name = (char*) realloc(name, sizeof(char)*len);
					name[len-1] = code[i++];
				}
				len++;
				name = (char*) realloc(name, sizeof(char)*len);
				name[len-1] = 0;
				i++;
				printf("%s, nargs: %d\n", name, code[i++]);
				free(name);

				break;
		}

	}

}

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: readkoic <koic>\n");
		return -1;
	}

	FILE *f = fopen(argv[1], "rb");
	if(!f) {
		printf("Failed to open '%s'\n", argv[1]);
	}

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	rewind(f);

	uint8_t *buffer = (uint8_t*) malloc(sizeof(uint8_t)*len);
	fread(buffer, sizeof(uint8_t), len, f);
	fclose(f);

	char magic[5] = "KOIC";
	char header[5];
	header[0] = *buffer;
	buffer++;
	header[1] = *buffer;
	buffer++;
	header[2] = *buffer;
	buffer++;
	header[3] = *buffer;
	buffer++;
	header[4] = 0;

	if(strcmp(magic, header) != 0) {
		printf("Magic number failed to match!\n");
		return -1;
	}

	printf("Magic number matched\n");

	uint32_t start_ip = read_u32(&buffer);
	printf("start_ip: %d\n", start_ip);

	uint32_t code_size = read_u32(&buffer);
	printf("code_size: %d\n", code_size);

	uint32_t data_size = read_u32(&buffer);
	printf("data_size: %d\n", data_size);

	uint32_t *code = (uint32_t*) malloc(sizeof(uint32_t) * code_size);

	for(int i = 0; i < code_size; i++) {
		code[i] = read_u32(&buffer);
	}

	fancy_print_code(code, code_size);
}
