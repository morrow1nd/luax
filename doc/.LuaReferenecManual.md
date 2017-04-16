# Luax Reference Manual




## Introduction



## Basic Concepts

### Values and Types

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Lua is a dynamically typed language. This means that variables do not have types; only values do. There are no type definitions in the language. All values carry their own type.
It functions the same in luax. Luax provides six kinds of types: *nil*, *bool*, *number*, *string*, *function*, *table*. The type *nil* has only one value: **nil**. The type *bool* has two vaules: **false** and **true**. Value **false**, **nil**, **0**, make a condition false; and other value makes it true. The type *number* represents real number. *function* can be implemented by luax or the host language -- C. *table* in luax is one kind of map container. It is one object containing a collection of mapping, which record the mapping from keys to values. In a mapping, the key(or value) can not only be a number or string, but also any value of these six kinds of types list above. Even such a example: `tab[tab] = tab;`.

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Tables, functions, threads, and (full) userdata values are objects: variables do not actually contain these values, only references to them. Assignment, parameter passing, and function returns always manipulate references to such values; these operations do not imply any kind of copy.
It functions the same in luax, only notice that luax doesn't have threads and userdata list above.

### Variables


### Metatable and Metamethod
:todo
Maybe you are familiar with the operator overload of c++. Luax provides the same thing. Without operator overload, it's hard to use smart pointer and more... Luax's metatable can provide the ability to overload normal arithmetic operators, logical operators, control the authority of read or write, custom the operation when function was called, use cpp-like RAII to manage resources(not only memory), simulate class inheritance and javascript's prototype and so on.

Luax provides these kinds of control of metatable:
 + **add**:
 + **sub**:
 + **mul**:
 + **div**:
 + **and**:
 + **or**:
 + **not**:
 + **equal**:
 + **not_equal**:
 + **less**:
 + **greater**: *please make sure `a < b`, `b > a`, `not a >= b` and `not b <= a` return the same value.*
 + **less_equal**:
 + **greater_equal**:
 + **index_read**:
 + **index_write**:
 + **__index**:
 + **call**:
 + **new**:
 + **delete**:

Creating a metatable is easy.

```lua
createMatrix = function()
    ret = {};
    ...

    ret[ret].add = function(ls, rs)
        //... add two matrices, then return result
    end;
    ret[ret].sub = ...
    return ret;
end;

matrix = createMatrix();
matrix2 = createMatrix();
matrix3 = matrix + matrix2;
```

For every table, luax uses itself as *key* to get it's metatable. For example, we have `tab = {};`. Luax create not only the table itself, but also it's metatable `tab[tab]`. This means code `tab = {};` equals to these code:
```lua
tab = createOneObjectWithNoMetatable();
tab[tab] = createOneObjectWithNoMetatable();
tab[tab].index_read = function(key)
    return tab[key];
end;
tab[tab].write_read = function(key, value)
    tab[key] = value;
    return value;
end;
```

### Environments

Actualy, there is no *variable* in luax. When you write this sentence `foo = 123;`, luax would explain it as `_E['foo'] = 123;`. `boo = foo;` ==> `_E['boo'] = _E['foo'];`. As you can image, `_E['_E']` is equal to `_E`. Every function has a implict argument `_E`, which means when this function is called, it would use this implict argument as it's environment. Defaultlly, luax would use curr environment as this functon's environment. By this way, luax achieve the traditional programming language's *nested scope*. 
But there is only one thing that is different -- luax uses a 独特的 scope transmitation. Every time you call a funcion in the default way, luax would do this thing:
1. create a table `new_E` as this function's environment
2. change **index_read** of `new_E`
3. backup current environment `_E`'s **index_read** **index_write**
4. set `_E` to a *no-enlarge mode* by change it's **index_read** and **index_write**
```lua
_E[_E].index_read = function(key)
    _E
end;
_E[_E].index_write = function(key, value)

end;
```
The environment `_E` passed to the called function has been reset to a *no-enlarge mode* before run the function's body, and 

