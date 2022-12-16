#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "frontend.h"

#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"

#include "../src/process_text/process_text.h"


#include "lexer/lexer.h"
#include "reader/reader.h"

static int Check_file_extension (const char *name_file);

static int Write_node_ast_format (FILE *fpout, const Node *node, const int shift = 0);

//======================================================================================================

int Frontend_struct_ctor (Frontend_struct *frontend_struct)
{
    assert (frontend_struct != nullptr && "frontend struct is nullptr");

    if (Tree_ctor (&frontend_struct->ast_tree))
        return PROCESS_ERROR (TREE_CTOR_ERR, "Ctor tree error\n");

    frontend_struct->cnt_tokens = 0;

    if (Array_ctor (&frontend_struct->tokens, Init_array_capacity))
        return PROCESS_ERROR (ARRAY_CTOR_ERR, "Ctor array error\n");

    return 0;
}

//======================================================================================================

int Frontend_struct_dtor (Frontend_struct *frontend_struct)
{
    assert (frontend_struct != nullptr && "frontend struct is nullptr");

    if (Tree_dtor (&frontend_struct->ast_tree))
        return PROCESS_ERROR (TREE_DTOR_ERR, "Dtor tree error\n");


    frontend_struct->cnt_tokens = 0;

    for (int id = 0; id < frontend_struct->tokens.capacity; id++)
        free (frontend_struct->tokens.data[id]);

    if (Array_dtor (&frontend_struct->tokens))
        return PROCESS_ERROR (ARRAY_DTOR_ERR, "Dtor array error\n");

    return 0;
}

//======================================================================================================

int Read_source_file (Frontend_struct *frontend_struct, const char *name_input_file)
{
    assert (name_input_file != nullptr && "name_input_file is nullptr");
    assert (frontend_struct != nullptr && "frontend_struct is nullptr");

    if (Check_file_extension (name_input_file))
        return PROCESS_ERROR (INVALID_EXTENSION, "invalid file extension %s\n", name_input_file);

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening iputfile file named \"%s\"\n", 
                                                                           name_input_file);

    Text_info text = {};
    
    if (Text_read (fdin, &text))
        return PROCESS_ERROR (ERR_FILE_READING, "Error reading into Text_info structure\n");

    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE, "Error close input file named \"%s\"\n", name_input_file);

    frontend_struct->cnt_tokens =  Lexical_analysis_str (text.text_buf, &frontend_struct->tokens);
    
    if (frontend_struct->cnt_tokens <= 0)
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokenization failed with an error."
                              " Count tokens = %d\n", frontend_struct->cnt_tokens);

    Free_buffer (&text);

    Array_recalloc (&frontend_struct->tokens, frontend_struct->cnt_tokens + 1);
    Array_set_ptr_by_ind (&frontend_struct->tokens, frontend_struct->cnt_tokens, strdup (""));


    int result_parce = Parce_source_file (&frontend_struct->ast_tree, 
                                          &frontend_struct->tokens, frontend_struct->cnt_tokens);
    
    #ifdef LOOK_FRONTEND_AST_TREE_
        Draw_database (&frontend_struct->ast_tree);
    #endif
    
    if (result_parce)
        return PROCESS_ERROR (PARCE_ERR, "Parce result = %d", result_parce);
    
    return 0;
}

//======================================================================================================

static int Check_file_extension (const char *name_file)
{
    assert (name_file != nullptr && "name_file is nullptr");

    int len_str = strlen (name_file);

    return strncmp (name_file + (len_str - 4), Extension, 4);
}

//======================================================================================================

int Write_database (const Tree *tree)
{
    assert (tree != nullptr && "tree is nullptr");

    FILE *fpout = Open_file_ptr (ast_format_file, "w");
    if (Check_nullptr (fpout))
        return PROCESS_ERROR (ERR_FILE_OPEN, "Opening the output file\n");

    Write_node_ast_format (fpout, tree->root);

    return 0;
}

//======================================================================================================

static int Write_node_ast_format (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
    
    if (Check_nullptr ((char*) node))
    {
        fprintf (fpout, "%*c{}", shift, ' ');
        return 0;
    }

    AST_data* ast_data = (AST_data*) node->data;

    fprintf (fpout, "%*c{ %s, ", shift, ' ', Name_type_node [ast_data->node_type]);
    
    if (ast_data->node_type == NUM)
        fprintf (fpout, "%.3lf, ", ast_data->data.val);
    
    else if (ast_data->node_type == OP)
        fprintf (fpout, "%s, ", Name_operations [ast_data->data.operation]);

    else 
        if (Check_nullptr ((char*) ast_data->data.obj))
            fprintf (fpout, "NULL, ");
        else
            fprintf (fpout, "%s, ", ast_data->data.obj);

    fprintf (fpout, "\n");
    
    Write_node_ast_format (fpout, node->left, shift + 3);
    fprintf (fpout, ",\n");

    Write_node_ast_format (fpout, node->right, shift + 3);
    fprintf (fpout, "\n");

    fprintf (fpout, "%*c}", shift, ' ');

    return 0;
}

//======================================================================================================