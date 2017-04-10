# Opcode

todo: add pic to show the change of stack for every command.

 name | argument | description | achieve | example
------|----------|-------------|---------|---------
tag | none | used as a tag/label | | 
pop_to_tag | none | pop elements from stack, until pop a tag. | |
pushc_number
    | | | |
pushc_str
    | | | |
pushc_nil
    | | | |
pushc_false
    | | | |
pushc_true
    | | | |
    
local
    | none
    | declare variable(s) in current environment, and init them to default values(`nil`).
    | variables' names are placed in stack, end by a tag. For example, `local a, b;` generates opcode: `tag; pushc_str b; pushc_str a; local;`.
    | `local foo` `local a, b`
local_init
    | none
    | declare variable(s) in current environment, and init them to specify values.
    | For example, `local a, b = 1, 2, 3;` generates opcode: `tag; pushc_number 3; pushc_number 2; pushc_number 1; tag; pushc_str b; pushc_str a; local;`.
    | `local a, b = 1, 2, 3`

table_key
    | none
    | used to access key-value of one specify table
    | the key has been pushed to the stack, there may be more than one object, so the tag is needed to label the real one
    | `tab["key1"]` `tab[1 + 2]` `tab[func()]`
table_key_imm
    | identifier
    | used to access key-value of one specify table by immediate string
    | 
    | `tab.name` `tab.socket.port` `tab._test` `func().key`
call
    | none
    | call a luax function
    | 
    | `func()` `func(a, b)` `tab[1]()` `func()()`
