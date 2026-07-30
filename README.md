# Custom x86-64 C Compiler in C

A lightweight, production-grade C compiler pipeline developed from scratch in C. This toolchain compiles an imperative C subset directly into standard x86-64 Intel-syntax Assembly (`.asm`), which can be assembled and linked into native binaries using GCC.

Designed with strict separation of concerns, the compiler features a hand-written lexer, a cascading recursive-descent parser, a semantic symbol table manager, and a stack-based x86-64 code generator fully compliant with the **Windows x64 ABI**.

---

## 🏛 Architecture & Pipeline

[ C Source Code (.c) ]
│
▼
[ Lexical Analyzer ] ───> Lexical Tokens
│
▼
[ Cascading Parser ] ───> Abstract Syntax Tree (AST)
│
▼
[ Semantic Analyzer ] ──> Symbol Table & Type Validation
│                (Type propagation, pointer/array scaling,
│                 scope resolution, break/continue validation)
▼
[ Assembly Generator ] ──> x86-64 Intel Assembly (.asm)
│                (RBP stack frames, ABI shadow space,
│                 movsx/movzx byte handling, loop label stacks)
▼
[ GCC Assembler/Linker ] ─> Executable Binary (.exe)


---

## ✨ Key Features

### 1. Front-End Parsing & Grammar
* **Cascading Recursive Descent Parser:** Multi-level operator precedence hierarchy enforcing standard C rules, including unary dereferencing (`*`), address-of (`&`), array indexing (`[]`), and explicit parenthesis overriding `()`.
* **Expanded Primitive & Derived Types:** 
  * Primitive types: `int` (4 bytes) and `char` (1 byte).
  * Pointer types: Single and multi-level pointers (`int*`, `char*`, `int**`).
  * Array types: Fixed-size 1D arrays with stack offset allocation (`int arr[10]`).
* **Advanced Control Flow:** Support for `if-else`, `while`, and `for` loops with arbitrary nesting depth, as well as `break` and `continue` control statements.

### 2. Semantic Analysis & Type System
* **Symbol Table Scope Tracking:** Tracks symbols across global and nested local block scopes (variables, pointers, arrays).
* **Pointer Arithmetic & Array Decay:** Automatically handles size scaling ($sizeof(T)$) during pointer addition/subtraction and evaluates base offset calculations for array element access.
* **Loop Context Stack:** Maintains a target label stack during AST traversal to validate that `break` and `continue` statements reside within valid loop constructs.
* **Semantic Error Trapping:**
  * **Redeclaration Error:** Prevents duplicate symbol declarations within the same scope.
  * **Undeclared Reference Error:** Halts compilation if an identifier is used prior to declaration.
  * **Type Mismatch Warnings/Errors:** Enforces type constraints between pointers, arrays, and scalar integers.

### 3. Low-Level x86-64 Code Generation
* **Intel Syntax Compliance:** Emits `.intel_syntax noprefix` output suitable for GCC/GAS toolchains.
* **Windows x64 ABI Alignment:** Manages 32-byte shadow space allocation, frame pointer (`RBP`) tracking, stack layout alignment (16-byte boundary), and target system calls (`printf`).
* **Byte-Level Memory Operations:** Emits `movsx` / `movzx` instructions for `char`-to-`int` widening and byte-register access (`AL`, `CL`, `BYTE PTR`).
* **Dynamic Loop & Label Resolution:** Generates unique labels for nested `for`/`while` loops and correctly routes `break` and `continue` jumps to their corresponding exit or increment basic blocks.

---

## 🛠 Supported Language Features & Syntax

| Category | Supported Syntax / Operators | Description |
| :--- | :--- | :--- |
| **Data Types** | `int`, `char`, `T*` (Pointers), `T[N]` (Arrays) | Scalar primitives, explicit pointer types, and contiguous stack arrays. |
| **Memory & Pointers** | `&var` (Address-Of), `*ptr` (Dereference), `arr[i]` | Address retrieval, pointer load/store, and scaled index offsets. |
| **Arithmetic** | `+`, `-`, `*`, `/`, `%` | Full precedence arithmetic evaluation. |
| **Bitwise & Shift** | `&`, `\|`, `^`, `~`, `<<`, `>>` | Bitwise logic and bit-shift operations. |
| **Relational & Logical** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Conditional comparison operators mapping to inverted jump instructions. |
| **Control Flow** | `if (...) {} else {}`, `while (...) {}`, `for (...) {}` | Full conditional branching and loop structures with arbitrary nesting. |
| **Jump Statements** | `break;`, `continue;` | Immediate loop termination or next-iteration dispatch. |
| **I/O Integration** | `printf(...)` | Native System V / Windows ABI call setup for dynamic string and scalar printing. |

---

## 🚀 Building & Running

### Requirements
* **Windows GCC Toolchain** (e.g., MinGW-w64)
* **Make Utility** (`mingw32-make`)

### Step-by-Step Execution

#### 1. Compile the Compiler
Clone the repository and build the binary via `Makefile`:
```bash
mingw32-make
```
*This produces the compiler executable `NttTrang.exe` along with intermediate object files.*

#### 2. Compile C Source Code to Assembly
Run the compiler against a target C source/test file (e.g., `test.txt`):
```bash
./NttTrang test.txt
```
*The compiler parses `test.txt` and writes the generated x86-64 code into `target.asm`.*

#### 3. Assemble, Link, and Run
Assemble `target.asm` using GCC and execute the resulting binary:
```powershell
gcc -x assembler -masm=intel target.asm -o target.exe ; if ($?) { .\target.exe }
```

---

### 💡 Key Highlights of the Update:
1. **Architecture & Pipeline:** Added pointer/array scaling, scope resolution, and `break`/`continue` context to the ASCII pipeline.
2. **Technical Details:** Added specific low-level x86 details like `movsx`/`movzx`, dynamic scale-factor arithmetic ($sizeof(T)$), and loop context stacks to reflect your low-level implementation choices accurately.
3. **Sample Code:** Updated `test.txt` in the README to showcase `for`, `while`, `break`, pointers (`*ptr`), and array indexing (`arr[i]`) simultaneously.