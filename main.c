#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
ASTNode *root = NULL;
int main(int argc, char *argv[]) {
    // check command line arguments
    if (argc < 2) {
        fprintf(stderr, "Error: No input file specified.\n");
        fprintf(stderr, "Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    // open the file in binary reading mode to calculate length
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("File opening Error");
        return 1;
    }

    // jumps to end of file -> ftell returns the position of the last byte -> jumps back to start of file
    fseek(file, 0, SEEK_END);      
    long file_size = ftell(file);  
    fseek(file, 0, SEEK_SET);      

    //dynamically allocating memory for source code
    char *source = (char *)malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Error in allocating memory for the source file.\n");
        fclose(file);
        return 1;
    }

    // use fread to read all bytes of the file into source string then closes file
    size_t bytes_read = fread(source, 1, file_size, file);
    source[bytes_read] = '\0';     
    fclose(file);                  

    // initialize the file in writing mode
    if (!init_asm_emitter("target.asm")) {
        perror("Error in opening the target.asm");
        free(source);
        return 1;
    }

    // init the parser
    init_parser(source);
    root = parse_program();
    
    // prints the AST into terminal for debugging
    inOrder(root);
    printf("\n");

    // emits ASM x86 into target.asm
    print_header_to_asm();
    Assembly_Generator(root);
    print_end_assembly_to_asm();

    //safely frees the heap memory
    FreeAll(root);
    close_asm_emitter();
    free(source);                 

    return 0;
}