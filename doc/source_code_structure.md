# Source Code Structure


 + src/
    - config.h - Configure file
    - base.h&.c - Included by all other header file.
    - opcode.h&.c - Opcode declarations and some opcode utilities.
    - parser.h&.c - Achieve a token scanner and a recursive descent parser.
    - parser_callback.h&.c - Callback functions, which are called when parser deductes successfully. Opcode generator is located here!
    - mem.h&.c - Memory allocator.
    - object.h&.c - Luax object management.
    - vm.h&.c - Luax virtual machine.
    - luax_api.h&.c - Luax api for the communication between C and luax.
    - standard_lib/ - Running time environment.

    - luax_exec.c - A standalone executable interpreter.
    - luax_standard_syntax.c - A syntax-standardlization tool.

    - luax.h - All included in one.
    - luax.hpp - Wrapper for c++.