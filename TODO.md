2016/12/21

 + [ ]  support `for ; ; then end`. Here are two way to do it.
    ```
    1. for_stmt ->
          FOR expr ';' expr ';' expr THEN stmt_sequence END
          FOR ';' expr ';' expr THEN stmt_sequence END
          ...
    2. expr ->
              ε
            | ...
    ```
 + [ ]  use Stack Allocator in parser's frequently alloc&free. Stack allocator can be used at this situation just right.

 2016/12/22

  + [ ] support more freedom in object_immediate syntax.
        + [ ] consider this situation: `aa = { aa : {} };`, should we support it?
        + [ ] we don't support `str = 'key1'; tab = { str : 'value1' }; ` now.
  + [ ] dynamic realloc in scan tokens: select one suitable LX_CONFIG_IDENTIFIER_MAX_LENGTH to fit big source code file and small source code file both.

2016/12/23
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

2017/01/20
 + [ ] remove prefix_op in BNF and code.
2017/01/21
 + [ ] support `for local i = 0; ....`

2017/01/23
 + [ ] support `for in...`
 
2017/04/10
 + [X] fix bug of closure


2017/04/14
 + [ ]  += -= *= /=
 + [ ]  str + str
 + [X]  table init: {a: 1, str:"aaa", subtab: {'test': num }}
 + [X]  exception and restore
 + [X]  consider `try catch` or provide a inside function `pcall`:
        ```
        local doThing = function(a, b) ... return d, e; end;
        local exception, d, e = pcall(doThing, a, b); -- call this function in protect mode.
        ```
 + [X]  provide a inside function `throw`
        ```
        local inner_doThing = function(a) if ... then throw(something); end end;
        local doThing = function(a, b) inner_doThing(a); return d, e; end;
        local exception, d, e = pcall(doThing, a, b); -- exception == something
        ```
 + [X]  GC

2017/04/16
 + [ ]  function's arguments
 + [ ]  [parser] tab.true == tab["true"]
 + [ ]  [GC] `return {};`

2017/04/17
 + [X]  recursive calling
 + [ ]  尾递归优化

2017/04/18
 + [ ]  [opcode] shift to one value: \(expr\)
 + [ ]  [VM] continue statement
 + [ ]  [parser] if-elseif statement
 + [ ]  [standard lib] next, prev(used to iterate a table's key-value)
 + [ ]  [GC] collect key of a table
 + [ ]  [object] using a id to be the hash key
 + [ ]  [doc] luax design document