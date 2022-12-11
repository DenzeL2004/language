#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "reader.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"


static Node* Definition_objects (int *pos, const Array_struct *tokens);

static inline int Is_definition (char *cur_token);


static Node* Definition_variable (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------
//RULE: EXPRESION

static Node* Parce_expression    (int *pos, const Array_struct *tokens);


static Node* Get_expression      (int *pos, const Array_struct *tokens);

static Node* Get_term            (int *pos, const Array_struct *tokens);

static Node* Get_differentiation (int *pos, const Array_struct *tokens);


static Node* Unique_operation    (int *pos, const Array_struct *tokens);

static Node* Get_priority        (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------


static bool Is_reserved_word (char *token);

static inline int Is_num (char *cur_token);

static inline int Is_name (char *cur_token);

//=================================================================================================

int Parce_source_file (Tree *ast_tree, const Array_struct *tokens, const int cnt_tokens)
{
    assert (ast_tree  != nullptr && "ast_tree is nullptr");
    assert (tokens    != nullptr && "tokens is nullptr");

    Free_AST_nodes_data (ast_tree->root);

    int pos = 0;   
    ast_tree->root = Definition_objects (&pos, tokens);

    if (Check_nullptr (ast_tree->root))
        return PROCESS_ERROR (PARCE_ERR, "root is nullptr, pos = %d\n", pos);

    if (pos != cnt_tokens)
        return PROCESS_ERROR (SYNTAX_ERR, "The number of tokens does not match what was specified.\n"
                                          "Tokens cnt = %d, pos = %d.\n", cnt_tokens, pos);

    return 0;
}

//=================================================================================================

static Node* Definition_objects (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    Node* node = nullptr;

    if (Is_definition (cur_token))
    {
        
        node = Create_empty_node ();
        DEF_TYPE (node, DEFS);
        
        (*pos)++;
        

        if (!strcmp (cur_token, Name_lang_type_node [NVAR]))
            node->left = Definition_variable (pos, tokens);
        else
            Dump_func (1);
    }

    if (Check_nullptr (node))   return nullptr;

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);


    if (Is_definition (cur_token))
        node->right = Definition_objects (pos, tokens);

    return node;
}

//=================================================================================================

static inline int Is_definition (char *token)
{
    return (!strcmp (token, Name_lang_type_node [NVAR]) || !strcmp (token, Name_lang_type_node [NFUNC]));
}

//=================================================================================================

static Node* Definition_variable (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Create_empty_node ();
    DEF_TYPE (node, NVAR);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (Is_reserved_word (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a variable name", cur_token);
        return nullptr;
    }

    (*pos)++;

    CHANGE_DATA (node, obj, cur_token);

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (!strcmp (cur_token, Name_lang_operations [ASS]))
        (*pos)++;

    node->right = Parce_expression (pos, tokens);
    
    return node;
}

//=================================================================================================

static Node* Parce_expression (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Get_expression (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (strcmp (cur_token, ";"))
    {
        PROCESS_ERROR (SYNTAX_ERR, "Expression does not end \';\'.\n"
                                   "Current token: \'%s\', pos = %d.", cur_token, *pos);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//=================================================================================================

static Node* Get_expression (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node* node = Get_term (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
 
    while (!strcmp (cur_token, Name_lang_operations [ADD]) || 
           !strcmp (cur_token, Name_lang_operations [SUB]))
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->left = node;
        node = new_node;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
        (*pos)++;

        node->right = Create_node ();
        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, right son is nullptr\n");
            return nullptr;
        }

        node->right = Get_term (pos, tokens);

        if (!strcmp (cur_token, Name_lang_operations[ADD]))
            node = Create_operation_node (ADD, node->left, node->right);

        if (!strcmp (cur_token, Name_lang_operations[SUB]))
            node = Create_operation_node (SUB, node->left, node->right);
   
        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }

    return node;
}

//=================================================================================================

static Node* Get_term (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");


    Node* node = Get_differentiation (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
 
    while (!strcmp (cur_token, Name_lang_operations[MUL]) || 
           !strcmp (cur_token, Name_lang_operations[DIV]))
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->left = node;
        node = new_node;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
        (*pos)++;

        node->right = Create_node ();
        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, right son is nullptr\n");
            return nullptr;
        }

        node->right = Get_differentiation (pos, tokens);

        if (!strcmp (cur_token, Name_lang_operations[MUL]))
            node = Create_operation_node (MUL, node->left, node->right);

        if (!strcmp (cur_token, Name_lang_operations[DIV]))
            node = Create_operation_node (DIV, node->left, node->right);

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }

    return node;   
}

//=================================================================================================

static Node* Get_differentiation (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");


    Node* node = Unique_operation (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    
    if (!strcmp (cur_token, "\'"))
    {
        Node* new_node = Create_node ();
        if (Check_nullptr (new_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, new node is nullptr\n");
            return nullptr;
        }

        new_node->right = node;
        node = new_node;

        (*pos)++;
        
        node = Create_operation_node (DF, nullptr, node->right);
    }

    return node;   
}

//=================================================================================================

static Node* Unique_operation (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node* node = nullptr;

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (Is_name (cur_token))
    {
        char *next_token = (char*) Array_get_ptr_by_ind (tokens, *pos + 1);

        if (!strcmp (next_token, "("))
            return nullptr;// TODO: Get_call_function
    }

    if (!strcmp (cur_token, Name_lang_operations [SUB]))
    {
        (*pos)++;

        node = Create_operation_node (MUL, nullptr, nullptr);
        node->left = Create_value_node (-1, nullptr, nullptr);
        node->right = Get_priority (pos, tokens);

        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (SYNTAX_ERR, "After unary minus must be num/var/expression");
            return nullptr;
        }

        return node;
    }

    node = Get_priority (pos, tokens);

    return node;
}



//=================================================================================================

static Node* Get_priority (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");
    
    Node* node = nullptr;

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (!strcmp(cur_token, "("))
    {
        (*pos)++;

        node = Get_expression (pos, tokens);

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

        if (strcmp(cur_token, ")"))
        {
            PROCESS_ERROR (SYNTAX_ERR, "not \')\' after \'(\',"
                                       " cur_token: |%s|.\npos = %d", cur_token, *pos);
            return nullptr;
        }
        
        (*pos)++;
    }

    else
    {
        if (Is_num (cur_token))
        {
            double val = strtod (cur_token, nullptr);
            if (errno == ERANGE)
                PROCESS_ERROR (STR_CONVERT_ERR, "Unable to convert from string to double number, cur_token: %s", cur_token);
            
            node = Create_value_node (val, nullptr, nullptr);

            (*pos)++;
        }

        if (Is_name (cur_token))
        {
            node = Create_object_node (cur_token, nullptr, nullptr);
            (*pos)++;
        }
    }

    return node;
}

//=================================================================================================

static bool Is_reserved_word (char *token)
{
    assert (token != nullptr && "token is nullptr");

    int cnt = sizeof (Name_lang_operations) / sizeof (Name_lang_operations[0]);

    for (int id = 0; id < cnt; id++)
    {
        if (!strcmp (Name_lang_operations[id], token))
            return true;
    }

    cnt = sizeof (Name_lang_type_node) / sizeof (Name_lang_type_node[0]);

    for (int id = 0; id < cnt; id++)
    {
        if (!strcmp (Name_lang_type_node[id], token))
            return true;
    }

    return false;
}

//=================================================================================================

static inline int Is_num (char *token)
{
    assert (token != nullptr && "token is nullptr");

    return ((token[0] != '\0') && (token[0] >= '0') && (token[0] <= '9'));
}

//=================================================================================================

static inline int Is_name (char *token)
{
    assert (token != nullptr && "token is nullptr");

    return ((token[0] != '\0') && (((token[0] >= 'a') && (token[0] <= 'z')) ||
                                   ((token[0] >= 'A') && (token[0] <= 'Z'))));
}

//=================================================================================================
