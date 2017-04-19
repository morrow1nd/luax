![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)

# What is Luax?

 Luax is a easy-to-learn, concise and powerful programming language. ([Chinese Page](./doc/doc-zh/README.md))

 Luax provides full documentation(from the language itself to internal design), making it a perfect project for beginner to learn how to make a *interpreter*.

 Luax is distributed in source code, which contains the bytecode generator, luax virtual machine, standard library, a standalone executable interpreter, a syntax-standardlization tool, and full documentation.


# Documentation

## For Language User

 + [Get Started](./doc/get-started.md) - Setup a working environment.
 + [5-minites-tour](./doc/5-minites-tour.md) (Coming soon!)
 + [Luax Reference Manual](./doc/luax_reference_manual.md)

## For Language Hacker

 + [Luax Design Document](./doc/luax_design_document.md)


# Features

The luax programming language itself:
 + easy-to-learn: [5-minites-tour](./doc/5-minites-tour.md)
 + powerful data description constructs

The C achieve of luax:
 + lightwight arch: [source code structure](./doc/source_code_structure.md)
 + using subset of c++ and c
 + full-commented


# Build

```shell
mkdir build && cd build
cmake ..
make && make test
```
