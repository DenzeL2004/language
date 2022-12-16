#ifndef _AST_TREE_H_
#define _AST_TREE_H_


#include <stdio.h>

#include "../language_config.h"

#include "../../src/tree/tree.h"


struct AST_data
{
    Node_type node_type;
    
    union Node_data
    {
        double           val;
        const char*      obj;
        int              operation;
    } data;
};


enum AST_node_func_err
{
    CREATE_EMPTY_NODE_ERR   = -1,

    GET_VALUE_NODE_ERR      = -2,
    GET_OPERATION_NODE_ERR  = -3,
    GET_OBJECT_NODE_ERR     = -4,
};


/**
 * @brief  Creates an empty tree node 
 * @return Node* pointer to the created node
 */
AST_data* Init_AST_node ();


Node* Create_empty_node ();

Node* Create_value_node     (const double value,   Node* left, Node* right);

Node* Create_operation_node (const int operation,  Node* left, Node* right);

Node* Create_object_node    (const char* name_obj, Node* left, Node* right);



/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
int Free_AST_nodes_data (Node *node);


#endif //#endif _AST_TREE_H_