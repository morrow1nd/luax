

2017/04/18
 + [X]  [opcode] shift to one value: \(expr\)
 + [X]  [VM] continue statement
 + [ ]  [new function][parser] if-elseif statement
 + [ ]  [standard lib] next, prev(used to iterate a table's key-value)
 + [X]  [GC] collect key of a table
 + [ ]  [object] using a id to be the hash key?
 + [ ]  [doc] luax design document

2017/04/16
 + [ ]  function's arguments
 + [ ]  [parser] tab.true == tab["true"]
 + [X]  [GC] `return {};`

2017/04/17
 + [X]  recursive calling
 + [ ]  尾递归优化

2017/04/14
 + [X]  += -= *= /=
 + [X]  str + str
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

 + [ ]  use Stack Allocator in parser's frequently alloc&free. Stack allocator can be used at this situation just right.
 + [ ]  dynamic realloc in scan tokens: select one suitable LX_CONFIG_IDENTIFIER_MAX_LENGTH to fit big source code file and small source code file both.
 + [ ] remove prefix_op in BNF and code.
 + [ ] support `for local i = 0; ....`
 + [ ] support `for in...`
 + [X] fix bug of closure
