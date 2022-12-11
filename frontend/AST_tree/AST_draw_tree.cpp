#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "AST_draw_tree.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"



static void Draw_nodes_recursive (FILE *fpout, const Node *node, int *counter, const int node_draw_mode);


static void Draw_node (FILE *fpout, const Node *node, const int id, const int node_draw_mode);

static void Draw_node_data (FILE *fpout, AST_data* node);

//======================================================================================

int Draw_tree_graph (const Tree *tree, const char* name_output_file, const int node_draw_mode)
{
    assert (tree != nullptr && "tree is nullptr");
    assert (name_output_file != nullptr && "name output file is nullptr");

    FILE *graph = Open_file_ptr ("temp/graph_img/graph.txt", "w");
    if (Check_nullptr (graph))
    {
        Err_report ("Open graph file\n");
        return TREE_DRAW_GRAPH_ERR;
    }

    fprintf (graph, "digraph G{\n");
    fprintf (graph, "rankdir=TB;\n");
    fprintf (graph, "splines=spline;\n");
    fprintf (graph, "{\n");

    int counter = 0;
    Draw_nodes_recursive (graph, tree->root, &counter, node_draw_mode);


    fprintf(graph, "}\n}\n");
    fclose(graph);



    char command_buffer[Max_command_buffer] = {0};
    sprintf(command_buffer, "dot -Tpng temp/graph_img/graph.txt"
                            " -o temp/%s", name_output_file);

    if (system(command_buffer))
    {
        Err_report ("Command call error via system\n");
        return TREE_DRAW_GRAPH_ERR;
    }

    FILE *fp_logs = Get_log_file_ptr ();
    if (Check_nullptr (fp_logs))
    {
        Err_report ("Pointer to log file is nullptr\n");
        return TREE_DRAW_GRAPH_ERR;
    }

    fprintf (fp_logs, "<img src = %s />\n", name_output_file);
                                
    return 0;
}

//======================================================================================

static void Draw_nodes_recursive (FILE *fpout, const Node *node, int *counter, const int node_draw_mode)
{
    assert (node  != nullptr && "node is nullptr\n");
    assert (fpout != nullptr && "fpout is nullptr\n");

    (*counter)++;    

    char* ch_right_node_ptr = (char*) node->right;
    char* ch_left_node_ptr  = (char*) node->left;

    char *ch_ptr = (char*) node;

    Draw_node (fpout, node, *counter, node_draw_mode);

    if (!Check_nullptr (node->left)) 
    {
        Draw_nodes_recursive (fpout, node->left,  counter, node_draw_mode);
        fprintf (fpout, "node%p -> node%p[style=filled, color=royalblue3];\n", 
                                ch_ptr, ch_left_node_ptr);    
    }

    if (!Check_nullptr (node->right))
    {
        Draw_nodes_recursive (fpout, node->right, counter, node_draw_mode);
        fprintf (fpout, "node%p -> node%p[style=filled, color=red3];\n", 
                                ch_ptr, ch_right_node_ptr);
    }

    return;
}

//======================================================================================

static void Draw_node (FILE *fpout, const Node *node, const int id, const int node_draw_mode)
{
    assert (node != nullptr && "node is nullptr\n");

    char* ch_right_node_ptr = (char*) node->right;
    char* ch_left_node_ptr  = (char*) node->left;

    char *ch_ptr = (char*) node;


    fprintf (fpout, "node%p [style=filled, shape = record, label =  \"{", ch_ptr);

    if (node_draw_mode & (1 << DRAW_ID))
    {
        fprintf (fpout, "ID: %d ", id);
        if ((node_draw_mode >> (DRAW_ID + 1)) && Mask_draw_node_modes) fprintf (fpout, "|"); //<- checking to make sure not to create an extra field
    }

    if (node_draw_mode & (1 << DRAW_PTR))
    {   
        fprintf (fpout, "NODE POINTER: %p ", ch_ptr);
        if ((node_draw_mode >> (DRAW_PTR + 1)) && Mask_draw_node_modes) fprintf (fpout, "|"); //<- checking to make sure not to create an extra field
    }

    if (node_draw_mode & (1 << DRAW_DATA))
    {
        Draw_node_data (fpout, (AST_data*)node->data);
        if ((node_draw_mode >> (DRAW_DATA + 1)) && Mask_draw_node_modes) fprintf (fpout, "|"); //<- checking to make sure not to create an extra field
    }      

    if (node_draw_mode & (1 << DRAW_SONS_PTR))
    {  
        fprintf (fpout, "left: %p | right: %p ", ch_left_node_ptr, ch_right_node_ptr);
        if ((node_draw_mode >> (DRAW_SONS_PTR + 1)) && Mask_draw_node_modes) fprintf (fpout, "|"); //<- checking to make sure not to create an extra field
    }

    fprintf (fpout, "}\",");


    if (Is_leaf_node (node))
        fprintf (fpout, " fillcolor=lightgreen ];\n");
    else
        fprintf (fpout, " fillcolor=lightgoldenrod1 ];\n");

    return;
}

static void Draw_node_data (FILE *fpout, AST_data* data)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (data  != nullptr && "AST_data is nullptr");

    fprintf (fpout, "{NODE TYPE: %s| ", Name_type_node [data->node_type]);

    switch (data->node_type)
    {
        case NUM:
            fprintf (fpout, "%.3lg}", data->data.val);
            break;

        case VAR:
            fprintf (fpout, "%s}", data->data.obj);
            break;

        case NVAR:
            fprintf (fpout, "%s}", data->data.obj);
            break;

        case OP:
            fprintf (fpout, "%s}", Name_operations[data->data.operation]);
            break;

        case UNKNOWN_T: 
            fprintf (fpout, "UNKNOWN}");
            break;

        default:
            fprintf (fpout, "NOT DATA}");
            break;
    }

    return;
}

//======================================================================================