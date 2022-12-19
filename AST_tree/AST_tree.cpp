#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>


#include "AST_tree.h"
#include "../config/language_dsl.h"

#include "../src/log_info/log_errors.h"
#include "../src/Generals_func/generals.h"

//======================================================================================

AST_data* Init_AST_node ()
{
    AST_data *data = (AST_data*) calloc (1, sizeof (AST_data)); 
    if (Check_nullptr (data))
    {
        PROCESS_ERROR (CREATE_EMPTY_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    data->node_type = UNKNOWN_T;

    data->data.operation = UNKNOWN_OP; 
    data->data.val       = NAN;
    data->data.obj       = nullptr;

    return data;
}

//======================================================================================

Node* Create_empty_node ()
{
    Node *node = Create_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    AST_data* node_data = Init_AST_node ();

    if (Check_nullptr (node_data))
    {
        PROCESS_ERROR (GET_OPERATION_NODE_ERR, "Memory allocation error, node nullptr\n");
        return nullptr;
    }

    node->data = node_data;

    return node;
}

//======================================================================================

Node* Create_value_node (const double value, Node* left, Node* right)
{
    Node *node = Create_empty_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    AST_data* node_data = (AST_data*) node->data;

    node_data->node_type = CONST;
    node_data->data.val = value;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;
}

//======================================================================================

Node* Create_operation_node (const int operation, Node* left, Node* right)
{
    Node *node = Create_empty_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    AST_data* node_data = (AST_data*) node->data;

    node_data->node_type = OP;
    node_data->data.operation = operation;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;

}

//======================================================================================

Node* Create_object_node (const char *name_obj, Node* left, Node* right)
{
    assert (name_obj != nullptr && "name_obj is nullptr");

    Node *node = Create_empty_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Node creation error\n");
        return nullptr;
    }

    AST_data* node_data = (AST_data*) node->data;

    node_data->node_type = VAR;
    node_data->data.obj = name_obj;

    node->data = node_data;

    node->left  = left;
    node->right = right;

    return node;
}

//======================================================================================

int Free_AST_nodes_data (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    if (!Check_nullptr (node->data))
    {
        ((AST_data*) node->data)->node_type = UNKNOWN_T;

        ((AST_data*) node->data)->data.operation = UNKNOWN_OP;
        ((AST_data*) node->data)->data.val       = NAN;
        ((AST_data*) node->data)->data.obj       = nullptr;

        free ((AST_data*) node->data);
        node->data = nullptr;
    }


    if (!Check_nullptr (node->left))
        Free_AST_nodes_data (node->left);

    if (!Check_nullptr (node->right))
        Free_AST_nodes_data (node->right);

    Delete_node (node);
   
    return 0;
}

//======================================================================================
