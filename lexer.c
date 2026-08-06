#include "compiler.h"
#include "lexer.h"
#include <string.h>
#include <ctype.h>

Token get_token(char source[], int *pos){
    Token t;
    t.type = TOKEN_EOF;
    t.value = 0;
    //bo qua dau cach
    while(isblank(source[*pos]) || isspace(source[*pos])){
        (*pos) += 1;
    }
    // ki tu null
    if(source[*pos] == '\0'){
        t.type = TOKEN_EOF;
        strcpy(t.lexeme, "EOF");
        return t;
    }
    //dau bang hoac ki tu so sanh bang
    if(source[*pos] == '='){
        if(source[*pos + 1] == '='){
            t.type = TOKEN_EQUAL;
            strcpy(t.lexeme, "==");
            (*pos) += 2;
            return t;
        }
        t.type = TOKEN_ASSIGN;
        strcpy(t.lexeme, "=");
        (*pos)++;
        return t;
    }
    // dau cong - dau tru - nhan - chia lay du (operation)
    if(lexer_check_operation(source[*pos])){
        t.type = lexer_classify_operation(source[*pos]);
        t.lexeme[0] = source[*pos];
        t.lexeme[1] = '\0';
        (*pos)++;
        return t;
    }
    // toan tu or - and bitwise
    if(source[*pos] == '|' || source[*pos] == '&'){
        t.type = source[*pos] == '|' ? TOKEN_BITWISE_OR : TOKEN_BITWISE_AND;
        t.lexeme[0] = source[*pos];
        t.lexeme[1] = '\0';
        (*pos)++;
        return t;
    }
    // tam thoi bo qua chi thi tien xu ly #include <stdio.h>
    if(source[*pos] == '#'){
        while(source[*pos] != '\n' && source[*pos] != '\0'){
            (*pos)++;
        }
        return get_token(source, pos); 
    }
    // toan tu or - and bitwise + xu li comment code, phep chia
    if(source[*pos] == '/'){
        if(source[*pos + 1] == '/'){
            // Bỏ qua comment rồi gọi lại get_token để duyệt lại
            while(source[*pos] != '\n' && source[*pos] != '\0'){
                (*pos)++;
            }
            return get_token(source, pos); 
        }
        // xu ly comment /* */
        if(source[*pos + 1] == '*'){
            (*pos) += 2;
            if(source[*pos] == '\0'){
                printf("Syntax Error: Unclosed comment\n");
                exit(1);
            }
            while(!(source[*pos] == '*' && source[(*pos)+1] == '/')){
                (*pos)++;
                if(source[*pos] == '\0'){
                    printf("Syntax Error: Unclosed comment\n");
                    exit(1);
                }
            }
            // bỏ qua dấu */ cuối rồi gọi lại get_token để duyệt lại
            (*pos) += 2;
            return get_token(source, pos);
        }
        // default case
        t.type = TOKEN_DIVIDE;
        t.lexeme[0] = source[*pos];
        t.lexeme[1] = '\0';
        (*pos)++;
        return t;
    }
    // !=
    if(source[*pos] == '!'){
        if(source[*pos + 1] == '='){
            t.type = TOKEN_NOT_EQUAL;
            strcpy(t.lexeme, "!=");
            (*pos) += 2;
            return t;
        }
        else{
            t.type = TOKEN_EOF;
            strcpy(t.lexeme, "EOF");
            return t;
        }
    }
    // neu la so
    if(isdigit(source[*pos])){
        int i = 0;
        while(isdigit(source[*pos])){  // tiếp theo còn là số thì gom nhóm
            t.lexeme[i++] = source[(*pos)++];
        }
        t.lexeme[i] = '\0';
        t.type = TOKEN_NUMBER;
        t.value = atoi(t.lexeme);
        t.value = t.value;
        return t;
    }
    // neu la chu: co the la bien, co the la keyword
    if(isalpha(source[*pos]) || source[*pos] == '_'){
        int i = 0;
        while((isalpha(source[*pos]) || isdigit(source[*pos])) || source[*pos] == '_'){ // kí tự tiếp theo là số hoặc chữ thì vẫn gom nhóm thành 1 token.
            t.lexeme[i++] = source[(*pos)++];
        }
        t.lexeme[i] = '\0';
        t.type = check_is_keyword(t.lexeme);
        return t;
    }
    // dau nho hon (co the la nho hon hoac bang, co the la bitwise shift left)
    if(source[*pos] == '<'){
        if(source[*pos + 1] == '<'){
            t.type = TOKEN_BITWISE_SHIFT_LEFT;
            strcpy(t.lexeme, "<<");
            (*pos) += 2;
            return t;
        }
        else if(source[*pos+1] == '='){
            t.type = TOKEN_LESS_EQUAL;
            strcpy(t.lexeme, "<=");
            (*pos) += 2;
            return t;
        }
        t.type = TOKEN_LESS_THAN;
        strcpy(t.lexeme, "<");
        (*pos)++;
        return t;
    }
    // dau lon hon (co the la lon hon hoac bang, co the la bitwise shift right)
    if(source[*pos] == '>'){
        if(source[*pos + 1] == '>'){
            t.type = TOKEN_BITWISE_SHIFT_RIGHT;
            strcpy(t.lexeme, ">>");
            (*pos) += 2;
            return t;
        }
        else if(source[*pos+1] == '='){
            t.type = TOKEN_GREATER_EQUAL;
            strcpy(t.lexeme, ">=");
            (*pos) += 2;
            return t;
        }
        t.type = TOKEN_GREATER_THAN;
        strcpy(t.lexeme, ">");
        (*pos)++;
        return t;
    }
    // xu ly dau phay 
    if(source[*pos] == ','){
        t.type = TOKEN_COMMA;
        strcpy(t.lexeme, ",");
        (*pos)++;
        return t;
    }
    // cac dau ngoac va dau cham phay
    if(source[*pos] == '{' || source[*pos] == '(' || source[*pos] == ')' || source[*pos] == '}' || source[*pos] == ';' || source[*pos] == '[' || source[*pos] == ']'){
        t.type = check_is_bracket(source[*pos]);
        t.lexeme[0] = source[*pos];
        t.lexeme[1] = '\0';
        (*pos)++;
        return t;
    }
    // dau nhay kep - bao hieu chuoi
    if(source[*pos] == '"'){
        int i = 0;
        (*pos)++; // bo qua dau "
        while(source[*pos] != '"'){
            t.lexeme[i++] = source[(*pos)++];
        }
        if(source[*pos] == '"'){
            (*pos)++; // bo qua dau "
        }
        t.lexeme[i] = '\0';
        t.type = TOKEN_STRING;
        return t;
    }
    // xu li char literal
    if(source[*pos] == '\''){
        (*pos)++; // bo qua dau '
        char c = source[*pos];
        int char_val = (int)c;
        int i = 0;
        if(c == '\\'){ // neu c la backslash -> \n, \t,..
            t.lexeme[i++] = c;
            (*pos)++; // bo qua backslash
            char esc = source[*pos];
            t.lexeme[i++] = esc;
            if(esc == 'n') char_val = '\n';
            else if (esc == 't') char_val = '\t';
            else if (esc == '0') char_val = '\0';
            else char_val = esc;
        }
        else {
            t.lexeme[i++] = c; // copy vao trong lexeme
        }
        t.lexeme[i] = '\0';
        (*pos)++; // bo qua ki tu 
        if (source[*pos] == '\'') {
            (*pos)++; // Bỏ qua dấu ' đóng
        } else {
            printf("Lexer Error: Unclosed character literal!\n");
            exit(1);
        }
        
        // Tạo token hằng ký tự
        t.type = TOKEN_CHAR_LITERAL;
        t.value = char_val; 
        return t;
    }
    // neu khong phai tat ca nhung cai kia
    t.type = TOKEN_EOF;
    strcpy(t.lexeme, "EOF");
    return t;
}

