#include "compiler.h"
#include "codegen.h"
#include <string.h>
#include <ctype.h>
#define LOOP_FOR 0
#define LOOP_WHILE 1

typedef struct {
    int label_id; // ID nhãn (local_label)
    int type;     // LOOP_FOR hoặc LOOP_WHILE
} LoopItem;

static LoopItem loop_stack[100]; // Đủ chứa 100 cấp vòng lặp lồng nhau
static int loop_top = -1;

// Hàm push nhãn vào Stack
void push_loop(int label_id, int type) {
    loop_top++;
    loop_stack[loop_top].label_id = label_id;
    loop_stack[loop_top].type = type;
}

// Hàm pop nhãn khỏi Stack
void pop_loop() {
    if (loop_top >= 0) {
        loop_top--;
    }
}
// bảng tra cứu biến toàn cục
Variable var_table[1000];
int var_count = 0;
static int label_count = 0;
static int string_count = 0;
static int current_stack_offset = 0;

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
// tạo node xâu
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
    // Khi gặp node_seq: là node để kiểm soát việc nhập nhiều câu lệnh.
    // Sinh mã lần lượt cho con trái và con phải.
    if(root->type == NODE_SEQ){
        Assembly_Generator(root->left);
        Assembly_Generator(root->right);
    }
    // Khi gặp node '=': xét các trường hợp
    if (root->type == NODE_ASSIGN) {
        if (root->left->type == NODE_ARRAY_ACCESS) {
            // 1. Tính giá trị vế phải -> push rax
            Assembly_Generator(root->right);
            printf("\tpush rax\n");
            asm_printf("\tpush rax\n");

            Assembly_Generator(root->left->right);

            DataType var_type = root->left->left->data_type;

            // 4. Tính elem_size dưa trên kiểu dữ liệu của biến
            int elem_size = (var_type == TYPE_CHAR_PTR || var_type == TYPE_CHAR_ARRAY) ? 1 : 8;
            printf("\timul rax, %d\n", elem_size);
            asm_printf("\timul rax, %d\n", elem_size);

            // 5. Lấy Base Offset
            char *arr_name = root->left->left->name;
            int index = find_variable(arr_name, root->left->left->element_count);
            int base_offset = var_table[index].offset;

            // 6. PHÂN BIỆT CON TRỎ VÀ MẢNG TỪ NODE DATA_TYPE
            if (var_type == TYPE_CHAR_PTR || var_type == TYPE_INT_PTR) {
                // Con trỏ: Đọc giá trị địa chỉ lưu trong ô nhớ [rbp - offset]
                printf("\tmov rbx, [rbp - %d]\n", base_offset);
                asm_printf("\tmov rbx, [rbp - %d]\n", base_offset);
            } else {
                // Mảng: Lấy địa chỉ của vùng nhớ [rbp - offset]
                printf("\tlea rbx, [rbp - %d]\n", base_offset);
                asm_printf("\tlea rbx, [rbp - %d]\n", base_offset);
            }

            printf("\tadd rax, rbx\n");
            asm_printf("\tadd rax, rbx\n");

            // 7. Pop vế phải -> rbx và ghi vào [rax]
            printf("\tpop rbx\n");
            asm_printf("\tpop rbx\n");

            if (elem_size == 1) {
                printf("\tmov byte ptr [rax], bl\n");
                asm_printf("\tmov byte ptr [rax], bl\n");
            } else {
                printf("\tmov [rax], rbx\n");
                asm_printf("\tmov [rax], rbx\n");
            }
        }
        // 1. Gán qua con trỏ giải tham chiếu: *p = expr
        else if (root->left->type == NODE_DEREF) {
            Assembly_Generator(root->right);
            printf("\tpush rax\n");
            asm_printf("\tpush rax\n");

            Assembly_Generator(root->left->left);

            printf("\tpop rbx\n");
            asm_printf("\tpop rbx\n");

            if (root->left->data_type == TYPE_CHAR) {
                printf("\tmov byte ptr [rax], bl\n");
                asm_printf("\tmov byte ptr [rax], bl\n");
            } else {
                printf("\tmov [rax], rbx\n");
                asm_printf("\tmov [rax], rbx\n");
            }
        }
        // 2. Gán biến hoặc con trỏ trực tiếp: x = expr HOẶC p = expr
        else {
            Assembly_Generator(root->right);
            int index = find_variable((root->left)->name, (root->left)->element_count);
            DataType dtt = (root->left)->data_type;

            if (dtt == TYPE_CHAR) {
                // Chỉ kiểu char mới dùng byte ptr (1 byte)
                printf("\tmov byte ptr [rbp - %d] (%s), al\n", var_table[index].offset, (root->left)->name);
                asm_printf("\tmov byte ptr [rbp - %d], al\n", var_table[index].offset);
            } else {
                // TYPE_INT, TYPE_INT_PTR, TYPE_CHAR_PTR đều dùng 64-bit (8 bytes)
                printf("\tmov [rbp - %d] (%s), rax\n", var_table[index].offset, (root->left)->name);
                asm_printf("\tmov [rbp - %d], rax\n", var_table[index].offset);
            }
        }
    }
    // gặp node phép toán 2 ngôi
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
    // gặp node phép chia hoặc chia lấy dư
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
    // các phép toán dịch bit
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
    // gặp các node số có giá trị 
    if(root->type == NODE_LITERAL){
        printf("\tmov rax, %d\n", root->int_value);
        asm_printf("\tmov rax, %d\n", root->int_value);
    }
    // gặp các biến đứng độc lập
    if(root->type == NODE_VARIABLE){
        int size = root->element_count;
        int index = find_variable(root->name, size);
        DataType cur_type = root->data_type;
        if(cur_type == TYPE_INT || cur_type == TYPE_INT_PTR || cur_type == TYPE_CHAR_PTR){
            printf("\tmov rax, [rbp - %d] (%s)\n", var_table[index].offset, root->name);
            asm_printf("\tmov rax, [rbp - %d]\n", var_table[index].offset);
        }
        if(cur_type == TYPE_CHAR){
            printf("\tmovzx rax, byte ptr [rbp - %d] (%s)\n", var_table[index].offset, root->name);
            asm_printf("\tmovzx rax, byte ptr [rbp - %d]\n", var_table[index].offset);
        }
        if(cur_type == TYPE_CHAR_ARRAY || cur_type == TYPE_INT_ARRAY){
            printf("\tlea rax, [rbp - %d] (%s base address)\n", var_table[index].offset, root->name);
            asm_printf("\tlea rax, [rbp - %d]\n", var_table[index].offset);
        }
    }
    // xử lí node IF
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
    if(root->type == NODE_IF_BODY){

    }
    // các phép so sánh
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
    // xử lí hàm print
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
    // xử lí vòng lặp while
    if(root->type == NODE_WHILE){
        int local_label = label_count++;
        push_loop(local_label, LOOP_WHILE);
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
        pop_loop();
    }
    // xử lí node xâu
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
    // xử lí phép lấy địa chỉ
    if(root->type == NODE_ADDR){
        int index = find_variable(root->left->name, root->element_count);
        int offset = var_table[index].offset;
        printf("\tlea rax, [rbp - %d] (%s)\n", offset, root->left->name);
        asm_printf("\tlea rax, [rbp - %d]\n", offset);
    }
    // xử lí phép giải tham chiếu
    if(root->type == NODE_DEREF){
        Assembly_Generator(root->left);
        if (root->data_type == TYPE_CHAR) {
        // Đọc 1 byte tại địa chỉ [rax]
            printf("\tmovzx rax, byte ptr [rax]\n");
            asm_printf("\tmovzx rax, byte ptr [rax]\n");
        } else {
            // Đọc 8 byte tại địa chỉ [rax]
            printf("\tmov rax, [rax]\n");
            asm_printf("\tmov rax, [rax]\n");
        }
    }
    if (root->type == NODE_ARRAY_ACCESS) {
        // 1. Tính chỉ số i (con phải) -> rax
        Assembly_Generator(root->right);

        // 2. Đọc kiểu dữ liệu của biến từ AST Node (root->left)
        DataType var_type = root->left->data_type;

        // 3. Tính elem_size
        int elem_size = (var_type == TYPE_CHAR_PTR || var_type == TYPE_CHAR_ARRAY) ? 1 : 8;
        printf("\timul rax, %d\n", elem_size);
        asm_printf("\timul rax, %d\n", elem_size);

        // 4. Lấy Base Offset
        char *arr_name = root->left->name;
        int index = find_variable(arr_name, root->left->element_count);
        int base_offset = var_table[index].offset;

        // 5. PHÂN BIỆT CON TRỎ VÀ MẢNG TỪ NODE DATA_TYPE
        if (var_type == TYPE_CHAR_PTR || var_type == TYPE_INT_PTR) {
            printf("\tmov rbx, [rbp - %d]\n", base_offset);
            asm_printf("\tmov rbx, [rbp - %d]\n", base_offset);
        } else {
            printf("\tlea rbx, [rbp - %d]\n", base_offset);
            asm_printf("\tlea rbx, [rbp - %d]\n", base_offset);
        }

        printf("\tadd rax, rbx\n");
        asm_printf("\tadd rax, rbx\n");

        // 6. Giải tham chiếu đọc giá trị tại ô nhớ [rax]
        if (elem_size == 1) {
            printf("\tmovzx rax, byte ptr [rax]\n");
            asm_printf("\tmovzx rax, byte ptr [rax]\n");
        } else {
            printf("\tmov rax, [rax]\n");
            asm_printf("\tmov rax, [rax]\n");
        }
    }
    if (root->type == NODE_FOR) {
        int local_label = label_count++;
        push_loop(local_label, LOOP_FOR);
        // Bóc tách các nhánh từ AST
        ASTNode *header = root->left;  
        ASTNode *body_node = root->right;

        ASTNode *init_node = header->left;
        ASTNode *cond_node = header->right->left;
        ASTNode *inc_node  = header->right->right;

        // 1. Chạy biểu thức Khởi tạo (Init) - nếu có
        if (init_node != NULL) {
            Assembly_Generator(init_node);
        }

        // 2. Nhãn bắt đầu vòng lặp
        printf(".L_start_for_%d:\n", local_label);
        asm_printf(".L_start_for_%d:\n", local_label);

        // 3. Kiểm tra Điều kiện (Cond) - nếu có
        if (cond_node != NULL) {
            Assembly_Generator(cond_node);
            // Nếu điều kiện SAI -> Nhảy ra khỏi vòng lặp
            printf("\t%s .L_end_for_%d\n", compiler_comparison_classification(cond_node->type), local_label);
            asm_printf("\t%s .L_end_for_%d\n", compiler_comparison_classification(cond_node->type), local_label);
        }

        // 4. Chạy Thân vòng lặp (Body)
        if (body_node != NULL) {
            Assembly_Generator(body_node);
        }
        printf(".L_inc_for_%d:\n", local_label);
        asm_printf(".L_inc_for_%d:\n", local_label);
        // 5. Chạy biểu thức Tăng (Inc) - nếu có
        if (inc_node != NULL) {
            Assembly_Generator(inc_node);
        }

        // 6. Nhảy quay lại kiểm tra điều kiện
        printf("\tjmp .L_start_for_%d\n", local_label);
        asm_printf("\tjmp .L_start_for_%d\n", local_label);

        // 7. Nhãn kết thúc vòng lặp
        printf(".L_end_for_%d:\n", local_label);
        asm_printf(".L_end_for_%d:\n", local_label);

        pop_loop();
    }
    if (root->type == NODE_BREAK) {
        if (loop_top < 0) {
            printf("Semantic Error: 'break' outside loop!\n");
            exit(1);
        }
        int cur_id = loop_stack[loop_top].label_id;
        int cur_type = loop_stack[loop_top].type;

        if (cur_type == LOOP_FOR) {
            printf("\tjmp .L_end_for_%d\n", cur_id);
            asm_printf("\tjmp .L_end_for_%d\n", cur_id);
        } else {
            printf("\tjmp .L_end_while_%d\n", cur_id);
            asm_printf("\tjmp .L_end_while_%d\n", cur_id);
        }
    }
    if (root->type == NODE_CONTINUE) {
        if (loop_top < 0) {
            printf("Semantic Error: 'continue' outside loop!\n");
            exit(1);
        }
        int cur_id = loop_stack[loop_top].label_id;
        int cur_type = loop_stack[loop_top].type;

        if (cur_type == LOOP_FOR) {
            // Continue trong for nhảy về bước TĂNG (inc)
            printf("\tjmp .L_inc_for_%d\n", cur_id);
            asm_printf("\tjmp .L_inc_for_%d\n", cur_id);
        } else {
            // Continue trong while nhảy về bước KIỂM TRA ĐIỀU KIỆN
            printf("\tjmp .L_start_while_%d\n", cur_id);
            asm_printf("\tjmp .L_start_while_%d\n", cur_id);
        }
    }
}

