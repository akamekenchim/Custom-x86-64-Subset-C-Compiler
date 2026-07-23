#include <stdio.h>
#include <stdarg.h>
#include "codegen.h"
static FILE *asm_out = NULL;

// Khởi tạo file stream
int init_asm_emitter(const char *filename) {
    asm_out = fopen(filename, "w");
    if (!asm_out) {
        return 0; // Thất bại
    }
    return 1; // Thành công
}

// Hàm variadic đóng vai trò như printf chuyên dụng cho ASM
void asm_printf(const char *format, ...) {
    if (!asm_out) return;

    va_list args;
    va_start(args, format);
    
    // vfprintf nhận vào va_list và đẩy thẳng vào stream của file
    vfprintf(asm_out, format, args);
    
    va_end(args);
    
    // Tùy chọn: fflush(asm_out) nếu muốn dữ liệu ghi xuống đĩa ngay lập tức
}

// Đóng file stream sạch sẽ
void close_asm_emitter() {
    if (asm_out) {
        fclose(asm_out);
        asm_out = NULL;
    }
}