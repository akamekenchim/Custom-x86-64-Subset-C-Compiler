# declares the underlying compiler: GCC
CC = gcc

# System flags:
# -Wall -Wextra: prints all warnings to the terminal.
# -Iakame: 'Include-Akame', so the preprocessor knows to find header files in the akame folder
# -MMD -MP: processes the source code and automatically makes dependency files.
CFLAGS = -Wall -Wextra -g -Iakame -MMD -MP

# declares the target file name
TARGET = NttTrang

# source files
SRCS = main.c compiler.c lexer.c parser.c codegen.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

# make sure the target files wont physically produce files
.PHONY: all clean

# default target file
all: $(TARGET)

# Linking phase, creating the program target file
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