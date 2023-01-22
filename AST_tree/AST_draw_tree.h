#ifndef _AST_DRAW_TREE_H_
#define _AST_DRAW_TREE_H_

#include "AST_tree.h"

static const int Max_command_buffer = 100;

static const int Mask_draw_node_modes = (1 << 4) - 1;

enum Draw_tree_func_err
{
    TREE_DRAW_GRAPH_ERR     = -1,

    DRAW_DATABASE_ERR       = -4,
};

enum Tree_node_draw_modes
{
    
    DRAW_ID         = 0,

    DRAW_PTR        = 1,
    DRAW_DATA       = 2,

    DRAW_SONS_PTR   = 3,
};


int Draw_tree_graph (const Tree *tree, const char* name_output_file, 
                     const int node_draw_mode = Mask_draw_node_modes);

int Draw_database (const Tree *tree, const int node_mode = Mask_draw_node_modes);

#endif  //#endif _AST_DRAW_TREE_H_