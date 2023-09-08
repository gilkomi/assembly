# Assembly Compiler
Final project for the "Systems Programming Laboratory" course at the Open University of Israel.<br>
This project aims to build an assembler for a 16-instruction assembly language for an imaginary 12-bit CPU with 12-bit & 1024-word memory size.<br>
Project author: gilkomi

## Description
There are many techniques to build a compiler.<br>
In this project, I combined both the AST and Two-Pass Compilation methods.<br>
One of the requirements of this project was to produce a base-16 coded list at the end.<br>

### AST (abstracts syntax tree)
The purpose of the AST phase is to take a line and convert it to the appropriate struct.<br>
This phase uses the modules: lexer and parser.<br>
* The **lexer** module receives an array of chars (string) representing a command line and knows how to transform the command line into a useful struct for reading.<br>
* The **parser** module's purpose is to re-read the struct and verify if the command line is logically correct. If not, the struct changes to an error type struct.<br>

### Two-Pass Compilation
The Two-Pass Compilation phase is divided into two stages, as the name suggests: from the first pass to the second pass.
* The **first-pass** phase receives the command line and uses the AST to convert it into the correct struct. Furthermore, this phase uses a symbols table to store information about each symbol that appears in the code. The table retains the name of the symbol, type of the symbol (declaration, use, external use), and the line number. At the end of this phase, we have a complete symbols table and a list containing all the 12-bit coded lines from the code, except for the symbols. Instead of getting the appropriate code for the symbol, there's a placeholder with the symbol's name.
* The **second-pass** phase uses the coded list and the symbols table. It re-reads the coded list, and wherever there's a placeholder, it scans the symbols table to rewrite the line with the correct 12-bit code.
  



## Directory Structure (Modules)
* `am_builder` - Converts `.as` files to `.am` format. Functions as a macro interpreter and removes comment lines. <br>
* `coded_list` - Consists of 12-bit code structs and their related functions. <br>
* `first_pass` - Implements the first phase of the Two-Pass Compilation technique. <br>
* `second_pass` - Implements the second phase of the Two-Pass Compilation technique. <br>
* `lexer.h` - Contains the definition of the Abstract Syntax Tree for a line in a source code. <br>
* `lexer` - Implements the lexer functionality of the assembler. <br>
* `parser` - Contains functions related to verifying if a command line is logically correct. <br>
* `symbol_table.h` - Contains the definitions for the symbols table. <br>
* `symbol_table` - Implements the functionalities of the symbols table. <br>
* `utils` - Provides functions for general use throughout the entire project. <br>
