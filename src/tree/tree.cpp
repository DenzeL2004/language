#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#include "tree.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"


static uint64_t Check_tree (const Tree *tree);


static void Print_tree_variables (const Tree *tree, FILE *fpout);

static void Print_error_value (uint64_t err, FILE *fpout);



#define REPORT(...)                                         \
    {                                                       \
        Tree_dump (tree, __VA_ARGS__);                      \
        Err_report (__VA_ARGS__);                                      \
                                                            \
    }while (0)
                                    
//======================================================================================

int Tree_ctor (Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    tree->root = Create_node ();

    if (Check_nullptr (tree->root))
        return PROCESS_ERROR (TREE_CTOR_ERR, "Memory allocation error, root is nullptr\n");

    return 0;
}

//======================================================================================

int Tree_dtor (Tree *tree)
{
    assert (tree != nullptr && "tree ptr is nullptr");

    if (Check_tree (tree))
    {
        REPORT ("ENTRY\nFROM: Tree_dtor\n");
        return TREE_DTOR_ERR;
    }

    Delete_subtree (tree->root);
    tree->root = nullptr;

    return 0;
}

//======================================================================================

void Delete_subtree (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    if (!Check_nullptr (node->left))    Delete_subtree (node->left);
    if (!Check_nullptr (node->right))   Delete_subtree (node->right);

    if (Delete_node (node))
    {
        PROCESS_ERROR (DELETE_SUBTREE_ERR, "Failed to delete node by pointer = |%p|\n", (char*) node);
        return;
    }

    node = nullptr;

    return;
}

//======================================================================================

Node* Create_node ()
{
    Node *new_node = (Node*) calloc (1, sizeof (Node));
    if (Check_nullptr (new_node))
    {
        PROCESS_ERROR (CREATE_NODE_ERR, "Memory allocation error, new_node is nullptr\n");
        return nullptr;
    }

    new_node->left  = nullptr;
    new_node->right = nullptr;
    new_node->data  = nullptr;

    return new_node;
}

//======================================================================================

int Delete_node (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    node->left  = nullptr;
    node->right = nullptr;

    node->data = nullptr;

    free (node);
    node = nullptr;

    return 0;
}

//======================================================================================

int Is_leaf_node (const Node *node)
{
    assert (node != nullptr && "node is nullptr");

    return (Check_nullptr (node->left) && Check_nullptr (node->right));
}

//======================================================================================

int Add_node_sons (Node *node)
{
    assert (node != nullptr && "node is nullptr");

    node->left  = Create_node ();
    if (Check_nullptr (node->left))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, left son is nullptr\n");
        

    node->right  = Create_node ();
    if (Check_nullptr (node->right))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, right son is nullptr\n");

    return 0;

}

//======================================================================================

int Tree_dump_ (const Tree *tree,
                const char* file_name, const char* func_name, int line, const char *format, ...)
{
    assert (tree != nullptr && "tree is nullptr");

    uint64_t err = Check_tree (tree);

    FILE *fp_logs = Get_log_file_ptr ();

    fprintf (fp_logs, "=================================================\n\n");

    va_list args = {};

    va_start(args, format);

        fprintf (fp_logs, "<h2>");
        vfprintf(fp_logs, format, args);
        fprintf (fp_logs, "</h2>");
    
    va_end(args);

    fprintf (fp_logs, "REFERENCE:\n");

    if (err) 
        fprintf (fp_logs, "ERROR\nCaused an error in file %s, function %s, line %d\n\n", LOG_VAR);
    else
        fprintf (fp_logs, "OK\nlast call in file %s, function %s, line %d\n\n", LOG_VAR);
    
                     
    if (err)
    {
        fprintf (fp_logs, "ERR CODE: ");
        Bin_represent (fp_logs, err, sizeof (err));
        fprintf (fp_logs, "\n");
    }

    Print_error_value (err, fp_logs);
    Print_tree_variables (tree, fp_logs);

    fprintf (fp_logs, "\n\n");


    fprintf (fp_logs, "==========================================================\n\n");

    return 0;
}

//======================================================================================

static void Print_tree_variables (const Tree *tree, FILE *fpout)
{
    assert (tree  != nullptr &&  "tree is nullptr");
    assert (fpout != nullptr && "fpout is nullptr");

    fprintf (fpout, "<body>\n");
    fprintf (fpout, "<table border=\"1\">\n");
    
    fprintf (fpout, "<tr><td> pointer to root </td> <td> %p </td></tr>", (char*) tree->root);


    fprintf (fpout, "</table>\n");
    fprintf (fpout, "</body>\n");
   
    return;
}

//======================================================================================

static void Print_error_value (uint64_t err, FILE *fpout)
{
    assert (fpout != nullptr && "fpout is nullptr\n");


    if (err & ROOT_IS_NULLPTR)
        fprintf (fpout, "root is nullptr\n");        

    fprintf (fpout, "\n\n");

    return;
}

//======================================================================================

static uint64_t Check_tree (const Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    uint64_t err = 0;

    if (Check_nullptr (tree->root)) err |= ROOT_IS_NULLPTR;

    return err;
}

//======================================================================================