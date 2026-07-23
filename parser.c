#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"

Token curToken;
static int position = 0;
static char source[5000];
Symbol symbol_table[1000];
static int symbol_count = 0;

void init_parser(char src[]){
    position = 0;
    strcpy(source, src);
    advance_token();
}
void print_current_token(){
    printf("Lexeme: %s\n", curToken.lexeme);
}
int advance_token(){
    curToken = get_token(source, &position);
    print_current_token();
    if(curToken.type == TOKEN_EOF) return 0;
    return 1;
}
void match_token(TokenType expected_token_type){
    if(curToken.type == expected_token_type){
        advance_token();
    }
    else{
        printf("There's an error in your Syntax\n");
        exit(1);
    }
}
ASTNode *parse_multiple_statements(){
    while(curToken.type == TOKEN_SEMICOLON){
        match_token(TOKEN_SEMICOLON);
    }
    if(curToken.type == TOKEN_RBRACE || curToken.type == TOKEN_EOF){
        return NULL;
    }
    ASTNode *first_statement = parse_statement();
    if(curToken.type == TOKEN_SEMICOLON){
        match_token(TOKEN_SEMICOLON);
    }
    if(curToken.type != TOKEN_RBRACE && curToken.type != TOKEN_EOF){
        ASTNode *next_statement = parse_multiple_statements();
        return makeNew_Operation(first_statement, next_statement, NODE_SEQ);
    }
    return first_statement;
}
ASTNode *parse_statement(){
    if (curToken.type == TOKEN_LBRACE) {
        match_token(TOKEN_LBRACE);
        ASTNode *block_content = parse_multiple_statements();
        match_token(TOKEN_RBRACE);
        return block_content; 
    }
    // xu li token dau la int
    if(curToken.type == TOKEN_INT_KEYWORD){
        match_token(TOKEN_INT_KEYWORD);
        char var_name[25];
        strcpy(var_name, curToken.lexeme);
        
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(var_name, symbol_table[i].symbol_name) == 0){
                printf("Symbol already defined, semantic error\n");
                exit(1);
            }
        }

        parser_add_symbol(TYPE_INT, var_name);

        match_token(TOKEN_IDENTIFIER);
        
        if(curToken.type == TOKEN_ASSIGN){
            match_token(TOKEN_ASSIGN);

            ASTNode *left_part = makeNew_Variable(var_name, TYPE_INT);
            ASTNode *right_tree = parse_expression();

            return makeNew_Assign(left_part, right_tree);
        }
        
        return makeNew_Variable(var_name, TYPE_INT);
    }
    // xu li token dau la bien
    if(curToken.type == TOKEN_IDENTIFIER){
        //match_token(TOKEN_INT_KEYWORD);
        char var_name[25];
        strcpy(var_name, curToken.lexeme);

        int found = 0;
        DataType dt;
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(symbol_table[i].symbol_name, curToken.lexeme)==0){
                found = 1;
                dt = symbol_table[i].data_type;
            }
        }
        if(found == 0){
            printf("The variable %s is not defined yet!\n", curToken.lexeme);
            exit(1);
        }

        match_token(TOKEN_IDENTIFIER);
        
        match_token(TOKEN_ASSIGN);

        ASTNode *left_part = makeNew_Variable(var_name, dt);
        ASTNode *right_tree = parse_expression();

        return makeNew_Assign(left_part, right_tree);

    }
    // xu li token dau la 'if'  
    if(curToken.type == TOKEN_IF){
        match_token(TOKEN_IF);
        match_token(TOKEN_LPAREN);
        ASTNode *left_condition = parse_expression();
        match_token(TOKEN_RPAREN);
        match_token(TOKEN_LBRACE);
        ASTNode *then_branch = parse_multiple_statements();
        match_token(TOKEN_RBRACE);
        ASTNode *else_branch = NULL;
        if(curToken.type == TOKEN_ELSE){
            match_token(TOKEN_ELSE);
            match_token(TOKEN_LBRACE);
            else_branch = parse_multiple_statements();
            match_token(TOKEN_RBRACE);
        }
        ASTNode *if_statement_body = makeNew_Operation(then_branch, else_branch, NODE_IF_BODY);
        ASTNode *if_statement_tree = makeNew_Operation(left_condition, if_statement_body, NODE_IF);
        return if_statement_tree;
    }
    // xu li token dau la 'printf'
    if(curToken.type == TOKEN_PRINT){
        match_token(TOKEN_PRINT);
        match_token(TOKEN_LPAREN);
        ASTNode *toPrint = parse_expression();
        match_token(TOKEN_RPAREN);
        return makeNew_Operation(toPrint, NULL, NODE_PRINT);
    }
    // xu li token dau la 'while'
    if(curToken.type == TOKEN_WHILE){
        match_token(TOKEN_WHILE);
        match_token(TOKEN_LPAREN);
        ASTNode *condition_branch = parse_expression();
        match_token(TOKEN_RPAREN);
        match_token(TOKEN_LBRACE);
        ASTNode *statement_branch = parse_multiple_statements();
        match_token(TOKEN_RBRACE);
        return makeNew_Operation(condition_branch, statement_branch, NODE_WHILE);
    }
    return NULL;
}
ASTNode *parse_expression(){
    ASTNode *left_part = parse_bitwise_or();
    if(parser_check_is_comparison(curToken.type)){
        TokenType curType = curToken.type;
        match_token(curType);
        ASTNode *right_part = parse_bitwise_or();
        ASTNodeType node_type = parser_classify_comparison(curType); 
        left_part = makeNew_Operation(left_part, right_part, node_type);
    }
    return left_part;
}
ASTNode *parse_bitwise_or(){
    ASTNode *left_part = parse_bitwise_and();
    while(curToken.type == TOKEN_BITWISE_OR){
        TokenType curType = curToken.type;
        match_token(curType);
        ASTNode *right_part = parse_bitwise_and();
        ASTNodeType node_type = NODE_BITWISE_OR; 
        left_part = makeNew_Operation(left_part, right_part, node_type);
    }
    return left_part;
}
ASTNode *parse_bitwise_and(){
    ASTNode *left_part = parse_shift();
    while(curToken.type == TOKEN_BITWISE_AND){
        TokenType curType = curToken.type;
        match_token(curType);
        ASTNode *right_part = parse_shift();
        ASTNodeType node_type = NODE_BITWISE_AND; 
        left_part = makeNew_Operation(left_part, right_part, node_type);
    }
    return left_part;
}
ASTNode *parse_shift(){
    ASTNode *left_part = parse_additive();
    while(curToken.type == TOKEN_BITWISE_SHIFT_LEFT || curToken.type == TOKEN_BITWISE_SHIFT_RIGHT){
        TokenType curType = curToken.type;
        match_token(curType);
        ASTNode *right_part = parse_additive();
        ASTNodeType node_type = (curType == TOKEN_BITWISE_SHIFT_LEFT) ? NODE_BITWISE_SHIFT_LEFT : NODE_BITWISE_SHIFT_RIGHT;
        left_part = makeNew_Operation(left_part, right_part, node_type);
    }
    return left_part;
}
ASTNode *parse_additive(){
    ASTNode *left_side = parse_multiplicative();
    while(curToken.type == TOKEN_ADD || curToken.type == TOKEN_SUB){
        TokenType type_t = curToken.type;
        match_token(type_t);

        ASTNode *right_side = parse_multiplicative();
        ASTNodeType op_type = (type_t == TOKEN_ADD) ? NODE_ADD : NODE_SUB;
        left_side = makeNew_Operation(left_side, right_side, op_type);
    }
    return left_side;
}
ASTNode *parse_multiplicative(){
    ASTNode *left_side = parse_primary();
    while(curToken.type == TOKEN_MULTIPLY || curToken.type == TOKEN_DIVIDE || curToken.type == TOKEN_REMAINDER){
        TokenType type_t = curToken.type;
        match_token(type_t);

        ASTNode *right_side = parse_primary();
        ASTNodeType op_type = (type_t == TOKEN_MULTIPLY) ? NODE_MULTIPLY : (type_t == TOKEN_DIVIDE) ? NODE_DIVIDE : NODE_REMAINDER;
        left_side = makeNew_Operation(left_side, right_side, op_type);
    }
    return left_side;
}
ASTNode *parse_primary(){
    if(curToken.type == TOKEN_NUMBER){
        ASTNode *newNode = makeNew_Literal(curToken.value);
        match_token(TOKEN_NUMBER);
        return newNode;
    }
    else if(curToken.type == TOKEN_IDENTIFIER){
        int found = 0;
        DataType dt;
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(symbol_table[i].symbol_name, curToken.lexeme)==0){
                found = 1;
                dt = symbol_table[i].data_type;
            }
        }
        if(found == 0){
            printf("The variable %s is not defined yet!\n", curToken.lexeme);
            exit(1);
        }
        ASTNode *newNode = makeNew_Variable(curToken.lexeme, dt);
        match_token(TOKEN_IDENTIFIER);
        return newNode;
    }
    else if (curToken.type == TOKEN_STRING) {
        ASTNode *node = makeNew_String(curToken.lexeme);
        match_token(TOKEN_STRING);
        return node;
    }
    else if (curToken.type == TOKEN_LPAREN) {
        match_token(TOKEN_LPAREN);
        ASTNode *expr = parse_expression();
        match_token(TOKEN_RPAREN);
        return expr;
    }
    else{
        printf("Expected a Number, Identifier, Open parenthesis or a String element. Error in syntax \n");
        exit(1);
    }

}

