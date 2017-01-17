#include "./parser_callback.h"
#include "./base.h"
#include "./mem.h"

LX_CALLBACK_DECLARE1(compile_unit, stmt_sequence)
{
    debuglog("compile_unit  ->  stmt_sequence");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(stmt_sequence, stmt, stmt_sequence)
{
    debuglog("stmt_sequence  ->  stmt stmt_sequence");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt_sequence, stmt)
{
    debuglog("stmt_sequence  ->  stmt");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt_sequence, EMPTY_SYMBOL) // use EMPTY_SYMBOL to replace symbol:ε
{
    debuglog("stmt_sequence  ->  EMPTY_SYMBOL");

    // FREE_SYNTAX_NODE(_1); // empty_symbol is NULL
}

LX_CALLBACK_DECLARE1(stmt, EOS) // EOS: end of statement
{
    debuglog("stmt  ->  EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== empty statement");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, if_stmt)
{
    debuglog("stmt  ->  if_stmt");
    debuglog("================== new statement ================== if");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, while_stmt)
{
    debuglog("stmt  ->  while_stmt");
    debuglog("================== new statement ================== while");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, for_stmt)
{
    debuglog("stmt  ->  for_stmt");
    debuglog("================== new statement ================== for");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(stmt, expr_stmt)
{
    debuglog("stmt  ->  expr_stmt");
    debuglog("================== new statement ================== expr_stmt");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, BREAK, EOS)
{
    debuglog("stmt  ->  BREAK EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== break;");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, CONTINUE, EOS)
{
    debuglog("stmt  ->  CONTINUE EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== continue;");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(stmt, RETURN, EOS)
{
    debuglog("stmt  ->  RETURN EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== return;");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(stmt, RETURN, expr_list, EOS)
{
    debuglog("stmt  ->  RETURN expr_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== return expr_list;");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(stmt, LOCAL, identifier_list, EOS)
{
    debuglog("stmt  ->  LOCAL identifier_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== local identifier_list;");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE5(stmt, LOCAL, identifier_list, EQL, expr_list, EOS)
{
    debuglog("stmt  ->  LOCAL identifier_list EQL expr_list EOS");
    debuglog_l(_1->token->linenum, "================== new statement ================== local identifier_list = expr_list;");

    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE5(if_stmt, IF, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "if_stmt  ->  IF expr THEN stmt_sequence END");

    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE7(if_stmt, IF, expr, THEN, stmt_sequence, ELSE, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "if_stmt  ->  IF expr THEN stmt_sequence ELSE stmt_sequence END");

    FREE_SYNTAX_NODE(_7);
    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE5(while_stmt, WHILE, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "while_stmt  ->  WHILE expr THEN stmt_sequence END");

    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE9(for_stmt, FOR, expr, EOS, expr, EOS, expr, THEN, stmt_sequence, END)
{
    debuglog_l(_1->token->linenum, "for_stmt  ->  FOR expr EOS expr EOS expr THEN stmt_sequence END");

    FREE_SYNTAX_NODE(_9);
    FREE_SYNTAX_NODE(_8);
    FREE_SYNTAX_NODE(_7);
    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(expr_stmt, expr, EOS)
{
    debuglog_l(_2->token->linenum, "expr_stmt  ->  expr EOS");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE3(expr_list, expr, COMMA, expr_list)
{
    debuglog("expr_list  ->  expr COMMA expr_list");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(expr_list, expr)
{
    debuglog("expr_list  ->  expr");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(prefix_expr_list, prefix_expr, COMMA, prefix_expr_list)
{
    debuglog("prefix_expr_list  ->  prefix_expr COMMA prefix_expr_list");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(prefix_expr_list, prefix_expr)
{
    debuglog("prefix_expr_list  ->  prefix_expr");

    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE1(expr, assign_expr)
{
    debuglog("expr  ->  assign_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(assign_expr, prefix_expr_list, assign_op, expr_list)
{
    debuglog("assign_expr  ->  prefix_expr_list assign_op expr_list");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_expr, logical_expr)
{
    debuglog("assign_expr  ->  logical_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(logical_expr, NOT, compare_expr)
{
    debuglog("logical_expr  ->  NOT compare_expr");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(logical_expr, compare_expr, logical_op, logical_expr)
{
    debuglog("logical_expr  ->  compare_expr logical_op logical_expr");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(logical_expr, compare_expr)
{
    debuglog("logical_expr  ->  compare_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(compare_expr, addtive_expr, compare_op, compare_expr)
{
    debuglog("compare_expr  ->  addtive_expr compare_op compare_expr");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_expr, addtive_expr)
{
    debuglog("compare_expr  ->  addtive_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(addtive_expr, multiply_expr, addtive_op, addtive_expr)
{
    debuglog("addtive_expr  ->  multiply_expr addtive_op addtive_expr");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(addtive_expr, multiply_expr)
{
    debuglog("addtive_expr  ->  multiply_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(multiply_expr, prefix_expr, multiply_op, multiply_expr)
{
    debuglog("multiply_expr  ->  prefix_expr multiply_op multiply_expr");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(multiply_expr, prefix_expr)
{
    debuglog("multiply_expr  ->  prefix_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(prefix_expr, prefix_op, suffix_expr)
{
    debuglog("prefix_expr  ->  prefix_op suffix_expr");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(prefix_expr, suffix_expr)
{
    debuglog("prefix_expr  ->  suffix_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(suffix_expr, single_expr, suffix_op)
{
    debuglog("suffix_expr  ->  single_expr suffix_op");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(suffix_expr, single_expr)
{
    debuglog("suffix_expr  ->  single_expr");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(single_expr, SL, expr, SR)
{
    debuglog("single_expr  ->  SL expr SR");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, NIL)
{
    debuglog("single_expr  ->  NIL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, FALSE)
{
    debuglog("single_expr  ->  FALSE");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, TRUE)
{
    debuglog("single_expr  ->  TRUE");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, immediate)
{
    debuglog("single_expr  ->  immediate");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(single_expr, IDENTIFIER)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("single_expr  ->  IDENTIFIER");

    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE3(suffix_op, SL, SR, suffix_op)
{
    debuglog("suffix_op  ->  SL SR suffix_op");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(suffix_op, SL, SR)
{
    debuglog("suffix_op  ->  SL SR");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE4(suffix_op, SL, expr_list, SR, suffix_op)
{
    debuglog("suffix_op  ->  SL expr_list SR suffix_op");

    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, SL, expr_list, SR)
{
    debuglog("suffix_op  ->  SL expr_list SR");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE4(suffix_op, ML, expr, MR, suffix_op)
{
    debuglog("suffix_op  ->  ML expr MR suffix_op");

    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, ML, expr, MR)
{
    debuglog("suffix_op  ->  ML expr MR");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(suffix_op, DOT, IDENTIFIER, suffix_op)
{
    debuglog("suffix_op  ->  DOT IDENTIFIER suffix_op");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE2(suffix_op, DOT, IDENTIFIER)
{
    debuglog("suffix_op  ->  DOT IDENTIFIER");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE1(immediate, object_immediate)
{
    debuglog("immediate  ->  object_immediate");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, function_define)
{
    debuglog("immediate  ->  function_define");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, STRING_IMMEDIATE)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("immediate  ->  STRING_IMMEDIATE");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(immediate, NUMBER_IMMEDIATE)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("immediate  ->  NUMBER_IMMEDIATE");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE2(object_immediate, BL, BR)
{
    debuglog("object_immediate  ->  BL BR");

    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate, BL, object_immediate_item_list, BR)
{
    debuglog("object_immediate  ->  BL object_immediate_item_list BR");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(object_immediate_item_list, object_immediate_item, COMMA, object_immediate_item_list)
{
    debuglog("object_immediate_item_list  ->  object_immediate_item COMMA object_immediate_item_list");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(object_immediate_item_list, object_immediate_item)
{
    debuglog("object_immediate_item_list  ->  object_immediate_item");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE3(object_immediate_item, IDENTIFIER, COLON, object_immediate_item_value)
{
    debuglog("object_immediate_item  ->  IDENTIFIER COLON object_immediate_item_value");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate_item, STRING_IMMEDIATE, COLON, object_immediate_item_value)
{
    debuglog("object_immediate_item  ->  STRING_IMMEDIATE COLON object_immediate_item_value");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(object_immediate_item, NUMBER_IMMEDIATE, COLON, object_immediate_item_value)
{
    debuglog("object_immediate_item  ->  NUMBER_IMMEDIATE COLON object_immediate_item_value");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(object_immediate_item_value, immediate)
{
    debuglog("object_immediate_item_value  ->  immediate");

    FREE_SYNTAX_NODE(_1);
}


LX_CALLBACK_DECLARE6(function_define, FUNCTION, SL, identifier_list, SR, stmt_sequence, END)
{
    debuglog("function_define  ->  FUNCTION SL identifier_list SR stmt_sequence END");

    FREE_SYNTAX_NODE(_6);
    FREE_SYNTAX_NODE(_5);
    FREE_SYNTAX_NODE(_4);
    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE3(identifier_list, IDENTIFIER, COMMA, identifier_list)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("identifier_list  ->  IDENTIFIER COMMA identifier_list");

    FREE_SYNTAX_NODE(_3);
    FREE_SYNTAX_NODE(_2);
    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(identifier_list, IDENTIFIER)
{
    debuglog_luax_str(_1->token->text_len, _1->token->text);
    debuglog("identifier_list  ->  IDENTIFIER");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(assign_op, EQL)
{
    debuglog("assign_op  ->  EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, ADD_EQL)
{
    debuglog("assign_op  ->  ADD_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, SUB_EQL)
{
    debuglog("assign_op  ->  SUB_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, MUL_EQL)
{
    debuglog("assign_op  ->  MUL_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(assign_op, DIV_EQL)
{
    debuglog("assign_op  ->  DIV_EQL");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(logical_op, AND)
{
    debuglog("logical_op  ->  AND");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(logical_op, OR)
{
    debuglog("logical_op  ->  OR");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(compare_op, LESS)
{
    debuglog("compare_op  ->  LESS");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, GREATER)
{
    debuglog("compare_op  ->  GREATER");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, LESS_EQL)
{
    debuglog("compare_op  ->  LESS_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, GREATER_EQL)
{
    debuglog("compare_op  ->  GREATER_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, EQL_EQL)
{
    debuglog("compare_op  ->  EQL_EQL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(compare_op, NOT_EQL)
{
    debuglog("compare_op  ->  NOT_EQL");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(addtive_op, ADD)
{
    debuglog("addtive_op  ->  ADD");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(addtive_op, SUB)
{
    debuglog("addtive_op  ->  SUB");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(multiply_op, MUL)
{
    debuglog("multiply_op  ->  MUL");

    FREE_SYNTAX_NODE(_1);
}
LX_CALLBACK_DECLARE1(multiply_op, DIV)
{
    debuglog("multiply_op  ->  DIV");

    FREE_SYNTAX_NODE(_1);
}

LX_CALLBACK_DECLARE1(prefix_op, SUB)
{
    debuglog("prefix_op  ->  SUB");
    
    FREE_SYNTAX_NODE(_1);
}