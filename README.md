![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)

# What is Luax?

 Luax is a easy-to-learn, concise and powerful programming language. ([Chinese Page](./doc/doc-zh/README.md))

 Luax provides full documentation(from the language itself to internal design), making it a perfect project for beginner to learn how to make a *interpreter*.

 Luax is distributed in source code, which contains the bytecode generator, luax virtual machine, standard library, a standalone executable interpreter, a syntax-standardlization tool, and full documentation.


# Features

The luax programming language itself:
 + easy-to-learn
 + powerful data description constructs

The C achieve of luax:
 + lightwight arch(6000 line code)
 + using subset of c++ and c
 + full-documented: covered from design to implementation, a good guide to learn modern compiler&amp;interpreter


# Documentation

## For Language User

 + [Get Started](./doc/get-started.md) - Setup a working environment.
 + [5-minites-tour](./doc/5-minites-tour.md) (Coming soon!)
 + [Luax Reference Manual](./doc/luax_reference_manual.md) (Coming soon!)

## For Language Hacker

 + [Luax Design Document](./doc/luax_design_document.md)


# Build

```shell
mkdir build && cd build
cmake ..
make && make test
```
