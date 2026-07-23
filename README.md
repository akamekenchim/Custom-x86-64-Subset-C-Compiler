# Custom x86-64 C Compiler in C

A lightweight, production-grade Full-Pipeline C Compiler developed from scratch in C. This compiler converts a imperative C subset directly into standard x86-64 Intel-syntax Assembly (.asm), which can be assembled and linked into executable binaries via GCC.

Designed with strict separation of concerns, the toolchain features a lexer, a cascading recursive-descent parser, an independent semantic symbol table, and a stack-based x86-64 code generator compliant with the **Windows x64 ABI**.

---

# 🏛 Architecture & Pipeline

[ C Source Code (.c) ]
         │
         ▼
[ Lexical Analyzer ] ───> Tokens
         │
         ▼
[ Cascading Parser ] ───> Abstract Syntax Tree (AST)
         │
         ▼
[ Semantic Analyzer ] ──> Frontend Symbol Table Validation
         │                (Type propagation, redeclaration & missing var checks)
         ▼
[ Assembly Generator ] ──> x86-64 Assembly (.asm)
         │                (Stack frame offsets, ABI shadow space, inverted jumps)
         ▼
[ GCC Assembler/Linker ] ─> Executable Binary (.exe)
✨ Key Features
1. Front-End Parsing & Grammar
Cascading Recursive Descent Parser: 8+ level precedence hierarchy enforcing standard C operator precedence rules and recursive parenthesis evaluation ( ).
Explicit Type System: Explicit type declaration support (int x = 10; and int x;).
Nested Control Flow: Infinite nesting support for if-else branches and while loops.
2. Semantic Analysis & Validation
Frontend Symbol Table: Tracks variable scopes, names, and explicit DataType attributes across the AST.
Semantic Error Trapping:
Redeclaration Error: Detects duplicate symbol declarations in the same scope.
Undeclared Variable Error: Halts compilation if a variable is referenced or assigned prior to declaration.
3. Low-Level x86-64 Code GenerationIntel Syntax Compliance: Generates .intel_syntax noprefix output.
Windows x64 ABI Alignment: Handles 32-byte shadow space stack allocation, RBP frame pointer tracking, and stack alignment for external standard library calls (printf).
Inverted Jump Mapping: Dynamically resolves relational operators into optimized inverted conditional branches (jg, jge, jl, jle, je, jne) to prevent dead-code execution in while and if-else blocks.
Dynamic .rodata Management: Automatically extracts string literals into a read-only data section with RIP-relative addressing ([rip + .L_str_X]).🛠 Supported Language Features & OperatorsCategorySupported Syntax / Operators
Data Types
int (Explicit declarations & assignments)
Arithmetic: Addition (+), Subtraction (-), Multiplication (*), Division (/), Modulo (%)
Bitwise & Shift: Bitwise AND (&), Bitwise OR (|), Left Shift (<<), Right Shift (>>)
Relational: Equal (==), Not Equal (!=), Less Than (<), Greater Than (>), Less or Equal (<=), Greater or Equal (>=)
Control Flow: if (...) { ... } else { ... }, while (...) { ... }I/O 
Integration: Native printf(...) support for integers and string literals
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# To run the Compiler:
Requirements: Windows GCC Compiler (MinGW64 folder).
Makefile Extension for VSCode

(1) After cloning the repository, run this command in Terminal:

mingw32-make 

This creates dependency files, .o files and executable.
Use the testcase provided, or you can import your own code (doesn't support function declaration yet, just simple stuff like nested if, variable declarations, 
while loops, arithmetic/logical/bitwise operations).
The example code is provided in test.txt

(2) Run:

./NttTrang test.txt

This command runs the output executable file with the test.txt (example code) as command line argument.\
The generated Assembly x86 code is then written into 'target.asm' file.

(3): Run:

gcc -x assembler -masm=intel "c:\Projects\C_C++_Practice\target.asm" -o .\target.exe ; if ($?) { .\target.exe }

This command uses the GCC Compiler to create an executable out of 'target.asm' then runs it.