/* =========================================
===============Các hàm tiện ích============== */

// khi tạo variable cần khởi tạo khoảng trống (base pointer). Khi khởi tạo ta cho vào mảng var và duyệt tuần tự
int find_variable(char n[], int size) {
    // 1. Tra cứu: Nếu đã có -> Trả về vị trí cũ ngay
    for (int i = 0; i < var_count; i++) {
        if (strcmp(n, var_table[i].name) == 0) {
            return i;
        }
    }
    Variable newVar;
    strcpy(newVar.name, n);

    current_stack_offset += size * 8; // Mảng 10 phần tử -> chiếm 80 bytes
    newVar.offset = current_stack_offset;

    var_table[var_count++] = newVar;
    return (var_count - 1);
}
int get_aligned_stack_size(){
    int total = current_stack_offset; // Tích lũy từ find_variable
    if (total % 16 != 0) {
        total = (total / 16 + 1) * 16; // Làm tròn lên bội số của 16
    }
    return total;
}
// Các hàm tiện ích cho việc tạo file ASM và các thao tác khác
void print_header(){
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("\tpush rbp\n");
    printf("\tmov rbp, rsp\n");
    printf("\tsub rsp, %d\n", 1024);
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
    asm_printf("\tsub rsp, %d\n", 1024);

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
int get_element_size(DataType dt) {
    if (dt == TYPE_CHAR || dt == TYPE_CHAR_ARRAY || dt == TYPE_CHAR_PTR) {
        return 1;
    }
    return 8; // TYPE_INT, TYPE_INT_ARRAY, TYPE_INT_PTR
}