# Luax Design Document

 - [opcode explanation](./opcode.md)
 - [luax syntax BNF](./BNF.txt)
 - [source code structure](./source_code_structure.md)
 - test luax code
    + [opcode generator test](../test/generate_bytecode_test.luax)
    + [Virtual Machine test](../test/vm_run_test.luax)


## syntax parser
 
 - [Luax syntax BNF](./BNF.txt)

 Luax uses a simple token scanner and recursive descent parser. These codes are located at [parser.c](../src/parser.c). These functions of every nonterminal symbols are named the same as [BNF.txt](./BNF.txt), which makes it easy to view the whole code. When parser find one rule, it would call the callback function of this rule. These callback functions are located at [parser_callback.c](../src/parser_callback.c).


## opcode explanation and opcode generator

 - [opcode explanation](./opcode.md)



## VM

### (namespace)environment

### environment table's meta functions

### garbage collection





## run-time environment

luax complier translate luax code to one kind of middle code, we call it opcode. luax virtual machine run commands in opcode one by one. The luax statement is runed in a default function, created by the VM. It's very important to know that every luax statement runs in a luax function. A function has these things hanged on it:
 - a `table` object used to store local variable
 - a `table` reference used to link to it's father environment
When a function was created, it store it's current environment as it's father environment. Every time this function was called, it created a table used to store it's local variable, and in the same time, we can access it's father environment(these variables when it was created). We use this to achieve **closure**.


#### sequence in stack
**forward**: first one first in. For example, `func(1, 2)` => `push 1; push 2`
**reverse**: 

            situation               |   type  |   explain
------------------------------------|---------|------------------------------------------------------------
 `func(1, 2)`                       | reverse |
 `return 1, 2`                      | reverse | `func = function() return 1, 2; end; local a, b = func();`
 `local a, b`                       | reverse | 
 `local a, b = 1, 2, 3`             | reverse | 

