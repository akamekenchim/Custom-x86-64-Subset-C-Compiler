#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "lexer.h"
#define MAX_PROGRAM_LENGTH 65536
#define MAX_SYMBOL_COUNT 1024
#define MAX_IDENTIFIER_LENGTH 48

Token curToken;
static int position = 0;
static char source[MAX_PROGRAM_LENGTH];
Symbol symbol_table[MAX_SYMBOL_COUNT];
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

ASTNode *parse_program() {
    ASTNode *root = makeNew_Operation(NULL, NULL, NODE_PROGRAM);
    ASTNode *func_list = NULL;

    while (curToken.type != TOKEN_EOF) {
        ASTNode *func = parse_function_definition();
        
        if (func_list == NULL) {
            func_list = func; 
        } else {
            func_list = makeNew_Operation(func_list, func, NODE_SEQ);
        }
    }
    root->left = func_list;
    return root;
}
ASTNode *parse_function_definition(){
    // resets the symbol_count to re-initialize the variable scope, then parses the type declaration
    symbol_count = 0;
    DataType return_type = TYPE_VOID;
    // types: int, char, void, int *, char *
    if(curToken.type == TOKEN_INT_KEYWORD){
        match_token(TOKEN_INT_KEYWORD);
        return_type = TYPE_INT;
    }
    else if(curToken.type == TOKEN_CHAR_KEYWORD){
        match_token(TOKEN_CHAR_KEYWORD);
        return_type = TYPE_CHAR;
    }
    else if(curToken.type == TOKEN_VOID_KEYWORD){
        match_token(TOKEN_VOID_KEYWORD);
        return_type = TYPE_VOID;
    }
    else{
        printf("Expected a valid return type!\n");
        exit(1);
    }
    // parses the function name + left parenthesis
    char func_name[MAX_IDENTIFIER_LENGTH];
    strcpy(func_name, curToken.lexeme);
    match_token(TOKEN_IDENTIFIER);
    match_token(TOKEN_LPAREN);
    // builds the parameter 'linked list'
    ASTNode *head_param = NULL;
    ASTNode *tail_param = NULL;
    if(curToken.type != TOKEN_RPAREN){
        while(1){
            // parses the type declaration for the parameter
            DataType param_type;
            if (curToken.type == TOKEN_INT_KEYWORD) {
                param_type = TYPE_INT;
                match_token(TOKEN_INT_KEYWORD);
            } else if (curToken.type == TOKEN_CHAR_KEYWORD) {
                param_type = TYPE_CHAR;
                match_token(TOKEN_CHAR_KEYWORD);
            } else {
                printf("Syntax Error: Expected parameter type (int, char) \n");
                exit(1);
            }
            // check if it's a pointer, then parses the name of the param
            if (curToken.type == TOKEN_MULTIPLY) {
                param_type = (param_type == TYPE_CHAR) ? TYPE_CHAR_PTR : TYPE_INT_PTR;
                match_token(TOKEN_MULTIPLY);
            }
            if (curToken.type != TOKEN_IDENTIFIER) {
                printf("Syntax Error: Expected parameter name\n");
                exit(1);
            }
            parser_add_symbol(param_type, curToken.lexeme, 1);
            ASTNode *param = makeNew_Operation(NULL, NULL, NODE_FUNC_PARAM_LIST);
            param->data_type = param_type;
            param->right = NULL;
            strcpy(param->name, curToken.lexeme);
            match_token(TOKEN_IDENTIFIER);
            /* if head_param is null -> first parameter. 
            *  Builds a sub-tree structure that represents a linked list
            *
            *          head_param
            *          /        \
            *         NULL      node_func_param_list
            *                        \
            *                        node_func_param_list
            *                           \
            *                          node_func_param_list
            *                              \
            *                              tail_param
            * 
            * -- keeps going if the next token after identifier is a comma.
            */
            if (head_param == NULL) {
                head_param = param;
                tail_param = param;
            } else {
                tail_param->right = param;
                tail_param = param;
            }
            if (curToken.type == TOKEN_COMMA) {
                match_token(TOKEN_COMMA);
            } else {
                break;
            }
        }
    }
    match_token(TOKEN_RPAREN); 
    /* builds the final tree structure:
    *               node_func_define
    *               /              \
    *           head_param          body
    *            /    \
    *          null  node_func_param
    *                   \
    *                   ....
    */
    match_token(TOKEN_LBRACE);
    ASTNode *body = parse_multiple_statements(); 
    match_token(TOKEN_RBRACE);
    ASTNode *func_node = makeNew_Operation(NULL, NULL, NODE_FUNC_DEFINE);
    strcpy(func_node->name, func_name);
    func_node->data_type = return_type;
    func_node->left = head_param;
    func_node->right = body;     

    return func_node;
}

