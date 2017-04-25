![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)

# What is Luax?

 Luax is a easy-to-learn, concise and powerful programming language. ([Chinese Page](./doc/doc-zh/README.md))

 Luax provides full documentation(from the language itself to internal design), making it a perfect project for beginner to learn how to make a *interpreter*.

 Luax is distributed in source code, which contains the bytecode generator, luax virtual machine, standard library, a standalone executable interpreter, and full documentation.


# Documentation

## For Language User

 + [Get Started](./doc/get-started.md) - Setup a working environment.
 + [5-minites-tour](./doc/5-minites-tour.md) (Coming soon!)
 + [Luax Reference Manual](./doc/luax_reference_manual.md)

## For Language Hacker

 + [Luax Design Document](./doc/luax_design_document.md)  (Coming soon!)


# Features

The luax programming language itself:
 + easy-to-learn: [5-minites-tour](./doc/5-minites-tour.md)
 + powerful data description constructs

The C achieve of luax:
 + lightwight arch: [source code structure](./doc/source_code_structure.md)
 + using subset of c++ and c
 + full-commented

 Here is a list:
  * dynamic type language (*nil*, *bool*, *number*, *string*, *function*, *table*)
  * basic statements (*variable-declaration*, *if/while*, *break*, *continue*, *function-define*)
  * mutli-assign, multi-return  - `a, b = 1, 2;` `function(a, b) return a+b, a-b; end`
  * *table*  - a container containing several key-values. The type of key or value can be any one of those types listed above.
  * meta table  - a table defining what would happens when specific action made to a table(such as: get set call), example: [*read-only table*](https://github.com/morrow1nd/luax/blob/master/doc/luax_reference_manual.md#meta-table) 
  * function  - first-class citizen, example: [*link several functions*](https://github.com/morrow1nd/luax/blob/master/doc/luax_reference_manual.md#function) 




# Examples

```lua
-- table: a container containing several key-values.
local tab = { 'key' : 'value', 1 : false };
tab.name = "I'm a table";
tab["func"] = function() return "I can hold a function"; end; -- tab["func"] equals to tab.func
tab[true] = "the type of my key can be a boolean";
tab[another_table] = "or a table";
--[[
  luax has six kinds of type: nil, bool, number, string, table, function. Table's key-value can be any kind of these types.
]]


-- function
local func = function(a, b) -- define a function
    return a + b, a - b;
end;
tab.a, tab.b = func(1, 2); -- return 3, -1

print(func(1, 2), 2);   -- 3, -1, 2
print((func(1, 2)), 2); -- 3, 2

local func_sum = function(a, b)
    local i, sum = 0, 0;
    while i < arguments.size then  -- every function has a variable named arguments
        sum += arguments[i];
        i += 1;
    end
    return sum;
end
print(func_sum(1, 2, 3)); -- 6
print(func_sum(1)); -- 1, argument b equals to nil now
--[[ 
  See more examples about function from doc/luax_reference_manual.md#function
]]
```


# Build

```shell
mkdir build && cd build
cmake ..
make && make test
# find more platforms from [Get Started]
```
