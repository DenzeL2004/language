#ifndef _TREE_H_
#define _TREE_H_


#include "../log_info/log_def.h"


static const int Max_definition_buffer = 300;

static const int Max_object_buffer     = 100;

static const char *Default_output_file_name = "last_save_base.txt";

static const int Bad_ptr = 0x100;

struct Node
{
    void   *data  = nullptr;
    Node   *right = nullptr;
    Node   *left  = nullptr;
};

struct Tree
{
    Node *root = nullptr;
};


enum Tree_func_err
{
    TREE_CTOR_ERR           = -1,
    TREE_DTOR_ERR           = -2,

    CREATE_NODE_ERR         = -3,
    DELETE_SUBTREE_ERR      = -4,

    TREE_ADD_NODE_ERR       = -7,
};

enum Tree_err
{
    ROOT_IS_NULLPTR             = (1 << 1),   
};


int Tree_ctor (Tree *tree);

int Tree_dtor (Tree *tree);


/** 
 * @brief Removes all links and node values from the subtree
 * @param [in] node Pointer to Node pointer
*/
void Delete_subtree (Node *node);


/** 
 * @brief Creates an empty tree node
 * @return Returns a pointer to the node if the node was created, otherwise returns nullptr
*/
Node* Create_node ();

/** 
 * @brief Removes a node with all information
 * @param [in] node Pointer to Node pointer
 * @return Returns zero if the deletion was successful, otherwise returns a non-zero number
*/
int Delete_node (Node *node);


int Is_leaf_node (const Node *node);

int Add_node_sons (Node *node);

#define Tree_dump(tree, ...)                       \
        Tree_dump_ (tree, LOG_ARGS, __VA_ARGS__)

int Tree_dump_ (const Tree *tree, LOG_PARAMETS, const char *format, ...);

#endif  //#endif _TREE_H_