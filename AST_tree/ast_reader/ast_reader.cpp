#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "ast_reader.h"

#include "../../src/Generals_func/generals.h"
#include "../../src/log_info/log_errors.h"


#include "../../src/array/array.h"

#include "../../src/process_text/process_text.h"
#include "../../src/lexer/lexer.h"



static Node* Build_tree (int *pos, const Array_struct *tokens);

static Node* Read_node_from_buffer (int *pos, const Array_struct *tokens);

static int Define_operation (const char *token);

static int Define_type (const char *token);

//======================================================================================================

int Load_ast_tree (Tree *ast_tree, const char *name_input_file)
{
    assert (ast_tree != nullptr && "ast_tree is nullptr");
    assert (name_input_file != nullptr && "name_input_file is nullptr");

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening iputfile file named \"%s\"\n", name_input_file);

    Text_info text = {};
    
    if (Text_read (fdin, &text))
        return PROCESS_ERROR (ERR_FILE_READING, "Error reading into Text_info structure\n");

    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE,"Error close input file named \"%s\"\n", name_input_file);
    
    Array_struct tokens = {};
    if (Array_ctor (&tokens, Init_array_capacity))
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokens ctor failed");

    int cnt_tokens =  Lexical_analysis_str (text.text_buf, &tokens);
    
    if (cnt_tokens <= 0)
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokenization failed with an error."
                              " Count tokens = %d\n", cnt_tokens);

    Free_buffer (&text);

    Array_set_ptr_by_ind (&tokens, cnt_tokens, strdup (""));

    int pos = 0;
    ast_tree->root = Build_tree (&pos, &tokens);
    
    if (Check_nullptr (ast_tree->root))   
        return PROCESS_ERROR (READ_AST_FORMAT, "Error read a tree with a pointer to the root |%p|",
                                               (char*) ast_tree->root);

    if (Array_dtor (&tokens))
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokens dtor failed");

    #ifdef LOOK_READ_AST_TREE
        Draw_database (ast_tree);
    #endif

    return 0;
}

//======================================================================================================

#define GET_TOKEN(pos) (char*) Array_get_ptr_by_ind (tokens, (pos))

static Node* Build_tree (int *pos, const Array_struct *tokens)
{
    assert (pos != nullptr && "pos ins nullptr");
    assert (tokens != nullptr && "tokens ins nullptr");

    char *cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, "{"))
    {
        PROCESS_ERROR (READ_AST_FORMAT, "node must start with {\n"
                                        "cur token: |%s|", GET_TOKEN (*pos));
        return nullptr;
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);
    
    if (!strcmp (cur_token, "}"))
    {
        (*pos)++;
        return nullptr;
    }
    
    Node *node = Read_node_from_buffer (pos, tokens); 

    node->left  = Build_tree (pos, tokens); 
    
    cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, ","))
    {
        PROCESS_ERROR (READ_AST_FORMAT, "After the left son there should be \',\'.\n"
                                        "cur token: |%s|", cur_token);
        return nullptr;
    }

    (*pos)++;

    node->right = Build_tree (pos, tokens);

    cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, "}")) 
    {
        PROCESS_ERROR (READ_AST_FORMAT, "After the left son there should be \'}\'.\n"
                                        "cur token: |%s|\n", cur_token);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//======================================================================================================

static Node* Read_node_from_buffer (int *pos, const Array_struct *tokens)
{
    assert (pos != nullptr && "pos ins nullptr");
    assert (tokens != nullptr && "tokens ins nullptr");

    char *cur_token = GET_TOKEN (*pos);

    Node *node = Create_empty_node ();
    DEF_TYPE (node, Define_type (cur_token));

    (*pos)++;
    cur_token = GET_TOKEN (*pos);
    

    if (strcmp (cur_token, ","))
    {
        printf ("*pos %d\n", *pos);
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_token: %s", cur_token);
        return nullptr;
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);
    

    if (!strcmp (cur_token, "NULL"))
        CHANGE_DATA (node, obj, nullptr);
    else
    {
        if (Check_num (cur_token))
        {
            if (!strcmp (cur_token, "-")) (*pos)++;
            CHANGE_DATA (node, val, atof (GET_TOKEN (*pos)));
        }

        else
        {
            int op = Define_operation (cur_token);
            if (op == UNKNOWN_OP)
                CHANGE_DATA (node, obj, cur_token);
            else
                CHANGE_DATA (node, operation, op);
        }
    }
        

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (strcmp (cur_token, ","))
    {
         printf ("*pos %d\n", *pos);
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_token: %s", cur_token);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//======================================================================================================

static int Define_type (const char *token)
{
    assert (token != nullptr && "tokens is nullptr");

    int cnt_words = sizeof (Name_type_node)/ sizeof (Name_type_node [0]);
    for (int id = 0; id < cnt_words; id++)
        if (!strcmp (token, Name_type_node [id]))
            return id;

    return UNKNOWN_T;
}


//======================================================================================================

static int Define_operation (const char *token)
{
    assert (token != nullptr && "tokens is nullptr");

    int cnt_words = sizeof (Name_operations)/ sizeof (Name_operations [0]);
    for (int id = 0; id < cnt_words; id++)
        if (!strcmp (token, Name_operations [id]))
            return id;

    return UNKNOWN_OP;
}

//======================================================================================================