#ifndef LEXER_H
#define LEXER_H
#include "compiler.h"

TokenType check_is_keyword(char token[]);
Token get_token(char source[], int *pos);
char *token_type_converter(TokenType t);
TokenType check_is_bracket(char c);
TokenType lexer_classify_operation(char c);
int lexer_check_operation(char c);

#endif