<div class="out-div" style="font-size: 16px;line-height: 40px;display: inline-block;height: 40px;padding-left: 50px;padding-right: 50px;text-align: center;border-radius: 5px;margin-left: 32px;vertical-align: top;background-color: #fff6bf;">
    <span><b>注意</b>：同步于<a href="../luax_reference_manual.md" style="text-decoration: none">英文版</a></span>
</div>


# Luax手册




## 介绍

 Luax是一种动态类型语言，它的核心概念是简单。如果你有其他语言的编程经验，你只需要学习几个概念就可以开始使用luax了。




## 基本概念

### 值(value)和类型(type)

 Luax中有六种类型的值：**nil**, **bool**, **number**, **string**, **function**, **table**。类型**nil**只有一个值`nil`。类型**number**代表数字，在luax中没有整形数。**string**是只读的字符串。至于说**table**，它是一个盛装key-value的容器。key-value的key可以是上面列举的六种类型中的任意一种，value也同样。**function**代表着使用luax代码或C代码实现的函数。

### 变量(variable)

 Luax的变量可以引用任何一种值，这也是它为什么是动态类型语言的一个原因。不同于lua，luax的变量必须在使用前被声明。这意味着调用一个函数不会污染全局空间。被调用的函数不能添加新的变量到调用者的环境中。下面举例说明。

 在lua中，可以这样写：
```lua
local func = function() g = 123 end
func()
print(g) -- 123
```
 但是在luax中，上面的代码会抛出一个异常：*using undeclared variable*。下面是在luax中的使用方式：
```lua
local g;
local func = function() g = 123; end;
func();
print(g); -- 123
```

### table

 如果你学习过JavaScript，你一定对对象熟悉。JavaScript的对象具有属性和方法。但是**table**仅仅是一个容器，盛装着多个键值对。

 **table**的基本使用:
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

#### 元表(meta table)

 每一个一般的表都具有元表，一个表的元表记录在`tab[tab]`中。是的，用这个表自己作为键获取到的值就是它的元表。元表中具有多个元函数(meta function)。

 meta function:
+ "_get"  - function(tab, key):  当从表中获取值时被调用。 `print(tab.name)`
+ "_set"  - function(tab, key, new_value):  当将新的值写入到表中时被调用。 `tab.name = 'new name'`
+ "_call"  - 当把表当做一个函数一样调用时被调用。 `tab[tab]._call = function(tab, arg1, arg2) ... end; tab(arg1, ang2);`
+ "_delete"  - 当这个表被垃圾回收机制回收掉时被调用

 当一个表被创建时，这个表具有一个默认的元表，其中的元函数是用C代码实现的。你可以定义一个新的函数当做元函数。元函数可以使用luax代码或C代码实现。下面演示创建一个只读的表。

```lua
local rtab = {};
local meta_tab = rtab[rtab]; -- 获得rtab的元表
-- 使用内置函数tab_set来修改表中的键值对
table_set(meta_tab, '_set', function(tab, key, new_value) print('this table is readonly'); end);
rtab.name = 'name';
print(rtab.name); -- nil

-- 使rtab变成一个正常的表
table_set(meta_tab, '_set', table_set); -- 是的，默认的元表使用table_set作为'_set'对应的元函数
rtab.name = 'new name';
print(rtab.name); -- new name
```


#### 函数(function)

 在luax中，函数是一等值。使用表达式`function(...) ... end`来创建函数。使用luax或C语言实现的函数都具有类型**function**，这意味着在luax中不能区分他们两者。luax的函数有一个名为`arguments`的变量，它盛装实际的参数。`arguments.size`代表参数的个数。

 例子：**简单的闭包示例**,  **link serveral functions**
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




## 语言

### 值和类型

 + **nil** 只有一个值`nil`
 + **bool** 有两个值`true` and `false`
 + **number** 实数
 + **string** 只读字符串
    - `'this is a string'`
    - `"this is another string"`
    - `"say 'hi', \t\n\r\\\""`
 + **table** 盛装键值对的容器
    - `{}`
    - `{'key': 'value', 1 : 'value2'}`
    - `{ 'subtab' : { 0 : 123 }}`
 + **function** 使用luax代码或C代码实现的函数

### 转换(conversion)

 **number** 和 **string** 会自动相应转换，例如:
```lua
print('abcdef' + 123);
local str = '2.1';
print(str * 5); -- 10.5
print("not a number" * 5); -- 抛出一个错误
```

### 表达式

#### 操作符

类型        | 操作符   
----------------------|--------------------
算数操作符 | `+ - * /` 
关系操作符 | `< > <= >= == !=` 
逻辑操作符 | `and or not` 
赋值操作符 | `= += -= *= /=`

 是的，赋值是一种表达式。`a = ( b = 1 );`是正确的luax代码。`a = (b, c = func(), 2, 3);`等价于：`b, c = func(), 2, 3; a = b;`。`func`只是被调用了一次。


#### 表读写操作符

```lua
local tab = {};
tab["key1"] = 1;
tab[3] = 'value2';
tab[true] = 'value3';
tab.key2 = {};
tab['key2']['key1'] = 123;
```

 `tab.key2`仅仅是`tab["key2"]`的语法糖。`.`操作符后的字符串必须是一个标准C标识符。例如：`tab.32`是错误的，并且它不等于`tab['32']`或`tab[32]`。

