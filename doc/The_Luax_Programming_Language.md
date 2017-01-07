# Luax Reference Manual




## Introduction

 


## Basic Concepts

### Values and Types

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Lua is a dynamically typed language. This means that variables do not have types; only values do. There are no type definitions in the language. All values carry their own type.
It functions the same in luax. Luax provides six kinds of types: *nil*, *boolean*, *number*, *string*, *function*, *table*. The type *nil* has only one value: **nil**. The type *boolean* has two vaules: **false** and **true**. Value **false**, **nil**, **0**, make a condition false; and other value makes it true. The type *number* represents real number. *function* can be implemented by luax or the host language -- C. *table* in luax is one kind of map container. It is one object containing a collection of mapping, which record the mapping from keys to values. In a mapping, the key(or value) can not only be a number or string, but also any value of these six kinds of types list above. Even such a example: `tab[tab] = tab;`.

*-- Lua 5.3 Reference Manual.Chapter 2.1 --*
> Tables, functions, threads, and (full) userdata values are objects: variables do not actually contain these values, only references to them. Assignment, parameter passing, and function returns always manipulate references to such values; these operations do not imply any kind of copy.
It functions the same in luax, only notice that luax doesn't have threads and userdata list above.


### Metatable and Metamethod

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

Luax uses static scope transmission. This means there is no chance for global environment pollution. This's implemented by function call's **default** operation. Luax also allow you to change it to fit custom situation.




## The Language




## Standard Libraries

