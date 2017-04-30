<div class="out-div" style="font-size: 16px;line-height: 40px;display: inline-block;height: 40px;padding-left: 50px;padding-right: 50px;text-align: center;border-radius: 5px;margin-left: 32px;vertical-align: top;background-color: #fff6bf;">
    <span><b>注意</b>：同步于<a href="../../README.md" style="text-decoration: none">英文版</a></span>
</div> <br>

![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)



# Luax简介

 Luax是一种易于学习、简洁而不失强大的编程语言。Luax不仅仅提供供语言使用者阅读的文档，也提供讲解内部实现原理的文档。这使得它成为一个优秀的解释器实现教程。Luax的源码包括：字节码生成器、luax虚拟机、luax语言标准库、可运行的执行程序、以及文档。



# 文档

## 面向语言使用者

 + [开始](../get-started.md) - 建立使用环境
 + [5分钟上手教程](../5-minites-tour.md)
 + [Luax手册](./luax_reference_manual.md)

## 内部实现说明文档

 + [Luax设计文档](../luax_design_document.md)  (未完成!)



# 特点

语言：
 + 容易学习 [5分钟上手教程](../5-minites-tour.md)
 + 强大的数据表述能力

Luax的C实现：
 + 轻量级架构 [源代码结构](../source_code_structure.md)
 + 使用c++和c的子集
 + 完善的注释

特点列表：
 + **动态类型语言** (*nil*, *bool*, *number*, *string*, *function*, *table*)
 + **基本语句** (*变量声明语句*, *if/while*, *break*, *continue*, *函数定义*)
 + **赋值多个值，返回多个值** - `a, b = 1, 2;` `function(a, b) return a+b, a-b; end`
 + **table** - 一个盛装多个键值对的容器，键或值的类型可以是任意上面提到的类型。
 + **meta table**  - a table defining what would happens when specific action made to a table(such as: get set call), example: [*read-only table*](https://github.com/morrow1nd/luax/blob/master/doc/luax_reference_manual.md#meta-table) 
  + **function**  - first-class citizen, example: [*link several functions*](https://github.com/morrow1nd/luax/blob/master/doc/luax_reference_manual.md#function) 



# 例子

```lua
-- table: 一个盛装多个键值对的容器
local tab = { 'key' : 'value', 1 : false };
tab.name = "I'm a table";
tab["func"] = function() return "I can hold a function"; end; -- tab["func"] equals to tab.func
tab[true] = "the type of my key can be a boolean";
tab[another_table] = "or a table";


-- function
local func = function(a, b) -- 创建一个函数
    return a + b, a - b;
end;
tab.a, tab.b = func(1, 2); -- return 3, -1

print(func(1, 2), 2);   -- 3, -1, 2
print((func(1, 2)), 2); -- 3, 2

local func_sum = function(a, b)
    local i, sum = 0, 0;
    while i < arguments.size then  -- 每一个函数里面都有一个名为arguments的变量
        sum += arguments[i];
        i += 1;
    end
    return sum;
end
print(func_sum(1, 2, 3)); -- 6
print(func_sum(1)); -- 1, 这时参数b等于nil
--[[ 
  更多的例子见： doc/luax_reference_manual.md#function
]]
```


# Build

```shell
mkdir build && cd build
cmake ..
make && make test
# 从 [Get Started] 文档中可以找到更多的平台
```
