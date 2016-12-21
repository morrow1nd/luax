#ifndef __LX_PARSER_CALLBACK__H_
#define __LX_PARSER_CALLBACK__H_

#include "./base.h"
#include "./parser.h"


#ifdef TRUE
# undef TRUE
#endif
#ifdef FALSE
# undef FALSE
#endif


enum lx_syntax_node_tag
{

};

struct lx_syntax_node
{
    const Token * token;
    SyntaxNode * next;
    enum lx_syntax_node_tag tag;
    //VMCodeTem * vmcode;
};

typedef void ret;
typedef lx_syntax_node* par;


#define LX_CALLBACK_FUNCTION_NAME1(self_name, _1_name)           lx_callback_function__##__self_name##__to__##_1_name
#define LX_CALLBACK_FUNCTION_NAME2(self_name, _1_name, _2_name)           lx_callback_function__##__self_name##__to__##_1_name##__##_2_name
#define LX_CALLBACK_FUNCTION_NAME3(self_name, _1_name, _2_name, _3_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name
#define LX_CALLBACK_FUNCTION_NAME4(self_name, _1_name, _2_name, _3_name, _4_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name##__##_4_name
#define LX_CALLBACK_FUNCTION_NAME5(self_name, _1_name, _2_name, _3_name, _4_name, _5_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name##__##_4_name##__##_5_name
#define LX_CALLBACK_FUNCTION_NAME6(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name##__##_4_name##__##_5_name##__##_6_name
#define LX_CALLBACK_FUNCTION_NAME7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name##__##_4_name##__##_5_name##__##_6_name##__##_7_name
#define LX_CALLBACK_FUNCTION_NAME9(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, _8_name, _9_name)           ret lx_callback_function__##__self_name##__to__##_1_name##__##_2_name##__##_3_name##__##_4_name##__##_5_name##__##_6_name##__##_7_name##__##_8_name##__##_9_name


#define LX_CALLBACK_DECLARE1(self_name, _1_name)           ret LX_CALLBACK_FUNCTION_NAME1(self_name, _1_name)##(par _self, par _1)
#define LX_CALLBACK_DECLARE2(self_name, _1_name, _2_name)           ret LX_CALLBACK_FUNCTION_NAME2(self_name, _1_name, _2_name)##(par _self, par _1, par _2)
#define LX_CALLBACK_DECLARE3(self_name, _1_name, _2_name, _3_name)           ret LX_CALLBACK_FUNCTION_NAME3(self_name, _1_name, _2_name, _3_name)##(par _self, par _1, par _2, par _3)
#define LX_CALLBACK_DECLARE4(self_name, _1_name, _2_name, _3_name, _4_name)           ret LX_CALLBACK_FUNCTION_NAME4(self_name, _1_name, _2_name, _3_name, _4_name)##(par _self, par _1, par _2, par _3, par _4)
#define LX_CALLBACK_DECLARE5(self_name, _1_name, _2_name, _3_name, _4_name, _5_name)           ret LX_CALLBACK_FUNCTION_NAME5(self_name, _1_name, _2_name, _3_name, _4_name, _5_name)##(par _self, par _1, par _2, par _3, par _4, par _5)
#define LX_CALLBACK_DECLARE6(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name)           ret LX_CALLBACK_FUNCTION_NAME6(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name)##(par _self, par _1, par _2, par _3, par _4, par _5, par _6)
#define LX_CALLBACK_DECLARE7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name)           ret LX_CALLBACK_FUNCTION_NAME7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name)##(par _self, par _1, par _2, par _3, par _4, par _5, par _6, par _7)
#define LX_CALLBACK_DECLARE9(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, _8_name, _9_name)           ret LX_CALLBACK_FUNCTION_NAME7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, _8_name, _9_name)##(par _self, par _1, par _2, par _3, par _4, par _5, par _6, par _7, par _8, par _9)


#define LX_CALLBACK_CALL1(self_name, _1_name, ...)           LX_CALLBACK_FUNCTION_NAME1(self_name, _1_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL2(self_name, _1_name, _2_name, ...)           LX_CALLBACK_FUNCTION_NAME2(self_name, _1_name, _2_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL3(self_name, _1_name, _2_name, _3_name, ...)           LX_CALLBACK_FUNCTION_NAME3(self_name, _1_name, _2_name, _3_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL4(self_name, _1_name, _2_name, _3_name, _4_name, ...)           LX_CALLBACK_FUNCTION_NAME4(self_name, _1_name, _2_name, _3_name, _4_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL5(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, ...)           LX_CALLBACK_FUNCTION_NAME5(self_name, _1_name, _2_name, _3_name, _4_name, _5_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL6(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, ...)           LX_CALLBACK_FUNCTION_NAME6(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, ...)           LX_CALLBACK_FUNCTION_NAME7(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name)##(##__VA_ARGS__)
#define LX_CALLBACK_CALL9(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, _8_name, _9_name, ...)           LX_CALLBACK_FUNCTION_NAME9(self_name, _1_name, _2_name, _3_name, _4_name, _5_name, _6_name, _7_name, _8_name, _9_name)##(##__VA_ARGS__)