Every time you call a function, such as `func`, luax would defaultlly use the curr environment as this function's environment. So you can write code as usually:
```lua
g_num = 1;
func = function(a)
    return a + g_num;
end;
func(1); // 2
```


```lua
foo = 123;

```

delete''''''Luax uses static scope transmission. This means there is no chance for global environment pollution. This's implemented by function call's **default** operation. Luax also allow you to change it to fit custom situation.
''''''



## The Language


### Values and Types

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Lua is a dynamically typed language. This means that variables do not have types; only values do. There are no type definitions in the language. All values carry their own type.
It functions the same in luax. Luax provides six kinds of types: *nil*, *bool*, *number*, *string*, *function*, *table*. The type *nil* has only one value: **nil**. The type *bool* has two vaules: **false** and **true**. Value **false**, **nil**, **0**, make a condition false; and other value makes it true. The type *number* represents real number. *function* can be implemented by luax or the host language -- C. *table* in luax is one kind of map container. It is one object containing a collection of mapping, which record the mapping from keys to values. In a mapping, the key(or value) can not only be a number or string, but also any value of these six kinds of types list above. Even such a example: `tab[tab] = tab;`.

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Tables, functions, threads, and (full) userdata values are objects: variables do not actually contain these values, only references to them. Assignment, parameter passing, and function returns always manipulate references to such values; these operations do not imply any kind of copy.
It functions the same in luax, only notice that luax doesn't have threads and userdata list above.


### Variables

*** maybe this should be placed at `Luax Hacher Doc`.

Actually, there are no variables in luax.
```lua
local main = function()
    local a = 1;
    local func = function()
        console.log('a = ' + a);
    end;

    func();
    a = 5;
    func();
end;
main();
```

Every statement is in a function. Luax generates a first function to run the code you input or from a luax source code file. Every function has a table called `_G`, we use this table to record the variables you declare. Actually, everything you do is just making change to the table `_G`.



### Conversions


### Expressions

#### Operators

          type        |    operators 
----------------------|-----------------
 Arithmetic operators | `+ - * /`
 Relational operators | `< > <= >= == !=`
  Logical operators   | `and or not`

#### The table assess expression

```lua
local tab = {};
tab["key1"] = 1;
tab[3] = 'value2';
tab[true] = 'value3';
tab.key2 = {};
tab['key2']['key1'] = 123;
```

`tab.key2` is just a syntax-sugar of `tab["key2"]`. String after `.` operator must be named as a standard c identifier. For example, `tab.32` is wrong, and isn't equal to `tab['32']` or `tab[32]`.

#### The function defination expression
Yes, function defination is a expression. You can assign this expression to a variable, and then call it multi-times.
```lua
local func = function(a, b)
    return a + b;
end;
func(1, 2);

local compare_result = function(a, b)
    return a > b;
end(1, 2);
```


### Statements

#### The empty statement

```
;
```

 Luax allows this kind of syntax:
```lua
if expr then
    ;  -- use a empty statement
else
       -- or nothing
end
```

#### Declaration statements
Only after declare a variable, can this variable be used. This doesn't function the same as Lua.

```lua
local var;
local a, b, c;
local str = 'John';
local s1, s2, s3 = 'hi', 123; -- s3 -> nil

varUndeclared = 123;  -- error in luax. Lua treats it as a new global 
                      -- variable declaration and exposures it to everywhere. 
```

#### Expression Statements

```lua
5 + 3;
'position:' + position;
func();
funcRetFunc()();
```
Actually, assignment is also a expression.
```lua
a = (b, c = 123, 456); -- equals to `b, c = 123, 456; a = b, c;`
io.print(a); -- 123
```

#### Selection statements
 * **if statements**
```lua
if condition_expr then
    statement_blocks;
end
```
```lua
if condition_expr then
    statements_blocks;
else
    statements_blocks;
end
```
#### Iteration statements
 * **while statements**
```lua
while condition_expr then
    statments_blocks;
    break;
    continue;
end
```
 * **for statements**
```lua
for expr; condition_expr; expr then
    statements_blocks;
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



### Namespaces

### Luax Standard Libraries

 see [standard libraries documentation](./luax_standard_library.md)



## Luax API

