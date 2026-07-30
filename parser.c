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
    //print_current_token();
    if(curToken.type == TOKEN_EOF) return 0;
    return 1;
}
void match_token(TokenType expected_token_type){
    if(curToken.type == expected_token_type){
        advance_token();
    }
    else{
        //print_current_token();
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

        DataType var_type = TYPE_INT;
        
        if(curToken.type == TOKEN_MULTIPLY){
            match_token(TOKEN_MULTIPLY);
            var_type = TYPE_INT_PTR;
        }

        char var_name[25];
        strcpy(var_name, curToken.lexeme);

        match_token(TOKEN_IDENTIFIER);
        int array_size = 1;

        if(curToken.type == TOKEN_LBRACKET){
            match_token(TOKEN_LBRACKET);
            if(curToken.type != TOKEN_NUMBER){
                printf("Expected a number, syntax error\n");
                exit(1);
            }
            if (curToken.value <= 0) {
                printf("Semantic Error: Array size must be greater than 0\n");
                exit(1);
            }
            array_size = curToken.value;
            var_type = TYPE_INT_ARRAY;
            match_token(TOKEN_NUMBER);
            match_token(TOKEN_RBRACKET);
        }

        for(int i = 0; i<symbol_count; i++){
            if(strcmp(var_name, symbol_table[i].symbol_name) == 0){
                printf("Symbol already defined, semantic error\n");
                exit(1);
            }
        }
        parser_add_symbol(var_type, var_name, array_size);
        if(curToken.type == TOKEN_ASSIGN){
            match_token(TOKEN_ASSIGN);

            ASTNode *left_part = makeNew_Variable(var_name, var_type);
            ASTNode *right_tree = parse_expression();
            left_part->element_count = array_size;
            return makeNew_Assign(left_part, right_tree);
        }
        ASTNode *node = makeNew_Variable(var_name, var_type);
        node->element_count = array_size;
        return node;
    }

    // xu li token dau la char
    if(curToken.type == TOKEN_CHAR_KEYWORD){
        match_token(TOKEN_CHAR_KEYWORD);

        DataType var_type = TYPE_CHAR;
        int array_size = 1;

        if(curToken.type == TOKEN_MULTIPLY){
            match_token(TOKEN_MULTIPLY);
            var_type = TYPE_CHAR_PTR;
        }
        char var_name[25];
        strcpy(var_name, curToken.lexeme);
        match_token(TOKEN_IDENTIFIER);

        if(curToken.type == TOKEN_LBRACKET){
            match_token(TOKEN_LBRACKET);
            if(curToken.type != TOKEN_NUMBER){
                printf("Expected a number, syntax error\n");
                exit(1);
            }
            if (curToken.value <= 0) {
                printf("Semantic Error: Array size must be greater than 0\n");
                exit(1);
            }
            array_size = curToken.value;
            var_type = TYPE_CHAR_ARRAY;
            match_token(TOKEN_NUMBER);
            match_token(TOKEN_RBRACKET);
        }

        for(int i = 0; i<symbol_count; i++){
            if(strcmp(var_name, symbol_table[i].symbol_name) == 0){
                printf("Symbol already defined, semantic error\n");
                exit(1);
            }
        }
        parser_add_symbol(var_type, var_name, array_size);

        if(curToken.type == TOKEN_ASSIGN){
            match_token(TOKEN_ASSIGN);
            ASTNode *left_part = makeNew_Variable(var_name, var_type);
            left_part->element_count = array_size;
            ASTNode *right_part = parse_expression();
            return makeNew_Assign(left_part, right_part);
        }
        ASTNode *node = makeNew_Variable(var_name, var_type);
        node->element_count = array_size;
        return node;
    }
    // xu li token dau la bien
    if (curToken.type == TOKEN_IDENTIFIER || curToken.type == TOKEN_MULTIPLY) {
        // parse_unary() sẽ tự động phân tách:
        // - Với 'x'  -> Trả về NODE_VARIABLE
        // - Với '*p' -> Trả về NODE_DEREF
        ASTNode *left_part = parse_unary();

        if (curToken.type == TOKEN_ASSIGN) {
            match_token(TOKEN_ASSIGN);
            ASTNode *right_tree = parse_expression();
            return makeNew_Assign(left_part, right_tree);
        }

        return left_part;
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
    // xu li token dau la for
    if(curToken.type == TOKEN_FOR_KEYWORD){
        match_token(TOKEN_FOR_KEYWORD);
        match_token(TOKEN_LPAREN);
        ASTNode *init_for = NULL;
        ASTNode *cond_for = NULL;
        ASTNode *inc_for = NULL;
        ASTNode *body_for = NULL;
        // 1. Nếu KHÔNG PHẢI dấu ';' -> Mới parse init
        if (curToken.type != TOKEN_SEMICOLON) {
            init_for = parse_expression();
        }
        match_token(TOKEN_SEMICOLON); // Nuốt ';' thứ nhất
        // 2. Nếu KHÔNG PHẢI dấu ';' -> Mới parse cond
        if (curToken.type != TOKEN_SEMICOLON) {
            cond_for = parse_expression();
        }
        match_token(TOKEN_SEMICOLON); // Nuốt ';' thứ hai

        // 3. Nếu KHÔNG PHẢI dấu ')' -> Mới parse inc
        if (curToken.type != TOKEN_RPAREN) {
            inc_for = parse_expression();
        }
        match_token(TOKEN_RPAREN); // Nuốt ')'  
        body_for = parse_statement();
        ASTNode *for_cond_inc = makeNew_Operation(cond_for, inc_for, NODE_SEQ);
        ASTNode *for_header = makeNew_Operation(init_for, for_cond_inc, NODE_FOR_HEADER);
        ASTNode *for_node = makeNew_Operation(for_header, body_for, NODE_FOR);
        return for_node;
    }
    // xu li token break va continue
    if(curToken.type == TOKEN_BREAK_KEYWORD){
        match_token(TOKEN_BREAK_KEYWORD);
        match_token(TOKEN_SEMICOLON);
        return makeNew_Operation(NULL, NULL, NODE_BREAK);
    }
    if(curToken.type == TOKEN_CONTINUE_KEYWORD){
        match_token(TOKEN_CONTINUE_KEYWORD);
        match_token(TOKEN_SEMICOLON);
        return makeNew_Operation(NULL, NULL, NODE_CONTINUE);
    }
    return NULL;
}
ASTNode *parse_expression(){
    ASTNode *left_part = parse_bitwise_or();
    if (curToken.type == TOKEN_ASSIGN) {
        match_token(TOKEN_ASSIGN);
        ASTNode *right = parse_expression(); // Đệ quy đọc vế phải
        return makeNew_Assign(left_part, right);
    }
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
    ASTNode *left_side = parse_unary();
    while(curToken.type == TOKEN_MULTIPLY || curToken.type == TOKEN_DIVIDE || curToken.type == TOKEN_REMAINDER){
        TokenType type_t = curToken.type;
        match_token(type_t);

        ASTNode *right_side = parse_unary();
        ASTNodeType op_type = (type_t == TOKEN_MULTIPLY) ? NODE_MULTIPLY : (type_t == TOKEN_DIVIDE) ? NODE_DIVIDE : NODE_REMAINDER;
        left_side = makeNew_Operation(left_side, right_side, op_type);
    }
    return left_side;
}
ASTNode *parse_unary(){
    if(curToken.type == TOKEN_BITWISE_AND){
        match_token(TOKEN_BITWISE_AND);
        ASTNode *node = parse_unary();
        DataType result_type = (node->data_type == TYPE_CHAR) ? TYPE_CHAR_PTR : TYPE_INT_PTR;
        ASTNode *operation = makeNew_Operation(node, NULL, NODE_ADDR);
        operation->data_type = result_type;
        return operation;
    }
    if (curToken.type == TOKEN_MULTIPLY) {
        match_token(TOKEN_MULTIPLY);
        ASTNode *operand = parse_unary(); // Cho phép con trỏ nhiều cấp **p
        
        DataType result_type = TYPE_INT;
        if (operand->data_type == TYPE_CHAR_PTR) result_type = TYPE_CHAR;
        else if (operand->data_type == TYPE_INT_PTR) result_type = TYPE_INT;

        ASTNode *node = makeNew_Operation(operand, NULL, NODE_DEREF);
        node->data_type = result_type;
        return node;
    }
    return parse_primary();
}
ASTNode *parse_primary(){
    if(curToken.type == TOKEN_NUMBER){
        ASTNode *newNode = makeNew_Literal(curToken.value);
        match_token(TOKEN_NUMBER);
        return newNode;
    }
    else if(curToken.type == TOKEN_IDENTIFIER){
        int found = 0;
        char var_name[30];
        DataType dt;
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(symbol_table[i].symbol_name, curToken.lexeme)==0){
                found = 1;
                dt = symbol_table[i].data_type;
                strcpy(var_name, symbol_table[i].symbol_name);
            }
        }
        if(found == 0){
            printf("The variable %s is not defined yet!\n", curToken.lexeme);
            exit(1);
        }
        ASTNode *newNode = makeNew_Variable(curToken.lexeme, dt);
        match_token(TOKEN_IDENTIFIER);

        if(curToken.type == TOKEN_LBRACKET){
            if (dt != TYPE_INT_ARRAY && dt != TYPE_CHAR_ARRAY && dt != TYPE_INT_PTR && dt != TYPE_CHAR_PTR) {
                printf("Semantic Error: Variable '%s' is not an array or pointer!\n", var_name);
                exit(1);
            }
            match_token(TOKEN_LBRACKET);
            ASTNode *exp = parse_expression();
            match_token(TOKEN_RBRACKET);

            ASTNode *array_access = makeNew_Operation(newNode, exp, NODE_ARRAY_ACCESS);
            if(dt == TYPE_CHAR_ARRAY) array_access->data_type = TYPE_CHAR;
            if(dt == TYPE_INT_ARRAY) array_access->data_type = TYPE_INT;
            return array_access;
        }

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
    else if(curToken.type == TOKEN_CHAR_LITERAL){
        ASTNode *newNode = makeNew_Literal(curToken.value);
        newNode->data_type = TYPE_CHAR;
        match_token(TOKEN_CHAR_LITERAL);
        return newNode;
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

void parser_add_symbol(DataType dtt, char n[], int size){
    Symbol new_sym;
    new_sym.data_type = dtt;
    new_sym.size_bytes = size;
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