#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "frontend.h"

#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"

#include "../src/process_text/process_text.h"


#include "AST_tree/AST_tree.h"
#include "AST_tree/AST_draw_tree.h"

#include "lexer/lexer.h"
#include "reader/reader.h"

static int Check_file_extension (const char *name_file);

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
    
    #ifdef LOOK_AST_TREE
        Draw_database (&frontend_struct->ast_tree);
    #endif
    
    if (result_parce)
        return PROCESS_ERROR (PARCE_ERR, "Parce result = %d", result_parce);
    
    return 0;
}

//======================================================================================================

int Draw_database (Tree *tree, const int node_mode)
{
    assert (tree != nullptr && "tree is nullptr");

    static int Cnt_graphs = 0;      //<-To display the current tree view

    char name_output_file[Max_command_buffer] = "";
    sprintf (name_output_file, "graph_img\\picture%d.png", Cnt_graphs); 

    Cnt_graphs++;

    if (Draw_tree_graph (tree, name_output_file, node_mode))
        return PROCESS_ERROR (DRAW_DATABASE_ERR, "Error in graph drawing\n");

    char command_buffer[Max_command_buffer] = {0};
    sprintf(command_buffer, "@temp\\%s", name_output_file);

    if (system (command_buffer))
        return PROCESS_ERROR (DRAW_DATABASE_ERR, "Failed to open image\n");

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