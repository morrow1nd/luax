<div class="out-div" style="font-size: 16px;line-height: 40px;display: inline-block;height: 40px;padding-left: 50px;padding-right: 50px;text-align: center;border-radius: 5px;margin-left: 32px;vertical-align: top;background-color: #fff6bf;">
    <span><b>注意</b>：同步于<a href="../../README.md" style="text-decoration: none">英文版</a></span>
</div>

![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)


# Luax简介

 Luax是一种易于学习、简洁而不失强大的编程语言。Luax不仅仅提供供语言使用者阅读的文档，也提供讲解内部实现原理的文档。这使得它成为一个优秀的解释器实现教程。Luax的源码包括：字节码生成器、luax虚拟机、luax语言标准库、可运行的执行程序、语法标准化工具、以及文档。

# 特点

语言：
 + 容易学习 [5分钟上手教程](./5-minites-tour.md)
 + 强大的数据表述能力

Luax的C实现：
 + 轻量级架构 [源代码结构](./source_code_structure.md)
 + 使用c++和c的子集
 + 完善的注释

# 文档

## 面向语言使用者

 + [开始](./get-started.md) - 建立使用环境
 + [5分钟上手教程](./5-minites-tour.md) (待完成)
 + [Luax手册](./luax_reference_manual.md)

## 内部实现说明文档

 + [Luax设计文档](./luax_design_document.md)

## Build

```shell
mkdir build && cd build
cmake ..
make && make test
```
