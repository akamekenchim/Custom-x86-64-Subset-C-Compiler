#ifndef PARSER_H
#define PARSER_H
#include "compiler.h"

// core functions for parser
void init_parser(char src[]);
int advance_token();
void print_current_token();
void match_token(TokenType expected_token_type);

// abstract syntax tree (ast) construction
ASTNode *parse_program();
ASTNode *parse_function_definition();
ASTNode *parse_statement();
ASTNode *parse_multiple_statements();
ASTNode *parse_expression(); // =, >=, <=, ==, <, >
ASTNode *parse_bitwise_or(); // |
ASTNode *parse_bitwise_and(); // &
ASTNode *parse_shift(); // << >>
ASTNode *parse_additive(); // + -
ASTNode *parse_multiplicative(); // * / %
ASTNode *parse_unary(); // *ptr, &ptr
ASTNode *parse_primary(); 

// utilities
int parser_check_is_comparison(TokenType type);
ASTNodeType parser_classify_comparison(TokenType type);
void parser_add_symbol(DataType dtt, char n[], int size);
int parser_find_symbol(char name[]);

#endif