LX_CALLBACK_DECLARE1(compile_unit, stmt_sequence);

LX_CALLBACK_DECLARE2(stmt_sequence, stmt_sequence, stmt);
LX_CALLBACK_DECLARE1(stmt_sequence, stmt);
LX_CALLBACK_DECLARE1(stmt_sequence, empty_symbol); // use empty_symbol to replace symbol:ε

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


LX_CALLBACK_DECLARE3(expr_list, comma, expr_list);
LX_CALLBACK_DECLARE1(expr_list, expr);
LX_CALLBACK_DECLARE3(prefix_expr_list, prefix_expr, comma, prefix_expr_list);
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

LX_CALLBACK_DECLARE3(single_expr, sl, expr, sr);
LX_CALLBACK_DECLARE1(single_expr, NIL);
LX_CALLBACK_DECLARE1(single_expr, FALSE);
LX_CALLBACK_DECLARE1(single_expr, TRUE);
LX_CALLBACK_DECLARE1(single_expr, immediate);
LX_CALLBACK_DECLARE1(single_expr, IDENTIFIER);


LX_CALLBACK_DECLARE3(suffix_op, sl, sr, suffix_op);
LX_CALLBACK_DECLARE2(suffix_op, sl, sr);
LX_CALLBACK_DECLARE4(suffix_op, sl, expr_list, sr, suffix_op);
LX_CALLBACK_DECLARE3(suffix_op, sl, expr_list, sr);
LX_CALLBACK_DECLARE4(suffix_op, ml, expr, mr, suffix_op);
LX_CALLBACK_DECLARE3(suffix_op, ml, expr, mr);
LX_CALLBACK_DECLARE3(suffix_op, dot, IDENTIFIER, suffix_op);
LX_CALLBACK_DECLARE2(suffix_op, dot, IDENTIFIER);


LX_CALLBACK_DECLARE1(immediate, object_immediate);
LX_CALLBACK_DECLARE1(immediate, function_define);
LX_CALLBACK_DECLARE1(immediate, STRING_IMMEDIATE);
LX_CALLBACK_DECLARE1(immediate, NUMBER_IMMEDIATE);

LX_CALLBACK_DECLARE2(object_immediate, bl, br);
LX_CALLBACK_DECLARE3(object_immediate, bl, object_immediate_item_list, br);

LX_CALLBACK_DECLARE3(object_immediate_item_list, object_immediate_item, comma, object_immediate_item_list);
LX_CALLBACK_DECLARE1(object_immediate_item_list, object_immediate_item);

LX_CALLBACK_DECLARE3(object_immediate_item, IDENTIFIER, colon, object_immediate_item_value);
LX_CALLBACK_DECLARE3(object_immediate_item, STRING_IMMEDIATE, colon, object_immediate_item_value);
LX_CALLBACK_DECLARE3(object_immediate_item, NUMBER_IMMEDIATE, color, object_immediate_item_value);

LX_CALLBACK_DECLARE1(object_immediate_item_value, immediate);


LX_CALLBACK_DECLARE6(function_define, FUNCTION, sl, identifier_list, sr, stmt_sequence, END);
LX_CALLBACK_DECLARE3(identifier_list, IDENTIFIER, comma, identifier_list);
LX_CALLBACK_DECLARE1(identifier_list, IDENTIFIER);

LX_CALLBACK_DECLARE1(assign_op, EQUAL);
LX_CALLBACK_DECLARE1(assign_op, ADD_EQUAL);
LX_CALLBACK_DECLARE1(assign_op, SUB_EQUAL);
LX_CALLBACK_DECLARE1(assign_op, MUL_EQUAL);
LX_CALLBACK_DECLARE1(assign_op, DIV_EQUAL);

LX_CALLBACK_DECLARE1(logical_op, AND);
LX_CALLBACK_DECLARE1(logical_op, OR);

LX_CALLBACK_DECLARE1(compare_op, LESS);
LX_CALLBACK_DECLARE1(compare_op, BIGGER);
LX_CALLBACK_DECLARE1(compare_op, LESS_EQUAL);
LX_CALLBACK_DECLARE1(compare_op, BIGGER_EQUAL);
LX_CALLBACK_DECLARE1(compare_op, EQUAL_EQUAL);
LX_CALLBACK_DECLARE1(compare_op, NOT_EQUAL);

LX_CALLBACK_DECLARE1(addtive_op, ADD);
LX_CALLBACK_DECLARE1(addtive_op, SUB);

LX_CALLBACK_DECLARE1(multiply_op, MUL);
LX_CALLBACK_DECLARE1(multiply_op, DIV);

LX_CALLBACK_DECLARE1(prefix_op, SUB);


#endif __LX_PARSER_CALLBACK__H_