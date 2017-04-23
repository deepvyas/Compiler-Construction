# Compiler-Construction
Toy Compiler for ERPLAG-Eradicate Plagiarism, a language inspired from C,but a highly stripped down version of it. This project was built as a part of partial fulfillment of course *Compiler Construction* conducted by *Dr.Vandana Agarwal*.


This project has been been implemented in a pipeline manner with stages consisting of:
* Lexer
* Parser
* AST generation
* Typechecking & Semantic checking
* Code Generation (NASM 64-bit compatible assembly).

**Authors**
* [Rishabh Joshi](https://github.com/rishabhjoshi)
* [Deep Vyas](https://github.com/deepvyas)

## Usage

```bash
    make
    ./compiler <erplag_file> <asm_output_file>
    nasm -f elf64 <asm_output_file> -o <output_object_file>
    gcc -m64 <output_object_file> -o <output_executable>
    ./<output_executable>
```