ASTNode *parse_multiple_statements(){
    while(curToken.type == TOKEN_SEMICOLON){
        match_token(TOKEN_SEMICOLON);
    }
    if(curToken.type == TOKEN_RBRACE || curToken.type == TOKEN_EOF){
        return NULL;
    }
    /* outputs a tree, code generator will parse by in-order
    *               NODE_SEQ
    *                /   \ 
    *        statement   node_seq
    *                      /  \ 
    *             statement    node_seq
    *               ..................
    */
    ASTNode *first_statement = parse_statement();
    if(curToken.type == TOKEN_SEMICOLON){
        match_token(TOKEN_SEMICOLON);
    }
    // recursively calls itself to parse multiple lines of code
    ASTNode *next_statement = NULL;
    if(curToken.type != TOKEN_RBRACE && curToken.type != TOKEN_EOF){
        next_statement = parse_multiple_statements();
    }

    if (first_statement == NULL) return next_statement;
    if (next_statement == NULL) return first_statement;

    return makeNew_Operation(first_statement, next_statement, NODE_SEQ);
}
ASTNode *parse_statement(){
    /*  parses return keyword
    *    return takina == 3;
    *           ret_node
    *           /      \
    *      node_equal  null
    *      /       \ 
    *    takina    3
    */
    if (curToken.type == TOKEN_RETURN) {
        match_token(TOKEN_RETURN);
        ASTNode *ret_node = makeNew_Operation(NULL, NULL, NODE_RETURN);
        // check for return element
        if (curToken.type != TOKEN_SEMICOLON) {
            ret_node->left = parse_expression();
        }
        match_token(TOKEN_SEMICOLON); 
        return ret_node;
    }
    /* basically just like parse_multiple_statement */
    if (curToken.type == TOKEN_LBRACE) {
        match_token(TOKEN_LBRACE);
        ASTNode *block_content = parse_multiple_statements();
        match_token(TOKEN_RBRACE);
        return block_content; 
    }
    // handles int declaration
    if(curToken.type == TOKEN_INT_KEYWORD){
        match_token(TOKEN_INT_KEYWORD);
        DataType var_type = TYPE_INT;
        // checks for pointer declaration
        if(curToken.type == TOKEN_MULTIPLY){
            match_token(TOKEN_MULTIPLY);
            var_type = TYPE_INT_PTR;
        }

        char var_name[MAX_IDENTIFIER_LENGTH];
        strcpy(var_name, curToken.lexeme);
        match_token(TOKEN_IDENTIFIER);
        // handles array declaration
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
        // avoids multiple declaration of one symbol, then appends new symbol 
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(var_name, symbol_table[i].symbol_name) == 0){
                printf("Symbol already defined, semantic error\n");
                exit(1);
            }
        }
        parser_add_symbol(var_type, var_name, array_size);
        /*      output tree:
        *           node_assign (=)
        *           /            \ 
        *        node_var        node_add (+)    
        *   (name, elm count,     /       \ 
        *    type)              node_var  node_literal
        *                           ..........
        */
        if(curToken.type == TOKEN_ASSIGN){
            match_token(TOKEN_ASSIGN);
            ASTNode *left_part = makeNew_Variable(var_name, var_type);
            ASTNode *right_tree = parse_expression();
            left_part->element_count = array_size;
            return makeNew_Assign(left_part, right_tree);
        }
        return NULL;
    }

    // char keyword handle, similar to int
    if(curToken.type == TOKEN_CHAR_KEYWORD){
        match_token(TOKEN_CHAR_KEYWORD);

        DataType var_type = TYPE_CHAR;
        int array_size = 1;

        if(curToken.type == TOKEN_MULTIPLY){
            match_token(TOKEN_MULTIPLY);
            var_type = TYPE_CHAR_PTR;
        }
        char var_name[MAX_IDENTIFIER_LENGTH];
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
        return NULL;
    }
    // handles variable assignment
    if (curToken.type == TOKEN_IDENTIFIER || curToken.type == TOKEN_MULTIPLY) {
        // parse unary can call parse primary, so all cases will be handled
        ASTNode *left_part = parse_unary();

        if (curToken.type == TOKEN_ASSIGN) {
            /*
            *         node_assign
            *          /       \ 
            *    var||arr_acc  '+'
            *                  / \ 
            *                var 78
            *   -> takina[2] = chisato + 78;
            */
            match_token(TOKEN_ASSIGN);
            ASTNode *right_tree = parse_expression();
            return makeNew_Assign(left_part, right_tree);
        }
        
        return left_part;
    }
    // if statement handling
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
        /*      output tree:
        *               node_if
        *               /     \ 
        *              ==     node_if_body
        *             /  \     /        \ 
        *            kc  69  seq(then) seq (else branch)
        *                    / \           .....
        *                  ... seq
        *                      / \
        *                    .......
        *  if(kc == 69) {// code...} 
        *  else {// code ...}
        */
        ASTNode *if_statement_body = makeNew_Operation(then_branch, else_branch, NODE_IF_BODY);
        ASTNode *if_statement_tree = makeNew_Operation(left_condition, if_statement_body, NODE_IF);
        return if_statement_tree;
    }
    // printf handling
    if(curToken.type == TOKEN_PRINT){
        match_token(TOKEN_PRINT);
        match_token(TOKEN_LPAREN);
        ASTNode *toPrint = parse_expression();
        match_token(TOKEN_RPAREN);
        /*      output tree
        *          NODE_PRINT
        *           /       \ 
        *   string/var/num  null
        */
        return makeNew_Operation(toPrint, NULL, NODE_PRINT);
    }
    // while loop handling
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
    // for loop handling
    if(curToken.type == TOKEN_FOR_KEYWORD){
        match_token(TOKEN_FOR_KEYWORD);
        match_token(TOKEN_LPAREN);
        ASTNode *init_for = NULL;
        ASTNode *cond_for = NULL;
        ASTNode *inc_for = NULL;
        ASTNode *body_for = NULL;
        // check if there's initialization
        if (curToken.type != TOKEN_SEMICOLON) {
            init_for = parse_expression();
        }
        match_token(TOKEN_SEMICOLON);
        // check for condition
        if (curToken.type != TOKEN_SEMICOLON) {
            cond_for = parse_expression();
        }
        match_token(TOKEN_SEMICOLON);
        // check for increment
        if (curToken.type != TOKEN_RPAREN) {
            inc_for = parse_expression();
        }
        match_token(TOKEN_RPAREN); 
        body_for = parse_statement();
        /*                   output tree:
        *                  NODE_FOR
        *                  /      \ 
        *       NODE_FOR_HEADER   body_for
        *       /         \ 
        *   init_for    NODE_SEQ
        *               /      \
        *             cond     inc
        */
        ASTNode *for_cond_inc = makeNew_Operation(cond_for, inc_for, NODE_SEQ);
        ASTNode *for_header = makeNew_Operation(init_for, for_cond_inc, NODE_FOR_HEADER);
        ASTNode *for_node = makeNew_Operation(for_header, body_for, NODE_FOR);
        return for_node;
    }
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
        ASTNode *right = parse_expression();
        return makeNew_Assign(left_part, right);
    }
    if(parser_check_is_comparison(curToken.type)){
        TokenType curType = curToken.type;
        match_token(curType);
        ASTNode *right_part = parse_bitwise_or();
        ASTNodeType node_type = parser_classify_comparison(curType); 
        /*    both left and right parts are expressions.
        *    example: takina + chisato >= 69 * elaina / akame
        *          node_greater_equal
        *           /             \ 
        *       node_add         node_multiply
        *       /      \          /         \ 
        *    takina   chisato   69          node_divide
        *                                    /       \ 
        *                                   elaina   akame      
        */
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
    // handles negative numbers declaration
    if(curToken.type == TOKEN_SUB){
        match_token(TOKEN_SUB);
        ASTNode *minus_operand = parse_unary();
        ASTNode *node = makeNew_Operation(minus_operand, NULL, NODE_UNARY_MINUS);
        return node;
    }
    // handles getting address (allows &(*p))
    if(curToken.type == TOKEN_BITWISE_AND){
        match_token(TOKEN_BITWISE_AND);
        ASTNode *node = parse_unary();
        DataType result_type = (node->data_type == TYPE_CHAR) ? TYPE_CHAR_PTR : TYPE_INT_PTR;
        ASTNode *operation = makeNew_Operation(node, NULL, NODE_ADDR);
        operation->data_type = result_type;
        return operation;
    }
    // handles pointers and dereference (allows **p)
    if (curToken.type == TOKEN_MULTIPLY) {
        match_token(TOKEN_MULTIPLY);
        ASTNode *operand = parse_unary(); 
        
        DataType result_type = TYPE_INT;
        if (operand->data_type == TYPE_CHAR_PTR) result_type = TYPE_CHAR;
        else if (operand->data_type == TYPE_INT_PTR) result_type = TYPE_INT;

        ASTNode *node = makeNew_Operation(operand, NULL, NODE_DEREF);
        node->data_type = result_type;
        return node;
    }
    /*      output tree
    *           node_addr / node_deref
    *               /             \ 
    *     node_var||node_arr/acc  null
    *       (datatype, name)
    */
    return parse_primary();
}
ASTNode *parse_primary(){
    if(curToken.type == TOKEN_NUMBER){
        ASTNode *newNode = makeNew_Literal(curToken.value);
        match_token(TOKEN_NUMBER);
        return newNode;
    }
    else if(curToken.type == TOKEN_IDENTIFIER){
        char id_name[MAX_IDENTIFIER_LENGTH];
        strcpy(id_name, curToken.lexeme);
        match_token(TOKEN_IDENTIFIER);
        if(curToken.type == TOKEN_LPAREN){
            match_token(TOKEN_LPAREN);
            ASTNode *head_param = NULL;
            ASTNode *tail_param = NULL;
            if(curToken.type != TOKEN_RPAREN){
                while(1){
                    ASTNode *param = parse_expression();
                    if(head_param == NULL){
                        head_param = param;
                        tail_param = param;
                    }
                    else{
                        tail_param->right = param;
                        tail_param = param;
                    }
                    if(curToken.type == TOKEN_COMMA){
                        match_token(TOKEN_COMMA);
                    }
                    else{
                        break;
                    }
                }
            }
            match_token(TOKEN_RPAREN);
            ASTNode *func_call_node = makeNew_Operation(head_param, NULL, NODE_FUNC_CALL);
            strcpy(func_call_node->name, id_name);
            return func_call_node;
        }
        int found = 0;
        char var_name[MAX_IDENTIFIER_LENGTH];
        int elem_count = 1;
        DataType dt;
        for(int i = 0; i<symbol_count; i++){
            if(strcmp(symbol_table[i].symbol_name, id_name)==0){
                found = 1;
                dt = symbol_table[i].data_type;
                elem_count = symbol_table[i].size_bytes;
                strcpy(var_name, symbol_table[i].symbol_name);
            }
        }
        if(found == 0){
            printf("The variable %s is not defined yet!\n", id_name);
            exit(1);
        }
        ASTNode *newNode = makeNew_Variable(id_name, dt);
        newNode->element_count = elem_count;
       
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
    switch(type){
        case TOKEN_LESS_EQUAL:
        case TOKEN_LESS_THAN:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_GREATER_THAN:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            return 1;
        default:
            return 0;
    }
}
ASTNodeType parser_classify_comparison(TokenType type){
    switch(type){
        case TOKEN_LESS_EQUAL:    return NODE_LESS_EQUAL;
        case TOKEN_LESS_THAN:     return NODE_LESS_THAN;
        case TOKEN_GREATER_EQUAL: return NODE_GREATER_EQUAL;
        case TOKEN_GREATER_THAN:  return NODE_GREATER_THAN;
        case TOKEN_EQUAL:         return NODE_EQUAL;
        case TOKEN_NOT_EQUAL:     return NODE_NOT_EQUAL;
        default:                  return NODE_LESS_EQUAL;
    }
}
// creates a new symbol then appends it to the symbol table (datatype, name, size)
void parser_add_symbol(DataType dtt, char n[], int size){
    Symbol new_sym;
    new_sym.data_type = dtt;
    new_sym.size_bytes = size;
    strcpy(new_sym.symbol_name, n);
    symbol_table[symbol_count++] = new_sym;
}
/*
int parser_find_symbol(char name[]){
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].symbol_name, name) == 0) {
            return 1;
        }
    }
    return 0;
}*/