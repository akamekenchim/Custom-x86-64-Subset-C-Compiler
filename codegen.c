#include <stdio.h>
#include <stdarg.h>
#include "codegen.h"
static FILE *asm_out = NULL;

// initialize the target file
int init_asm_emitter(const char *filename) {
    asm_out = fopen(filename, "w");
    if (!asm_out) {
        return 0; 
    }
    return 1; 
}

// variadic function to emit ASM x86 into target file (format similar to printf)
void asm_printf(const char *format, ...) {
    if (!asm_out) return;

    va_list args;
    va_start(args, format);
    
    // parses the va_list and formats it like printf
    vfprintf(asm_out, format, args);
    
    va_end(args);
    //fflush(asm_out);
}

// close the target file
void close_asm_emitter() {
    if (asm_out) {
        fclose(asm_out);
        asm_out = NULL;
    }
}