# Luax Standard Library

 Luax standard library provides basic functions for programmer using luax. Some functions can only be achieved in c code, such as `math.int`. Other functions can be achieved in c and luax both. But we achieve them in c for performance's reason.


 [TODO]

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