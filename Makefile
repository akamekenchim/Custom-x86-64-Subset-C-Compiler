# 1. Trình biên dịch
CC = gcc

# 2. Cờ biên dịch (Compiler Flags)
# -Iakame : Chỉ thị cho Preprocessor (Bộ tiền xử lý) tìm kiếm file header (.h) trong thư mục 'akame'
# -MMD -MP: Tự động phân tích và sinh ra các file .d chứa dependency của từng file .o
CFLAGS = -Wall -Wextra -g -Iakame -MMD -MP

# 3. Tên file thực thi sản sinh ra
TARGET = NttTrang

# 4. Quản lý danh sách file nguồn
SRCS = main.c compiler.c lexer.c parser.c codegen.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

# 5. Các target không sinh ra file vật lý
.PHONY: all clean

# Target mặc định
all: $(TARGET)

# Quy tắc liên kết (Linking phase) để tạo ra NttTrang
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Quy tắc biên dịch (Compilation phase): Tự động build %.o từ %.c tương ứng
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 6. Include các file dependency tự động sinh (.d)
-include $(DEPS)

# Quy tắc dọn dẹp
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)