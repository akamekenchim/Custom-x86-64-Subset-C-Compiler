#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct V{
    char name[20];
    int offset;
}Variable;

typedef enum {
    NODE_ASSIGN,     // Nút đại diện cho phép gán '='
    NODE_VARIABLE,   // Nút đại diện cho biến (ví dụ: x)
    NODE_LITERAL,     // Nút đại diện cho hằng số (ví dụ: 42)
    NODE_ADD,
    NODE_SUB,
    NODE_IF,
    NODE_EQUAL,
    NODE_IF_BODY,
    NODE_LESS_THAN,
    NODE_BLOCK,
    NODE_PRINT,
    NODE_WHILE,
    NODE_SEQ,
    NODE_STRING,
    NODE_BITWISE_OR,
    NODE_BITWISE_AND,
    NODE_REMAINDER,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_BITWISE_SHIFT_LEFT,
    NODE_BITWISE_SHIFT_RIGHT,
    NODE_LESS_EQUAL,
    NODE_GREATER_EQUAL,
    NODE_GREATER_THAN,
    NODE_NOT_EQUAL
} ASTNodeType;

typedef enum {
    TOKEN_INT_KEYWORD, // Từ khóa 'int'
    TOKEN_IDENTIFIER,  // Tên biến (ví dụ: x, y, sum)
    TOKEN_ASSIGN,      // Toán tử gán '='
    TOKEN_NUMBER,      // Con số (ví dụ: 42, 100)
    TOKEN_ADD, 
    TOKEN_SUB,
    TOKEN_EOF,       // Ký hiệu kết thúc file (End Of File)
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_RBRACE,
    TOKEN_LBRACE,
    TOKEN_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_PRINT,
    TOKEN_WHILE,
    TOKEN_SEMICOLON,
    TOKEN_STRING,
    TOKEN_BITWISE_OR,
    TOKEN_BITWISE_AND,
    TOKEN_REMAINDER,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_BITWISE_SHIFT_LEFT,
    TOKEN_BITWISE_SHIFT_RIGHT,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_NOT_EQUAL
} TokenType;

typedef enum {
    TYPE_UNKNOWN,
    TYPE_INT,       // 8 bytes (hoặc 4 bytes)
    TYPE_CHAR,      // 1 byte
    TYPE_VOID
} DataType;

// Cấu trúc đóng gói thông tin của một Token
typedef struct {
    TokenType type;    // Loại Token
    char lexeme[64];   // Chuỗi ký tự gốc (ví dụ: "sum", "42")
    int value;         // Giá trị thực tế (chỉ dùng nếu type là TOKEN_NUMBER)
} Token;

// 2. Cấu trúc của một Nút trên cây
typedef struct ASTNode {
    ASTNodeType type;          // Loại nút để biết cách xử lý
    DataType data_type;         // Kiểu dữ liệu của nút (nếu cần)
    char name[64];             // Dùng để lưu tên biến (nếu là NODE_VARIABLE)
    int int_value;             // Dùng để lưu giá trị số (nếu là NODE_LITERAL)
    char *string_value;
    struct ASTNode *left;      // Con trỏ chỉ tới nút con bên trái
    struct ASTNode *right;     // Con trỏ chỉ tới nút con bên phải

} ASTNode;

typedef struct S{
    char symbol_name[25];
    DataType data_type;
} Symbol;

ASTNode *makeNew_String(char n[]);
ASTNode *makeNew_Variable(char n[], DataType dt);
ASTNode *makeNew_Literal(int value);
ASTNode *makeNew_Assign(ASTNode *var, ASTNode *literal);
ASTNode *makeNew_Operation(ASTNode *literal1, ASTNode *literal2, ASTNodeType type);
void inOrder(ASTNode *root);
void FreeAll(ASTNode *root);
void Assembly_Generator(ASTNode *root);
int find_variable(char n[]);
void print_header();
void print_end_assembly();
void print_header_to_asm();
void print_end_assembly_to_asm();
void printNode(ASTNode *root);
int check_is_operation(ASTNodeType type);
char *function_for_operation(ASTNodeType type);
int compiler_check_is_comparison_node(ASTNodeType type);
char *compiler_comparison_classification(ASTNodeType type);

#endif