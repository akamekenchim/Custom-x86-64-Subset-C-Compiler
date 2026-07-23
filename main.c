#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
ASTNode *root = NULL;
int main(int argc, char *argv[]) {
    // 1. Kiểm tra tham số dòng lệnh
    if (argc < 2) {
        fprintf(stderr, "Error: No input file specified.\n");
        fprintf(stderr, "Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    // 2. Mở file nguồn ở chế độ đọc nhị phân (để tính byte chính xác)
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Lỗi mở file nguồn");
        return 1;
    }

    // 3. Đo kích thước thực tế của file (Dynamic Size Detection)
    fseek(file, 0, SEEK_END);      // Nhảy xuống đáy file
    long file_size = ftell(file);  // Lấy vị trí byte hiện tại (chính là độ dài file)
    fseek(file, 0, SEEK_SET);      // Quay trở lại đầu file

    // 4. Cấp phát vùng nhớ Heap vừa khít + 1 byte cho ký tự kết thúc chuỗi '\0'
    char *source = (char *)malloc(file_size + 1);
    if (!source) {
        fprintf(stderr, "Lỗi cấp phát bộ nhớ cho file nguồn.\n");
        fclose(file);
        return 1;
    }

    // 5. Đọc toàn bộ nội dung file vào bộ đệm RAM
    size_t bytes_read = fread(source, 1, file_size, file);
    source[bytes_read] = '\0';     // Bắt buộc phải Null-terminate chuỗi
    fclose(file);                  // Đóng file nguồn an toàn

    // 6. Khởi chạy Emitter để xuất file target.asm
    if (!init_asm_emitter("target.asm")) {
        perror("Lỗi mở file target.asm");
        free(source);
        return 1;
    }

    // 7. Thực thi Pipeline Biên dịch
    init_parser(source);
    root = parse_multiple_statements();
    
    // Debug cây AST lên Console
    inOrder(root);
    printf("\n");

    // Sinh mã Assembly ra file target.asm
    print_header_to_asm();
    Assembly_Generator(root);
    print_end_assembly_to_asm();

    // 8. Giải phóng toàn bộ tài nguyên hệ thống
    FreeAll(root);
    close_asm_emitter();
    free(source);                  // Giải phóng bộ đệm file nguồn trên Heap

    return 0;
}