#include "compiler.h"
#include "codegen.h"
#include <string.h>
#include <ctype.h>

// bảng tra cứu biến toàn cục
Variable var_table[100];
int var_count = 0;
static int label_count = 0;
static int string_count = 0;

// tạo node biến
ASTNode *makeNew_Variable(char n[], DataType dt){
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    strcpy(newNode->name, n);
    newNode->type = NODE_VARIABLE;
    newNode->data_type = dt;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// tạo node là số có giá trị (int)
ASTNode *makeNew_Literal(int value){
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    newNode->type = NODE_LITERAL;
    newNode->int_value = value;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
ASTNode *makeNew_String(char *n){
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    newNode->string_value = strdup(n);
    newNode->type = NODE_STRING;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// tạo node phép gán ('='). Phép toán 2 ngôi
ASTNode *makeNew_Assign(ASTNode *var, ASTNode *literal){
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    newNode->type = NODE_ASSIGN;
    newNode->left = var;
    newNode->right = literal;
    return newNode;
}
// tạo node các phép hai ngôi chung (+, -, ==, if,...)
ASTNode *makeNew_Operation(ASTNode *literal1, ASTNode *literal2, ASTNodeType type){
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    newNode->type = type;
    newNode->left = literal1;
    newNode->right = literal2;
    return newNode;
}
// in ra cây Abstract syntax để kiểm tra (thứ tự duyệt giữa)
void inOrder(ASTNode *root){
    if(root == NULL) return;
    inOrder(root->left);
    printNode(root);
    inOrder(root->right);
}
// giải phóng bộ nhớ
void FreeAll(ASTNode *root){
    if(root == NULL) return;
    FreeAll(root->left);
    FreeAll(root->right);
    free(root);
}
// dịch cây abstract syntax ra mã assembly x86
void Assembly_Generator(ASTNode *root){
    if(root == NULL) return;
    if(root->type == NODE_SEQ){
        Assembly_Generator(root->left);
        Assembly_Generator(root->right);
    }
    if(root->type == NODE_ASSIGN){
        Assembly_Generator(root->right);
        int index = find_variable((root->left)->name);
        printf("\tmov [rbp - %d] (%s), rax\n", var_table[index].offset, (root->left)->name);
        asm_printf("\tmov [rbp - %d], rax\n", var_table[index].offset);
    }
    if(check_is_operation(root->type)){
        Assembly_Generator(root->right);
        printf("\tpush rax\n");
        asm_printf("\tpush rax\n");
        Assembly_Generator(root->left);
        printf("\tpop rbx\n");
        asm_printf("\tpop rbx\n");
        printf("\t%s rax, rbx\n", function_for_operation(root->type));
        asm_printf("\t%s rax, rbx\n", function_for_operation(root->type));
    }
    if(root->type == NODE_DIVIDE || root->type == NODE_REMAINDER){
        Assembly_Generator(root->right);
        printf("\tpush rax\n");
        asm_printf("\tpush rax\n");
        Assembly_Generator(root->left);
        printf("\tpop rbx\n");
        asm_printf("\tpop rbx\n");
        printf("\tcqo\n");
        asm_printf("\tcqo\n");
        printf("\tidiv rbx\n");
        asm_printf("\tidiv rbx\n");
        if(root->type == NODE_REMAINDER){
            printf("\tmov rax, rdx\n");
            asm_printf("\tmov rax, rdx\n");
        }
    }
    if(root->type == NODE_BITWISE_SHIFT_LEFT || root->type == NODE_BITWISE_SHIFT_RIGHT){
        Assembly_Generator(root->right);
        printf("\tpush rax\n");
        asm_printf("\tpush rax\n");
        Assembly_Generator(root->left);
        printf("\tpop rcx\n");
        asm_printf("\tpop rcx\n");
        printf("\t%s rax, cl\n", root->type == NODE_BITWISE_SHIFT_LEFT ? "sal" : "sar");
        asm_printf("\t%s rax, cl\n", root->type == NODE_BITWISE_SHIFT_LEFT ? "sal" : "sar");
    }
    if(root->type == NODE_LITERAL){
        printf("\tmov rax, %d\n", root->int_value);
        asm_printf("\tmov rax, %d\n", root->int_value);
    }
    if(root->type == NODE_VARIABLE){
        int index = find_variable(root->name);
        printf("\tmov rax, [rbp - %d] (%s)\n", var_table[index].offset, root->name);
        asm_printf("\tmov rax, [rbp - %d]\n", var_table[index].offset);
    }
    if(root->type == NODE_IF){
        // gọi sang nhánh condition
        int local_label = label_count++;
        Assembly_Generator(root->left);
        printf("\t%s .L_else_%d\n", compiler_comparison_classification((root->left)->type), local_label);
        asm_printf("\t%s .L_else_%d\n",  compiler_comparison_classification((root->left)->type), local_label);
        // sang nhánh body
        ASTNode *if_body = root->right;
        Assembly_Generator(if_body->left);
        if(if_body->right != NULL) {
            printf("\tjmp .L_end_%d\n", local_label);
            asm_printf("\tjmp .L_end_%d\n", local_label);
            
            // In nhãn bắt đầu khối ELSE
            printf(".L_else_%d:\n", local_label);
            asm_printf(".L_else_%d:\n", local_label);
            
            // Sinh mã cho khối ELSE (nhánh phải của IF_BODY)
            Assembly_Generator(if_body->right);
            
            // In nhãn kết thúc toàn bộ cấu trúc IF-ELSE
            printf(".L_end_%d:\n", local_label);
            asm_printf(".L_end_%d:\n", local_label);
        } 
        else {
            // KHÔNG CÓ ELSE: Nhãn ELSE chính là điểm hội tụ cuối cùng
            printf(".L_else_%d:\n", local_label);
            asm_printf(".L_else_%d:\n", local_label);
        }
        
    }
    if(compiler_check_is_comparison_node(root->type)){
        Assembly_Generator(root->right);
        printf("\tpush rax\n");
        asm_printf("\tpush rax\n");
        Assembly_Generator(root->left);
        printf("\tpop rbx\n");
        printf("\tcmp rax, rbx\n");
        asm_printf("\tpop rbx\n");
        asm_printf("\tcmp rax, rbx\n");
    }
    if(root->type == NODE_IF_BODY){

    }
    if(root->type == NODE_PRINT){
        // 1. Duyệt con trái để tính toán biểu thức. Kết quả cuối cùng sẽ nằm trong rax
        Assembly_Generator(root->left);
        if(root->left->type == NODE_STRING){
            printf("\tmov rcx, rax\n");
            asm_printf("\tmov rcx, rax\n");
        }
        else{
            // 2. Chuyển kết quả từ rax sang rdx (Tham số thứ 2 của printf)
            printf("\tmov rdx, rax\n");
            asm_printf("\tmov rdx, rax\n");
            // 3. Nạp địa chỉ của chuỗi định dạng vào rcx (Tham số thứ 1 của printf)
            printf("\tlea rcx, [rip + fmt_int]\n");
            asm_printf("\tlea rcx, [rip + fmt_int]\n");
        }
        // 4. Thiết lập Shadow Space (32-byte) và bảo toàn Stack Alignment
        printf("\tsub rsp, 32\n");
        asm_printf("\tsub rsp, 32\n");
        // 5. Gọi hàm printf từ thư viện liên kết hệ thống
        printf("\tcall printf\n");
        asm_printf("\tcall printf\n");
        // 6. Hoàn trả lại Stack Frame
        printf("\tadd rsp, 32\n");
        asm_printf("\tadd rsp, 32\n");
    }
    if(root->type == NODE_WHILE){
        int local_label = label_count++;
        printf(".L_start_while_%d:\n", local_label);
        asm_printf(".L_start_while_%d:\n", local_label);

        Assembly_Generator(root->left); // generate code cho phan dieu kien
        printf("\t%s .L_end_while_%d\n", compiler_comparison_classification((root->left)->type), local_label);
        asm_printf("\t%s .L_end_while_%d\n",  compiler_comparison_classification((root->left)->type), local_label);
        
        Assembly_Generator(root->right);
        printf("\tjmp .L_start_while_%d\n", local_label);
        asm_printf("\tjmp .L_start_while_%d\n", local_label);
        printf(".L_end_while_%d:\n", local_label);
        asm_printf(".L_end_while_%d:\n", local_label);
    }
    if (root->type == NODE_STRING) {
        int str_id = string_count++;
        // 1. Tạm thời chuyển sang phân vùng .rodata để định nghĩa chuỗi
        printf("\n.section .rodata\n");
        printf(".L_str_%d:\n", str_id);
        printf("\t.asciz \"%s\"\n", root->string_value);
        
        // 2. Quay trở lại phân vùng .text để tiếp tục thực thi lệnh
        printf(".section .text\n");
        
        // 3. Nạp địa chỉ của chuỗi vừa tạo vào thanh ghi rax (RIP-relative)
        printf("\tlea rax, [rip + .L_str_%d]\n", str_id);
        
        // Đồng bộ ra file asm
        asm_printf("\n.section .rodata\n.L_str_%d:\n\t.asciz \"%s\"\n.section .text\n", str_id, root->string_value);
        asm_printf("\tlea rax, [rip + .L_str_%d]\n", str_id);
    }
}
// khi tạo variable cần khởi tạo khoảng trống (base pointer). Khi khởi tạo ta cho vào mảng var và duyệt tuần tự
int find_variable(char n[]){
    for(int i = 0; i<var_count; i++){
        if(strcmp(n, var_table[i].name)==0){
            return i;
        }
    }
    Variable newVar;
    strcpy(newVar.name, n);
    newVar.offset = 8*(var_count + 1);
    var_table[var_count++] = newVar;
    return (var_count - 1);
}
// Các hàm tiện ích cho việc tạo file ASM và các thao tác khác
void print_header(){
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("\tpush rbp\n");
    printf("\tmov rbp, rsp\n");
    printf("\tsub rsp, 16\n");
}

void print_end_assembly(){
    printf("\tmov rax, 0\n");
    printf("\tleave\n");
    printf("\tret\n");
}

void print_header_to_asm(){
    asm_printf(".intel_syntax noprefix\n");
    asm_printf(".global main\n");
    asm_printf("main:\n");
    asm_printf("\tpush rbp\n");
    asm_printf("\tmov rbp, rsp\n");
    asm_printf("\tsub rsp, 64\n");

    asm_printf(".section .rodata\n");
    asm_printf("fmt_int:\n");
    asm_printf("\t.asciz \"%%d\\n\"\n"); 
    asm_printf(".text\n");
}

void print_end_assembly_to_asm(){
    asm_printf("\tmov rax, 0\n");
    asm_printf("\tleave\n");
    asm_printf("\tret\n");
}

void printNode(ASTNode *root){
    if(root->type == NODE_ASSIGN) printf("= ");
    else if(root->type == NODE_ADD) printf("+ ");
    else if(root->type == NODE_SUB) printf("- ");
    else if(root->type == NODE_LITERAL) printf("%d ", root->int_value);
    else if(root->type == NODE_VARIABLE) printf("%s ", root->name);
    else if(root->type == NODE_IF) printf("if ");
    else if(root->type == NODE_EQUAL) printf("== ");
    else if(root->type == NODE_LESS_THAN) printf("< ");
    else if(root->type == NODE_PRINT) printf("printf ");
}

int check_is_operation(ASTNodeType type){
    if(type == NODE_ADD) return 1;
    else if(type == NODE_SUB) return 1;
    else if(type == NODE_BITWISE_AND) return 1;
    else if(type == NODE_BITWISE_OR) return 1;
    else if(type == NODE_MULTIPLY) return 1;
    else return 0;
}
char *function_for_operation(ASTNodeType type){
    if(type == NODE_ADD) return "add";
    else if(type == NODE_SUB) return "sub";
    else if(type == NODE_BITWISE_AND) return "and";
    else if(type == NODE_BITWISE_OR) return "or";
    else if(type == NODE_MULTIPLY) return "imul";
    else return 0;
}
int compiler_check_is_comparison_node(ASTNodeType type){
    if(type == NODE_LESS_EQUAL) return 1;
    else if(type == NODE_LESS_THAN) return 1;
    else if(type == NODE_GREATER_EQUAL) return 1;
    else if(type == NODE_GREATER_THAN) return 1;
    else if(type == NODE_EQUAL) return 1;
    else if(type == NODE_NOT_EQUAL) return 1;
    else return 0;
}
char *compiler_comparison_classification(ASTNodeType type){
    if(type == NODE_LESS_EQUAL) return "jg";
    else if(type == NODE_LESS_THAN) return "jge";
    else if(type == NODE_GREATER_EQUAL) return "jl";
    else if(type == NODE_GREATER_THAN) return "jle";
    else if(type == NODE_EQUAL) return "jne";
    else if(type == NODE_NOT_EQUAL) return "je";
    else return 0;
}