#### 函数定义表达式

 是的，函数定义是一个表达式。你可以将这个表达式赋值到一个变量，之后调用多次。

```lua
local func = function(a, b)
    return a + b;
end;
func(1, 2);

print(function(a, b)
    return a > b;
end(1, 2)); -- false
```

#### \( expression \) 

 `func_return_multi_values()` 不等同于 `(func_return_multi_values())`. 例子:
```lua
local func = function() return 1, 2; end;
local a, b = func(); -- a = 1, b = 2
a, b = (func()); -- a = 1, b = nil
another_func(func(), "this should be the second argument"); -- actually, the second argument is number 2
```



### 语句

#### 空语句
```
;
```

#### 声明语句
只有在声明之后，变量才可以被使用。这一点和lua不同。

```lua
local var;
local a, b, c;
local str = 'John';
local s1, s2, s3 = 'hi', 123; -- s3 -> nil

var_undeclared = 123;  -- error in luax. Lua treats it as a new global 
                      -- variable declaration and exposures it to everywhere. 
```

#### 表达式语句

```lua
5 + 3;
'position:' + position;
func();
funcRetFunc()();
tab['subtab']['key'];
funcRetTable().key;
```

#### 选择语句

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
#### 循环语句
```lua
while condition_expr then
    statments_blocks;
    break;
    continue;
end
```
#### 跳转语句
 * **break语句**
 * **continue语句**

`break` 和 `continue` 的作用和C中的一样。

 * **return语句**

在luax中，函数可以返回多个值。
```lua
return; -- 和`return nil;`一样
return a;
return a, b, c;
```

### 作用域

```lua
local g = 1;
if true then
    print(g); -- 1
    local g; -- 覆盖之前的g
    g = 2;
end
print(g); -- 1
-- while-statement和if-statement一样
```

 在每一个作用域中有一个内置变量`_E`，`_E`引用这个作用域本身。luax使用表来当做一个作用域。环境表使用不同的元函数。
```lua
print(_E); -- dump _E to standard output to see what it contains.
print(_E._E == _E); -- true
```

### 内置函数

 + typeof(obj)  - 返回`obj`的类型（string）
 + meta_table(tab)  - 返回`tab`的元表
 + set_meta_table(tab, new_meta_table)
 + table_get(tab, key)  - 直接获取，不使用`tab`的元表的`_get`元函数
 + table_set(tab, key, value)  - 直接赋值
 + table_exist(tab, key)  - 换回一个bool类型表示表中是否有key
    ```lua
    local tab = { 0 : 123 };
    tab[1] = nil;
    print(table_exist(tab, 0)); -- true
    print(table_exist(tab, 1)); -- true
    print(table_exist(tab, 2)); -- false
    ```
 + table_next(tab, key)  - 遍历表中所有的键值对
    ```lua
    local tab = {};
    tab.name = "I'm a table";
    tab[2] = "2";
    tab.key = "this is a key";

    local i = nil;
    while i = table_next(tab, i) then
        if i == tab then continue; end  -- skip it's meta table
        print('key: ' + i + ' value: ' + tab[i]);
    end
    ```
 + table_prev(tab, key)  - 以相反的方向遍历
 + new_table(meta_table)  - 创建一个使用`meta_table`作为元表的表
 + pcall(func, args...)  - 在保护模式调用`func`，例如:
    ```lua
    local pcall_func = function(t, a, b)
        if t then throw('error'); end
        return a + b;
    end;
    local e, result = pcall(pcall_func, false, 1, 2); -- nil 3.0
    e, result = pcall(pcall_func, true, 1, 2); -- error nil
    ```
 + throw(exception)  - 抛出异常（被最近的pcall捕获）
 + collectgarbage([opt [, arg]])  - see: <http://www.lua.org/manual/5.3/manual.html#pdf-collectgarbage>
 + require(str)  - 加载一个标准库或一个luax源码文件
    ```lua
    local math = require('math');  -- standard library is not finished. Only 'math' now.
    print(math.abs(-123)); -- 123.0
    print(math.int(1.5)); -- 1.0

    local user = require("data/users.luax");
    print(user); -- see test/require_test_users.luax
    ```
 + dostring(str [, env])  - 在保护模式中运行`str`中的luax代码
    ```lua
    local code = "local a = 1; print(a); return 0; ";
    local e, res = dostring(code);
    if e then
        print('the luax code throws a exception ' + e);
    end
    print(res);  -- 0
    
    print(dostring('throw("error");')); -- error

    -- run code in a provided environment
    local env = {};
    local e = dostring(" print(1); ", env);
    print(e); -- luax exception: nil is not callable
    -- reason: env.print is nil

    -- the right way
    if true then
        local exports = {};
        local e = dostring(" print(1); exports.name = 'hello'; local username = 'a name'; ", _E);
        print(e); -- nil, success
        print(_E.exports.name, _E.username); -- 'hello', 'a name'
    end
    ```
 + dofile(filepath [, env])  - 在保护模式中运行文件中的luax代码


临时的debug内置函数:
 + print(obj)  - 打印一个对象到标准输出（打印表的每一个键值对）
 + dump_stack()  - 输出运行栈的内容到标准输出
 + emit_VS_breakpoint()  - 触发Visual Studio的断点，使得可以方便地从此处开始调试
 + show_gc_info()  - 输出垃圾回收相关信息到标准输出




### luax standard libraries

 see [Luax Standard Libraries](./luax_standard_library.md)





## Luax API
[TODO]
