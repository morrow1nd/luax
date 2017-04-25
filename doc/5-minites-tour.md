# 5-Minites-Tour


# Basic Concepts

+ *everything is an object* - a function, a string, a number ...
+ *every object has a **type**
+ variable must be declared before using  `local a;` `local b = 1;`
+ *table* - a container containing several key-values, key and value can be any type


# type

  there are six kinds of types in luax.

 + **nil**  only has one value `nil`. It's something you can use as a Java's `null`
 + **bool**  `true` or `false`
 + **number**  real number in math
 + **string**  `'this is a string'`  `"this is another 'one'"`
 + **function**  `function(...)  ... end`
 + **table**  `{}`  `{"key" : 123}`


# table

 + create a empty table `local tab = {};`
 + set key-value to table
    - `tab.name = 'this is my name';`
    - `tab["name"] = 'I change my name';`
    - `tab[false] = 'anything can be a key';`
    - `tab[nil] = tab;` *anything can be a value*
 + read key-value from table
    - `print(tab.name); print(tab[false]);`
    - `print(tab.no_exist);` would print `nil`
 + iterate all key-values, using table_next or table_prev
    ```lua
    local i = nil;
    while i = table_next(tab, i) then
        if i == tab then continue; end
        print('key: ' + i + ' value: ' + tab[i]);
    end
    ```


# function

```lua
local func = function(a, b) return a+b, a-b; end;  -- function can return several values
print(func(2, 1)); -- 3, 1

local a, b, c = func(2, 1, 3);  -- multi-assign
print(a, b, c); -- 3, 1, nil

local func_sum = function()
    local i, sum = 0, 0;
    while i < arguments.size then  -- every function has a variable named arguments
        sum += arguments[i];
        i += 1;
    end
    return sum;
end;
print(func_sum(1, 2, 3)); -- 6
```


# other statements

```lua
if condition_expr then
    <statement_blocks>
end
if condition_expr then
    <statements_blocks>
else
    <statements_blocks>
end

while condition_expr then
    <statments_blocks>
    break;
    continue;
end
```


# more
[Lua Reference Manual](./luax_reference_manual.md)
