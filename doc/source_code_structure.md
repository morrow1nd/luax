# Source Code Structure


 + src/
 	- opcode.h&.c - Opcode declarations and some opcode utilities.
 	- parser.h&.c - Achieve a token scanner and a recursive descent parser.
 	- parser_callback.h&.c - Callback functions, which are called when parser deductes successfully.
 	- mem.h&.c - Memory allocator.
 	- vm.h&.c - Luax virtual machine.
 	- standard_lib/ - Running time environment.

 	- luax_exec.c - A standalone executable interpreter.
 	- luax_standard_syntax.c - A syntax-standardlization tool.

 	- luax.h - All included in one.
 	- luax.hpp - Wrapper for c++.