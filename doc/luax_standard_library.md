# Luax Standard Library

 Luax standard library provides basic functions for programmer using luax. Some functions of this library can only be achieved in c code, such as ... Other functions can be achieved in c and luax both. But we achieve them in c for performance's reason.


## Basic Functions

## Modules

#### require(str)
 + str: the name of a standard library or the path of a luax file
 + return: a table which contains the importing objects
```lua
local io = require("io");
io.print("hello\n");
```
#### import(table, str)
 + table: contains the importing objects
 + str: [@copy require]

```lua
import(_G, "io");
print("hello\n");
```
 Basically, `local io = require("io")` equals to `io = {}; import(_G.io, "io")`.


## String Manipulation

## Table Manipulation

## Mathematical Functions

## Input and Output Facilities
 Library name: io

#### io.format(format, ...)
 It functions the same as c `printf`.
 ```lua
io.format("%.2f, %4d, %s\n", 2.5621, 12, 'hi!');
--[[ it would return this string:
2.56, 12  , hi!
]]
 ```

#### io.print(str [, str])


## Operating System Facilities

## The Debug Library(todo)