// cac ham tien ich
TokenType check_is_keyword(char token[]){
    if(strcmp(token, "int") == 0) return TOKEN_INT_KEYWORD;
    else if(strcmp(token, "char") == 0) return TOKEN_CHAR_KEYWORD;
    else if(strcmp(token, "if") == 0) return TOKEN_IF;
    else if(strcmp(token, "else") == 0) return TOKEN_ELSE;
    else if(strcmp(token, "printf") == 0) return TOKEN_PRINT;
    else if(strcmp(token, "while") == 0) return TOKEN_WHILE;
    else if(strcmp(token, "for") == 0) return TOKEN_FOR_KEYWORD;
    else if(strcmp(token, "break") == 0) return TOKEN_BREAK_KEYWORD;
    else if(strcmp(token, "continue") == 0) return TOKEN_CONTINUE_KEYWORD;
    else if(strcmp(token, "void") == 0) return TOKEN_VOID_KEYWORD;
    else if(strcmp(token, "return") == 0) return TOKEN_RETURN;
    return TOKEN_IDENTIFIER;
}


/* Refactor: uses switch-case instad of if-else if for optimization */
TokenType check_is_bracket(char c){
    switch(c){
        case '{': 
            return TOKEN_LBRACE; 
        case '}': 
            return TOKEN_RBRACE; 
        case '(': 
            return TOKEN_LPAREN; 
        case ')': 
            return TOKEN_RPAREN; 
        case ';': 
            return TOKEN_SEMICOLON; 
        case '[': 
            return TOKEN_LBRACKET; 
        case ']': 
            return TOKEN_RBRACKET; 
        default: 
            return TOKEN_EOF; 
    }
}
TokenType lexer_classify_operation(char c) {
    switch (c) {
        case '+': return TOKEN_ADD;
        case '-': return TOKEN_SUB;
        case '%': return TOKEN_REMAINDER;
        case '*': return TOKEN_MULTIPLY;
        default:  return TOKEN_EOF; 
    }
}

int lexer_check_operation(char c) {
    switch (c) {
        case '+':
        case '-':
        case '%':
        case '*':
            return 1;
        default:
            return 0;
    }
}

char *token_type_converter(TokenType t){
    if(t == TOKEN_ADD) return "TOKEN_ADD";
    else if(t == TOKEN_SUB) return "TOKEN_SUB";
    else if(t == TOKEN_ASSIGN) return "TOKEN_ASSIGN";
    else if(t == TOKEN_EOF) return "TOKEN_EOF";
    else if(t == TOKEN_INT_KEYWORD) return "TOKEN_INT_KEYWORD";
    else if(t == TOKEN_IDENTIFIER) return "TOKEN_IDENTIFIER";
    else if(t == TOKEN_NUMBER) return "TOKEN_NUMBER";
    else if(t == TOKEN_IF) return "TOKEN_IF";
    else if(t == TOKEN_ELSE) return "TOKEN_ELSE";
    else if(t == TOKEN_LPAREN) return "TOKEN_LPAREN";
    else if(t == TOKEN_RPAREN) return "TOKEN_RPAREN";
    else if(t == TOKEN_LBRACE) return "TOKEN_LBRACE";
    else if(t == TOKEN_RBRACE) return "TOKEN_RBRACE";
    else if(t == TOKEN_EQUAL) return "TOKEN_EQUAL";
    else if(t == TOKEN_LESS_THAN) return "TOKEN_LESS_THAN";
    return "NULL";
}