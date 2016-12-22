#include "./parser_callback.h"

LX_CALLBACK_DECLARE1(compile_unit, stmt_sequence)
{
    
}

LX_CALLBACK_DECLARE2(stmt_sequence, stmt_sequence, stmt);
LX_CALLBACK_DECLARE1(stmt_sequence, stmt);
LX_CALLBACK_DECLARE1(stmt_sequence, EMPTY_SYMBOL); // use EMPTY_SYMBOL to replace symbol:ε

LX_CALLBACK_DECLARE1(stmt, EOS); // EOS: end of statement
LX_CALLBACK_DECLARE1(stmt, if_stmt);
LX_CALLBACK_DECLARE1(stmt, while_stmt);
LX_CALLBACK_DECLARE1(stmt, for_stmt);
LX_CALLBACK_DECLARE1(stmt, expr_stmt);
LX_CALLBACK_DECLARE2(stmt, BREAK, EOS);
LX_CALLBACK_DECLARE2(stmt, CONTINUE, EOS);
LX_CALLBACK_DECLARE2(stmt, RETURN, EOS);
LX_CALLBACK_DECLARE3(stmt, RETURN, expr_list, EOS);

LX_CALLBACK_DECLARE5(if_stmt, IF, expr, THEN, stmt_sequence, END);
LX_CALLBACK_DECLARE7(if_stmt, IF, expr, THEN, stmt_sequence, ELSE, stmt_sequence, END);

LX_CALLBACK_DECLARE5(while_stmt, WHILE, expr, THEN, stmt_sequence, END);

LX_CALLBACK_DECLARE9(for_stmt, FOR, expr, EOS, expr, EOS, expr, THEN, stmt_sequence, END);

LX_CALLBACK_DECLARE2(expr_stmt, expr, EOS);


LX_CALLBACK_DECLARE3(expr_list, expr, COMMA, expr_list);
LX_CALLBACK_DECLARE1(expr_list, expr);
LX_CALLBACK_DECLARE3(prefix_expr_list, prefix_expr, COMMA, prefix_expr_list);
LX_CALLBACK_DECLARE1(prefix_expr_list, prefix_expr);


LX_CALLBACK_DECLARE1(expr, assign_expr);

LX_CALLBACK_DECLARE3(assign_expr, prefix_expr_list, assign_op, expr_list);
LX_CALLBACK_DECLARE1(assign_expr, logical_expr);

LX_CALLBACK_DECLARE2(logical_expr, NOT, compare_expr);
LX_CALLBACK_DECLARE3(logical_expr, compare_expr, logical_op, logical_expr);
LX_CALLBACK_DECLARE1(logical_expr, compare_expr);

LX_CALLBACK_DECLARE3(compare_expr, addtive_expr, compare_op, compare_expr);
LX_CALLBACK_DECLARE1(compare_expr, addtive_expr);

LX_CALLBACK_DECLARE3(addtive_expr, multiply_expr, addtive_op, addtive_expr);
LX_CALLBACK_DECLARE1(addtive_expr, multiply_expr);

LX_CALLBACK_DECLARE3(multiply_expr, prefix_expr, multiply_op, multiply_expr);
LX_CALLBACK_DECLARE1(multiply_expr, prefix_expr);

LX_CALLBACK_DECLARE2(prefix_expr, prefix_op, suffix_expr);
LX_CALLBACK_DECLARE1(prefix_expr, suffix_expr);

LX_CALLBACK_DECLARE2(suffix_expr, single_expr, suffix_op);
LX_CALLBACK_DECLARE1(suffix_expr, single_expr);

LX_CALLBACK_DECLARE3(single_expr, SL, expr, SR);
LX_CALLBACK_DECLARE1(single_expr, NIL);
LX_CALLBACK_DECLARE1(single_expr, FALSE);
LX_CALLBACK_DECLARE1(single_expr, TRUE);
LX_CALLBACK_DECLARE1(single_expr, immediate);
LX_CALLBACK_DECLARE1(single_expr, IDENTIFIER);


LX_CALLBACK_DECLARE3(suffix_op, SL, SR, suffix_op);
LX_CALLBACK_DECLARE2(suffix_op, SL, SR);
LX_CALLBACK_DECLARE4(suffix_op, SL, expr_list, SR, suffix_op);
LX_CALLBACK_DECLARE3(suffix_op, SL, expr_list, SR);
LX_CALLBACK_DECLARE4(suffix_op, ML, expr, MR, suffix_op);
LX_CALLBACK_DECLARE3(suffix_op, ML, expr, MR);
LX_CALLBACK_DECLARE3(suffix_op, DOT, IDENTIFIER, suffix_op);
LX_CALLBACK_DECLARE2(suffix_op, DOT, IDENTIFIER);


LX_CALLBACK_DECLARE1(immediate, object_immediate);
LX_CALLBACK_DECLARE1(immediate, function_define);
LX_CALLBACK_DECLARE1(immediate, STRING_IMMEDIATE);
LX_CALLBACK_DECLARE1(immediate, NUMBER_IMMEDIATE);

LX_CALLBACK_DECLARE2(object_immediate, BL, BR);
LX_CALLBACK_DECLARE3(object_immediate, BL, object_immediate_item_list, BR);

LX_CALLBACK_DECLARE3(object_immediate_item_list, object_immediate_item, COMMA, object_immediate_item_list);
LX_CALLBACK_DECLARE1(object_immediate_item_list, object_immediate_item);

LX_CALLBACK_DECLARE3(object_immediate_item, IDENTIFIER, COLON, object_immediate_item_value);
LX_CALLBACK_DECLARE3(object_immediate_item, STRING_IMMEDIATE, COLON, object_immediate_item_value);
LX_CALLBACK_DECLARE3(object_immediate_item, NUMBER_IMMEDIATE, COLON, object_immediate_item_value);

LX_CALLBACK_DECLARE1(object_immediate_item_value, immediate);


LX_CALLBACK_DECLARE6(function_define, FUNCTION, SL, identifier_list, SR, stmt_sequence, END);
LX_CALLBACK_DECLARE3(identifier_list, IDENTIFIER, COMMA, identifier_list);
LX_CALLBACK_DECLARE1(identifier_list, IDENTIFIER);

LX_CALLBACK_DECLARE1(assign_op, EQL);
LX_CALLBACK_DECLARE1(assign_op, ADD_EQL);
LX_CALLBACK_DECLARE1(assign_op, SUB_EQL);
LX_CALLBACK_DECLARE1(assign_op, MUL_EQL);
LX_CALLBACK_DECLARE1(assign_op, DIV_EQL);

LX_CALLBACK_DECLARE1(logical_op, AND);
LX_CALLBACK_DECLARE1(logical_op, OR);

LX_CALLBACK_DECLARE1(compare_op, LESS);
LX_CALLBACK_DECLARE1(compare_op, GREATER);
LX_CALLBACK_DECLARE1(compare_op, LESS_EQL);
LX_CALLBACK_DECLARE1(compare_op, GREATER_EQL);
LX_CALLBACK_DECLARE1(compare_op, EQL_EQL);
LX_CALLBACK_DECLARE1(compare_op, NOT_EQL);

LX_CALLBACK_DECLARE1(addtive_op, ADD);
LX_CALLBACK_DECLARE1(addtive_op, SUB);

LX_CALLBACK_DECLARE1(multiply_op, MUL);
LX_CALLBACK_DECLARE1(multiply_op, DIV);

LX_CALLBACK_DECLARE1(prefix_op, SUB);
