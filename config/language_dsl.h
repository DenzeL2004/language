#ifndef _LANGUAGE_DSL_
#define _LANGUAGE_DSL_

#define CREATE_VAL_NODE(val) Create_value_node ((val), nullptr, nullptr)

#define CHANGE_DATA(node, union_param, value)                                       \
    ((AST_data*) (node)->data)->data.union_param  = (value)

#define GET_DATA(node, union_param)                                                 \
        ((AST_data*) (node)->data)->data.union_param


#define DEF_TYPE(node, type)                                                        \
    ((AST_data*) (node)->data)->node_type = (type)

#define GET_TYPE(node)                                                              \
    ((AST_data*) (node)->data)->node_type 


#define IS_VAL(node) (((AST_data*) (node)->data)->node_type == NUM)
#define IS_OP(node)  (((AST_data*) (node)->data)->node_type == OP)
#define IS_VAR(node) (((AST_data*) (node)->data)->node_type == VAR)

#define IS_NOT_VAL(node) (IS_OP(node) || IS_VAR(node))


#define LEFT    node->left
#define RIGHT   node->right

#define CL  Tree_copy (LEFT)
#define CR  Tree_copy (RIGHT)


#define GET_VAL(node) (((AST_data*) (node)->data)->data.val)
#define GET_OP(node)  (((AST_data*) (node)->data)->data.operation)
#define GET_VAR(node) (((AST_data*) (node)->data)->data.obj)


#endif //_LANGUAGE_DSL_