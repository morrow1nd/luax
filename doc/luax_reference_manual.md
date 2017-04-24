# Luax Reference Manual



## Introduction

 Luax is a dynamic type language, whose core concept is keeping things simple. If you have programming experience, you only need to know several concepts before using luax.



## Basic Concepts

### value and type

 There are six kinds of values in luax. **nil**, **bool**, **number**, **string**, **function**, **table**. Type **nil** only has one value `nil`. Type **number** means the value is a number, there is no *int number* in luax. **string** is real-only string. As for **table**, it's a container containing several key-values. The key of key-values can be any kind of types list above, so does it's value. **function** can be achieved by luax code or C code.

### variable

 Luax's variable can refer to any kinds of values, which is why it's called a dynamic type languages. Unlike lua, luax's variable must be declared before using. This means a inner function calling can't pollute the global environment. The called function can't add new variables to the caller's environment. Let's see a example.

 In lua, you can write this:
```lua
local func = function() g = 123 end
func()
print(g) -- 123
```
 But in luax, it would throw a error: *using undeclared variable*. Here is the right way:
```lua
local g;
local func = function() g = 123; end;
func();
print(g); -- 123
```

### table

 If you has learnt Javascript, you must be familiar with Object. Javascript's Object has properties and methods. But **table** is just a container containing several key-values.

 Basic usage of **table**:
```lua
local tab = {}; -- create a empty table
local tab1 = { 'key1' : 'value', 123 : 'value2', 'subtab' : {} };
tab['name'] = "table's name"; -- set key-value to a table
print(tab['name']); -- get the value by a specify key in a table
tab.name = 'new name'; -- dot operator is just a syntax-sugar. `tab.name` equals to `tab['name']`.

tab[tab1] = 'hello!'; -- a table can be used as a key
tab[nil] = 'using a nil as a key';
tab[true] = 'using a bool as a key';
```

#### meta table

 Every normal table has a meta table, the meta table's a table is recorded in `tab[tab]`. Yes, using itself as a key to get it's meta table. Meta table stores some **meta function**s.

 meta function:
 + "_get"  - function(tab, key):  called when get value from a table. `print(tab.name)`
 + "_set"  - function(tab, key, new_value):  called when set value to a table. `tab.name = 'new name'`
 + "_call"  - called when call a table. `tab[tab]._call = function(tab, arg1, arg2) ... end; tab(arg1, ang2);`
 + "_delete"  - called when GC collects this table

 When a table is created, it has a default meta table, the meta functions in default meta table are achieved by C code. You can define a function to override it. Meta function can be achieved by luax code or C code. Let's show how to create a read-only table.

```lua
local rtab = {};
local meta_tab = rtab[rtab]; -- get rtab's meta table
-- here, we use a inside function table_set to assign a new key-value to a table
table_set(meta_tab, '_set', function(tab, key, new_value) print('this table is readonly'); end);
rtab.name = 'name';
print(rtab.name); -- nil

-- recovery rtab to a normal table
table_set(meta_tab, '_set', table_set); -- yes, the default meta table use `table_set` as it's _set meta function.
rtab.name = 'new name';
print(rtab.name); -- new name
```

#### function

 In luax, function is first-class citizen. Luax provides only one way to create a function, which is expression `function(...) end`. Function achieved by luax code or C code are both the type **function**, which means you can't know whether a function is achieved by C code in luax code. Luax's function provides a variable named `arguments`. `arguments` holds all the real arguments as a array. The number of arguments is stored in `arguments.size`. `arguments` is useful when transport variable parameters.

 Examples:
```lua
-- define a function
local show_array = function(tab)
    local i = 0;
    while i < tab.size then
        print(tab[i]);
        i += 1;
    end
end;

--[[
  example: closure
  luax achieves closure in this way. when a function was created, it recorded 
  it's current namespace(I call it environment), when this function is called, 
  it can access the environment in runtime.
]]
local createCounter = function(init_number) 
    local _inner = init_number; 
    return function()  -- when create this function, it stores a reference to current environment
        _inner += 1;
        return _inner; 
    end; 
end;

local init_number = 5;
local counter = createCounter(init_number);
print(counter()); -- 6, when call this function, it will get `_inner` from the stored environment
print(counter()); -- 7
print(init_number); -- 5


--[[
  example: link several functions
]]
local func1 = function(arguments) show_array(arguments); print('call func1'); end;
local func2 = function(arguments) show_array(arguments); print('call func2'); end;
local add_functions = function()
    local funcs = arguments;
    return function()
        local i = 0;
        while i < funcs.size then
            funcs[i](arguments);
            i += 1;
        end
    end;
end;

local linked_func = add_functions(func1, func2);
linked_func(123, '456'); -- it will call func1, func2
--[[  output:
123.0
456
call func1
123.0
456
call func2
]]
```




## The Language

### value and type

 + **nil**  - has only one value `nil`
 + **bool**  - has two values `true` and `false`
 + **number**  - store real number(in math)
 + **string**  - read-only string
    - `'this is a string'`
    - `"this is another string"`
    - `"say 'hi', \t\n\r\\\""`
 + **table**  - container containing key-value mapping
    - `{}`
    - `{'key': 'value', 1 : 'value2'}`
    - `{ 'subtab' : { 0 : 123 }}`
 + **function**  - function achieved by luax code or C code

### conversion

 **number** and **string** can be converted automatically. For example:
```lua
print('abcdef' + 123);
local str = '2.1';
print(str * 5); -- 10.5
```

### expression

#### operators

