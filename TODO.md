2016/12/20

 + [ ] use nginx-like string rather traditional c-str.

2016/12/21

 + [ ] support `for ; ; then end`. Here are two way to do it.
    1. for_stmt ->
          FOR expr ';' expr ';' expr THEN stmt_sequence END
          FOR ';' expr ';' expr THEN stmt_sequence END
          ...
    2. expr ->
              ε
            | ...
 + [ ] lost `local` statement...
 + [ ] use Stack Allocator in parser's frequently alloc&free. Stack allocator can be used at this situation just right.

 2016/12/22

  + [ ] support more feadom in object_immediate syntax.
        + [ ] consider this situation: `aa = { aa : {} };`, should we support it?
  + [ ] dynamic realloc in scan tokens: select one suitable LX_CONFIG_IDENTIFIER_MAX_LENGTH to fit big source code file and small source code file both.

2016/12/23

 + [X] support 'a string' type string.
 + [ ] add `local var_name;` support.
 + [ ] maybe we should change:
        ```
        single_expr ->
              '(' expr ')'
            | NIL
            | FALSE
            | TRUE
            | immediate
            | IDENTIFIER
        ```
        to 
        ```
        single_expr ->
              '(' expr ')'
            | NIL
            | FALSE
            | TRUE
            | IDENTIFIER
            | immediate
        ```
 + [ ] one thing to notice:
        ```
        assign_expr ->
            prefix_expr_list assign_op expr_list
          | logical_expr
        ```
        For this kind of rule, when it comes to this kind of source code: `55;`, it would function in this way:
        ```
        immediate  ->  NUMBER_IMMEDIATE
        single_expr  ->  immediate
        suffix_expr  ->  single_expr
        prefix_expr  ->  suffix_expr
        prefix_expr_list  ->  prefix_expr
        immediate  ->  NUMBER_IMMEDIATE
        single_expr  ->  immediate
        suffix_expr  ->  single_expr
        prefix_expr  ->  suffix_expr
        multiply_expr  ->  prefix_expr
        addtive_expr  ->  multiply_expr
        compare_expr  ->  addtive_expr
        logical_expr  ->  compare_expr
        assign_expr  ->  logical_expr
        expr  ->  assign_expr
        expr_stmt  ->  expr EOS
        stmt  ->  expr_stmt
        ```
        We can see that it try to use `assign_expr -> prefix_expr_list assign_op expr_list`, only when it comes to `assign_op`, can it finds that it's wrong, so it try to use second rule:`assign_expr -> logical_expr`. Before this, some callback funcitons have been called, this means it generates side-effect.