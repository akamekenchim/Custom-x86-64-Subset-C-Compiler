#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEXEME_LENGTH 256
#define MAX_NAME_NODE_LENGTH 48


typedef struct V{
    char name[20];
    int offset;
}Variable;

typedef enum {
    // Program control nodes
    NODE_PROGRAM, 
    NODE_BLOCK,
    NODE_SEQ,

    // Function management
    NODE_FUNC_DEFINE,
    NODE_FUNC_PARAM_LIST,
    NODE_FUNC_CALL,
    NODE_RETURN,

    // Control flow nodes
    NODE_IF,
    NODE_IF_BODY,
    NODE_WHILE,
    NODE_FOR,
    NODE_FOR_HEADER, // declaration, condition, increment
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_PRINT,

    // assignment and literals, variables
    NODE_ASSIGN,
    NODE_VARIABLE,
    NODE_LITERAL,
    NODE_STRING,

    // arithmetic operation nodes
    NODE_ADD,
    NODE_SUB,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_REMAINDER,
    NODE_UNARY_MINUS,

    // comparison nodes
    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_LESS_THAN,
    NODE_LESS_EQUAL,
    NODE_GREATER_THAN,
    NODE_GREATER_EQUAL,

    // arithmetic bitwise shifting nodes and logical nodes
    NODE_BITWISE_AND,
    NODE_BITWISE_OR,
    NODE_BITWISE_SHIFT_LEFT,
    NODE_BITWISE_SHIFT_RIGHT,

    // pointers and memory management
    NODE_ADDR,
    NODE_DEREF,
    NODE_ARRAY_ACCESS
} ASTNodeType;

typedef enum {
    // keywords
    TOKEN_INT_KEYWORD,
    TOKEN_CHAR_KEYWORD,
    TOKEN_VOID_KEYWORD,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR_KEYWORD,
    TOKEN_BREAK_KEYWORD,
    TOKEN_CONTINUE_KEYWORD,
    TOKEN_RETURN,
    TOKEN_PRINT,

    // identifiers and literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR_LITERAL,

    // (Assignment & Arithmetic Operators)
    TOKEN_ASSIGN,
    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_REMAINDER,

    // (Relational Operators)
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_EQUAL,

    // (Bitwise Operators)
    TOKEN_BITWISE_AND,
    TOKEN_BITWISE_OR,
    TOKEN_BITWISE_SHIFT_LEFT,
    TOKEN_BITWISE_SHIFT_RIGHT,

    // (Pointer & Memory Operators)
    TOKEN_POINTER,
    TOKEN_GET_ADDRESS,

    // Delimiters & Punctuators
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,

    // end of file token
    TOKEN_EOF
} TokenType;

typedef enum {
    TYPE_UNKNOWN,
    // int, char, void
    TYPE_INT,       // 8 bytes
    TYPE_CHAR,      // 1 byte
    TYPE_VOID,
    // pointers and arrays
    TYPE_INT_PTR,
    TYPE_CHAR_PTR,
    TYPE_INT_ARRAY,
    TYPE_CHAR_ARRAY
} DataType;

typedef struct {
    TokenType type;    
    char lexeme[MAX_LEXEME_LENGTH];   // original stream source
    int value;         // literal value 
} Token;

// 2. Cấu trúc của một Nút trên cây
typedef struct ASTNode {
    ASTNodeType type;   
    DataType data_type;     
    char name[MAX_NAME_NODE_LENGTH];             
    int int_value;             
    char *string_value;

    int element_count; // array management

    struct ASTNode *left;     
    struct ASTNode *right;    

} ASTNode;

typedef struct S{
    char symbol_name[MAX_NAME_NODE_LENGTH];
    DataType data_type;
    int size_bytes;
} Symbol;

ASTNode *makeNew_String(char *n);
ASTNode *makeNew_Variable(char n[], DataType dt);
ASTNode *makeNew_Literal(int value);
ASTNode *makeNew_Assign(ASTNode *var, ASTNode *literal);
ASTNode *makeNew_Operation(ASTNode *literal1, ASTNode *literal2, ASTNodeType type);
void inOrder(ASTNode *root);
void FreeAll(ASTNode *root);
void Assembly_Generator(ASTNode *root);
int find_variable(char n[], int size);
void print_header();
void print_end_assembly();
void print_header_to_asm();
void print_end_assembly_to_asm();
void printNode(ASTNode *root);
int check_is_operation(ASTNodeType type);
char *function_for_operation(ASTNodeType type);
int compiler_check_is_comparison_node(ASTNodeType type);
char *compiler_comparison_classification(ASTNodeType type);
int get_aligned_stack_size();
void reset_var_table();
void scan_local_variables(ASTNode *node);
void push_loop(int label_id, int type);
void pop_loop();

#endif