#ifndef PARSER_H
#define PARSER_H
#include "compiler.h"



void init_parser(char src[]);
int advance_token();
void print_current_token();
void match_token(TokenType expected_token_type);
ASTNode *parse_statement();
ASTNode *parse_additive();
ASTNode *parse_primary();
ASTNode *parse_expression();
ASTNode *parse_multiple_statements();
ASTNode *parse_bitwise_or();
ASTNode *parse_bitwise_and();
ASTNode *parse_shift();
ASTNode *parse_multiplicative();
int parser_check_is_comparison(TokenType type);
ASTNodeType parser_classify_comparison(TokenType type);
void parser_add_symbol(DataType dtt, char n[]);
int parser_find_symbol(char name[]);

#endif