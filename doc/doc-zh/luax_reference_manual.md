<div class="out-div" style="font-size: 16px;line-height: 40px;display: inline-block;height: 40px;padding-left: 50px;padding-right: 50px;text-align: center;border-radius: 5px;margin-left: 32px;vertical-align: top;background-color: #fff6bf;">
    <span><b>注意</b>：落后于<a href="../luax_reference_manual.md" style="text-decoration: none">英文版</a></span>
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
 但是在luax中，上面的代码会抛出一个异常：*使用未声明的变量*。下面是在luax中的使用方式：
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
```

### 表达式

#### 操作符

类型        | 操作符   
----------------------|--------------------
算数操作符 | `+ - * /` 
关系操作符 | `< > <= >= == !=` 
逻辑操作符 | `and or not` 

#### 表读写操作符

```lua
local tab = {};
tab["key1"] = 1;
tab[3] = 'value2';
tab[true] = 'value3';
tab.key2 = {};
tab['key2']['key1'] = 123;
```

 `tab.key2`仅仅是`tab["key2"]`的语法糖。`.`操作符后的字符串必须是一个标准C标识符。例如：`tab.32k`是错误的。

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
    local g; -- override it
    g = 2;
end
print(g); -- 1
-- while语句和if语句一样
```

### luax标准库

内置函数:
 + typeof(obj)  - 返回`obj`的类型（string）
 + meta_table(tab)  - 返回`tab`的元表
 + set_meta_table(tab, new_meta_table)
 + table_get(tab, key)  - 直接获取，不使用`tab`的元表的`_get`元函数
 + table_set(tab, key, value)  - 直接赋值
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

临时的debug内置函数:
 + print(obj)  - 打印一个对象到标准输出（打印表的每一个键值对）
 + dump_stack()  - 输出运行栈的内容到标准输出
 + emit_VS_breakpoint()  - 触发Visual Studio的断点，使得可以方便地从此处开始调试
 + show_gc_info()  - 输出垃圾回收相关信息到标准输出




## Luax API
[TODO]