type        | operators   
----------------------|--------------------
Arithmetic operators | `+ - * /` 
Relational operators | `< > <= >= == !=` 
Logical operators   | `and or not` 
Assignment operators | `= += -= *= /=`

 Yes, assignment is a expression. `a = ( b = 1 );` is the right luax code. `a = ( b, c = func(), 2, 3 );` equals to `b, c = func(), 2, 3; a = b;`. `func` is called only once.

#### table assess expression

```lua
local tab = {};
tab["key1"] = 1;
tab[3] = 'value2';
tab[true] = 'value3';
tab.key2 = {};
tab['key2']['key1'] = 123;
```

`tab.key2` is just a syntax-sugar of `tab["key2"]`. String after `.` operator must be named as a standard c identifier. For example, `tab.32` is wrong, and isn't equal to `tab['32']` or `tab[32]`.

#### function defination expression

 Yes, function defination is a expression. You can assign this expression to a variable, and then call it multi-times.

```lua
local func = function(a, b)
    return a + b;
end;
func(1, 2);

print(function(a, b)
    return a > b;
end(2, 1)); -- true
```

#### \(\) expression

 `func_return_multi_values()` is not equal to `(func_return_multi_values())`. Example:
```lua
local func = function() return 1, 2; end;
local a, b = func(); -- a = 1, b = 2
a, b = (func()); -- a = 1, b = nil
another_func(func(), "this should be the second argument"); -- actually, the second argument is number 2
```


### statement

#### the empty statement
```
;
```

#### declaration statement
Only after declare a variable, can this variable be used. This doesn't function the same as Lua.

```lua
local var;
local a, b, c;
local str = 'John';
local s1, s2, s3 = 'hi', 123; -- s3 -> nil

var_undeclared = 123;  -- error in luax. Lua treats it as a new global 
                      -- variable declaration and exposures it to everywhere. 
```

#### expression statement

```lua
5 + 3;
'position:' + position;
func();
funcRetFunc()();
tab['subtab']['key'];
funcRetTable().key;
```

#### selection statement

```lua
if condition_expr then
    statement_blocks;
end
if condition_expr then
    statements_blocks;
else
    statements_blocks;
end
```
#### Iteration statements
```lua
while condition_expr then
    statments_blocks;
    break;
    continue;
end
```
#### Jump statements
 * **The break statement**
 * **The continue statement**

`break` and `continue` function the same as c language.

 * **The return statement**

In luax, function can return multi values.
```lua
return; -- the same as `return nil;`
return a;
return a, b, c;
```


### namespace

```lua
local g = 1;
if true then
    print(g); -- 1
    local g; -- override it
    g = 2;
end
print(g); -- 1
-- while-statement functions the same as if-statement
```

 There is a inside variable `_E` in every namespace. `_E` refers to the current environment table. Yes, luax use a table to contain variables. Environment table use a different default meta table.
```lua
print(_E); -- dump _E to standard output to see what it contains.
print(_E._E == _E); -- true
```


### inside functions

 + typeof(obj)  - return the type of `obj` in string
 + meta_table(tab)  - return the meta table of tab
 + set_meta_table(tab, new_meta_table)
 + table_get(tab, key)  - raw get, don't use the "_get" function of tab's meta table
 + table_set(tab, key, value)  - raw set
 + table_exist(tab, key)  - return a **bool** to tell whether this tab contains this key
    ```lua
    local tab = { 0 : 123 };
    tab[1] = nil;
    print(table_exist(tab, 0)); -- true
    print(table_exist(tab, 1)); -- true
    print(table_exist(tab, 2)); -- false
    ```
 + table_next(tab, key)  - iterate over all the key-values in a table, example:
    ```lua
    local tab = {};
    tab.name = "I'm a table";
    tab[2] = "2";
    tab.key = "this is a key";

    local i = nil;
    while i = table_next(tab, i) then
        if i == tab then continue; end
        print('key: ' + i + ' value: ' + tab[i]);
    end
    ```
 + table_prev(tab, key)  - opposite direction
 + new_table(meta_table)  - create a table using the provided meta table
 + pcall(func, args...)  - call `func` in protected mode, example:
    ```lua
    local pcall_func = function(t, a, b)
        if t then throw('error'); end
        return a + b;
    end;
    local e, result = pcall(pcall_func, false, 1, 2); -- nil 3.0
    e, result = pcall(pcall_func, true, 1, 2); -- error nil
    ```
 + throw(exception)  - throw a exception
 + collectgarbage([opt [, arg]])  - see: <http://www.lua.org/manual/5.3/manual.html#pdf-collectgarbage>
 + require()  - load a standard library or a luax code file, example:
    ```lua
    local math = require('math');
    print(math.abs(-123)); -- 123.0
    print(math.int(1.5)); -- 1.0
    local user = require("data/users.luax");
    print(user); -- see test/require_test_users.luax
    ```
 + dostring(str)  - run luax code from `str` in protected mode, example:
    ```lua
    local code = "local a = 1; print(a); return 0; ";
    local e, res = dostring(code);
    if e then
        print('the luax code throws a exception ' + e);
    end
    print(res);  -- 0
    print(dostring('throw("error")')); -- error
    ```
 + dofile(filepath)  - run luax code in a file in protected mode


template debug functions:
 + print(obj)  - show one obj to standard output
 + dump_stack()  - dump runtime stack to standard output
 + emit_VS_breakpoint()  - emit visual studio breakpoint, so we can use visual studio's debug tool begin from here!
 + show_gc_info()  - show gc info to stdout


### luax standard libraries

 see [Luax Standard Libraries](./luax_standard_library.md)


## Luax API
[TODO]