int parser_check_is_comparison(TokenType type){
    if(type == TOKEN_LESS_EQUAL) return 1;
    else if(type == TOKEN_LESS_THAN) return 1;
    else if(type == TOKEN_GREATER_EQUAL) return 1;
    else if(type == TOKEN_GREATER_THAN) return 1;
    else if(type == TOKEN_EQUAL) return 1;
    else if(type == TOKEN_NOT_EQUAL) return 1;
    else return 0;
}

ASTNodeType parser_classify_comparison(TokenType type){
    if(type == TOKEN_LESS_EQUAL) return NODE_LESS_EQUAL;
    else if(type == TOKEN_LESS_THAN) return NODE_LESS_THAN;
    else if(type == TOKEN_GREATER_EQUAL) return NODE_GREATER_EQUAL;
    else if(type == TOKEN_GREATER_THAN) return NODE_GREATER_THAN;
    else if(type == TOKEN_EQUAL) return NODE_EQUAL;
    else if(type == TOKEN_NOT_EQUAL) return NODE_NOT_EQUAL;
    else return NODE_LESS_EQUAL;
}

void parser_add_symbol(DataType dtt, char n[]){
    Symbol new_sym;
    new_sym.data_type = dtt;
    strcpy(new_sym.symbol_name, n);
    symbol_table[symbol_count++] = new_sym;
}

int parser_find_symbol(char name[]){
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].symbol_name, name) == 0) {
            return 1;
        }
    }
    return 0;
}