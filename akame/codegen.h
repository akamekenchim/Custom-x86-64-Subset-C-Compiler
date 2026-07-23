#ifndef CODEGEN_H
#define CODEGEN_H
int init_asm_emitter(const char *filename);
void asm_printf(const char *format, ...);
void close_asm_emitter();
#endif