#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "backend.h"

#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"


#include "../src/array/array.h"
#include "../src/name_table/name_table.h"

#include "../src/process_text/process_text.h"
#include "../src/lexer/lexer.h"


static Node* Build_tree (int *pos, const Array_struct *tokens);

static Node* Read_node_from_buffer (int *pos, const Array_struct *tokens);

static int Define_operation (const char *token);

static int Define_type (const char *token);


static int Namespace_check     (const Tree *ast_tree);

static int Check_function_call (const Node *node, Name_table *function_names);


int Create_asm_file (const Tree *ast_tree, const char* name_output_file)
{
    assert (ast_tree !=  nullptr && "ast_tree is nullptr");

    if (Namespace_check (ast_tree))
        return PROCESS_ERROR (COMPILATION_ERR, "\n");

    return 0;
}

//======================================================================================================

static int Namespace_check (const Tree *ast_tree)
{
    assert (ast_tree != nullptr && "ast_tree is nullptr");

    Name_table function_names = {};

    if (Name_table_ctor (&function_names))
        return PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Ctor Name_table \'function_names\' error\n");

    int check_res = Check_function_call (ast_tree->root, &function_names);
    if (check_res != 0)
        PROCESS_ERROR (INVALID_CALL, "invalid function call\n");

    if (Name_table_dtor (&function_names))
        return PROCESS_ERROR (NAME_TABLE_DTOR_ERR, "Dtor \'function_names\' error\n");   

    return 0;
}

//======================================================================================================

static int Check_function_call (const Node *node, Name_table *function_names)
{
    assert (node != nullptr && "node is nullptr");
    assert (function_names != nullptr && "function is nullptr");

    int cnt = 0;

    if (!Check_nullptr (node->left))
        cnt += Check_function_call (node->left, function_names);
    
    if (GET_TYPE (node) == ARG || GET_TYPE (node) == PAR) cnt++;

    if (GET_TYPE (node) == NFUNC)
    {
        int id = Add_object (function_names, GET_DATA (node, obj), OBJ_FUNC);
        function_names->objects[id].data = calloc (1, sizeof (int)); 

        *((int*) function_names->objects[id].data) = cnt;
        cnt = 0;
    }

    if (GET_TYPE (node) == CALL)
    {
        int id = Find_id_object (function_names, GET_DATA (node, obj));
        if (id != Not_init_object)
        {
            int cnt_param = *((int*) function_names->objects[id].data);
            if (cnt != cnt_param)
            {
                PROCESS_ERROR (INCORRECT_CNT_ARG, "\nFunction \'%s\' function has the wrong number of arguments: %d. "
                               "Must be: %d\n", GET_DATA (node, obj), cnt, cnt_param);
                cnt = Inf;
            }
            
            else
                cnt = 0;
        }
    }    

    if (!Check_nullptr (node->right))
        cnt += Check_function_call (node->right, function_names);
    
    return cnt;
}

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

    Array_recalloc (&tokens, cnt_tokens + 1);
    Array_set_ptr_by_ind (&tokens, cnt_tokens, strdup (""));
    
    int pos = 0;
    ast_tree->root = Build_tree (&pos, &tokens);

    if (Check_nullptr (ast_tree->root))   
        return PROCESS_ERROR (READ_AST_FORMAT, "Error read a tree with a pointer to the root |%p|",
                                               (char*) ast_tree->root);

    if (Array_dtor (&tokens))
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokens dtor failed");

    #ifdef LOOK_BACKEND_AST_TREE
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
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_tolen: %s", cur_token);
        return nullptr;
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (!strcmp (cur_token, "NULL"))
        CHANGE_DATA (node, obj, nullptr);
    else if (isdigit (cur_token[0]))
        CHANGE_DATA (node, val, atof (cur_token));
    else
    {
        int op = Define_operation (cur_token);
        if (op == UNKNOWN_OP)
            CHANGE_DATA (node, obj, cur_token);
        else
            CHANGE_DATA (node, operation, op);
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (strcmp (cur_token, ","))
    {
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_tolen: %s", cur_token);
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