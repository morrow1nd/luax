# luax design document

 - [opcode explanation](./opcode.md)
 - [luax syntax BNF](./BNF.txt)
 - [source code structure](./source_code_structure.md)
 - test luax code
    + [opcode generator test](../test/generate_bytecode_test.luax)
    + [Virtual Machine test](../test/vm_run_test.luax)


## value

 - nil
 - boolean
 - number - real number in math
 - string - read only string
 - table
 - function
 - user define data ?


## meta table

meta functions:
 + "_get"  - function(tab, key)
 + "_set"  - function(tab, key, new_value)
 + "_call"  - tab[tab]._call = function(tab, arg1, arg2) ... end; tab(arg1, ang2);
 + "_delete" - function(tab)

meta elements in environment table:
 + "_father_env"  - point to this env_table's father env_table


Every table has a meta table, you can access the meta table by `tab[tab]` or `meta_table(tab)`. A table's meta table is assigned to a default meta table. This default meta table has no meta table, this means if you want to change value of this table, you can't use normal way, you must use `table_set` and `table_get`. For example:

```lua
local tab = {};
local meta_tab = meta_table(tab); -- or: local meta_tab = tab[tab];
meta_tab["_get"] = function(tab, key) ... end; -- error
--[[ meta_tab has no meta table, but `meta_tab["_get"]` means 
calling the "_get" function of it's meta table. So, if you want to modify 
meta_tab, you only can use `table_set` and `table_get`.
]]
table_set(meta_tab, "_set", function(tab, key, value) print('this table is read-only'); end);
tab["name"] = "new name"; -- output 'this table is read-only'
```


## run-time environment

luax complier translate luax code to one kind of middle code, we call it opcode. luax virtual machine run commands in opcode one by one. The luax statement is runed in a default function, created by the VM. It's very important to know that every luax statement runs in a luax function. A function has these things hanged on it:
 - a `table` object used to store local variable
 - a `table` reference used to link to it's father environment
When a function was created, it store it's current environment as it's father environment. Every time this function was called, it created a table used to store it's local variable, and in the same time, we can access it's father environment(these variables when it was created). We use this to achieve **closure**.


#### sequence in stack
**forward**: first one first in. For example, `func(1, 2)` => `push 1; push 2`
**reverse**: 

            situation               |   type  |   explain
------------------------------------|---------|------------
`func(1, 2)`                        | reverse |
`return 1, 2`                       | reverse | `func = function() return 1, 2; end; local a, b = func();`
`local a, b`                        | reverse |
`local a, b = 1, 2, 3`              | reverse |



**inside functions**:
 + meta_table(tab)  - return the meta table of tab
 + set_meta_table(tab, new_meta_table)
 + table_get(tab, key)  - raw get, don't use the "_get" function of tab's meta table
 + table_set(tab, key, value)  - raw set
 + new_table(meta_table)  - create a table using the provided meta table

**template debug functions**:
 + print(obj)  - show one obj to standard output
 + dump_stack()  - dump runtime stack to standard output
 + emit_VS_breakpoint()  - emit visual studio breakpoint, so we can use visual studio's debug tool begin from here!
