#ifndef KOISTD_MACROS_H
#define KOISTD_MACROS_H

#define FUNCTION(name) void name (uint32_t *stack, uint32_t *sp_ptr)
#define VARS() int32_t sp = *sp_ptr;

#define push(val) stack[sp++] = val
#define pop() stack[sp--]

#endif /* KOISTD_MACROS_H */
