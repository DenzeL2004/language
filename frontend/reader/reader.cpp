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

//-------------------------------------------------------------------------------
//RULE: DEFINITION VARIABLE

static Node* Definition_variable (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//RULE: DEFINITION FUNCTION

static Node* Definition_function (int *pos, const Array_struct *tokens);

static Node* Get_param           (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//RULE: EXPRESION

static Node* Parce_expression    (int *pos, const Array_struct *tokens);


static Node* Get_expression      (int *pos, const Array_struct *tokens);

static Node* Get_term            (int *pos, const Array_struct *tokens);

static Node* Get_differentiation (int *pos, const Array_struct *tokens);


static Node* Unique_operation    (int *pos, const Array_struct *tokens);

static Node* Get_priority        (int *pos, const Array_struct *tokens);


static Node* Get_logical_not (int *pos, const Array_struct *tokens);

static Node* Get_logical_and (int *pos, const Array_struct *tokens);

static Node* Get_logical_or (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//RULE: CALL FUNCTION

static Node* Get_call_function (int *pos, const Array_struct *tokens);

static Node* Get_arg           (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------

static Node* Get_block (int *pos, const Array_struct *tokens);

static Node* Get_statement  (int *pos, const Array_struct *tokens);

static Node* Get_assignment (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------
//RULE: CONDITION

static Node* Get_condition  (int *pos, const Array_struct *tokens);

static int Is_compar_oper (const char *token);

static Node* Get_branch (int *pos, const Array_struct *tokens);

//-------------------------------------------------------------------------------

static inline bool Is_terminator (const char *token);

static bool Is_reserved_word (const char *token);

static inline int Is_name (const char *cur_token);

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
        
        if (!strcmp (cur_token, Name_lang_type_node [NVAR]))
            node->left = Definition_variable (pos, tokens);
        else
            node->left = Definition_function (pos, tokens);
    }

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

static Node* Definition_function (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    if (strcmp (cur_token, Name_lang_type_node [NFUNC])) return nullptr;
   
    (*pos)++;

    Node *node = Create_empty_node ();
    DEF_TYPE (node, NFUNC);
    
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (Is_reserved_word (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a function name", cur_token);
        return nullptr;
    }

    CHANGE_DATA (node, obj, cur_token);

    (*pos)++;
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (strcmp (cur_token, "("))
    {
        PROCESS_ERROR (SYNTAX_ERR, "Definition function must has \'(\' after name.\n"
                                   "cur_token: %s", cur_token);
        return nullptr;
    }

    (*pos)++;

    node->left = Get_param (pos, tokens);
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    Node *tmp_node = node->left;
    while (!strcmp (cur_token, ","))
    {
        (*pos)++;

        tmp_node->right = Get_param (pos, tokens);
        tmp_node = tmp_node->right;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }
        
    if (strcmp(cur_token, ")"))
    {
        PROCESS_ERROR (SYNTAX_ERR, "not \')\' after \'(\',"
                                    " cur_token: |%s|.\n", cur_token);
        return nullptr;
    }

    (*pos)++;

    node->right = Get_block (pos, tokens);

    if (Check_nullptr (node->right))
    {
        PROCESS_ERROR (SYNTAX_ERR, "function body not defined"
                                    " last_token: |%s|.\n", (char*) Array_get_ptr_by_ind (tokens, *pos));
        return nullptr;
    }

    return node;
}

//=================================================================================================

static Node* Get_param (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    
    if (!Is_name (cur_token)) return nullptr;

    if (Is_reserved_word (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a parameter name", cur_token);
        return nullptr;
    }

    Node *node = Create_object_node (cur_token, nullptr, nullptr);
    DEF_TYPE (node, PARAM);

    (*pos)++;

    return node;
}

//=================================================================================================

static Node* Get_block (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    
    if (strcmp (cur_token, "{"))
    {
        PROCESS_ERROR (SYNTAX_ERR, "block must start with {.\n"
                                   "cur_token: %s", cur_token);
        return nullptr;
    }

    (*pos)++;
    Node *node = Create_empty_node ();
    if (Check_nullptr (node))
    {
        PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, node is nullptr\n");
        return nullptr;
    }

    DEF_TYPE (node, SEQ);
    node->left = Get_statement (pos, tokens);

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    
    Node* tmp_node = node;
    while (strcmp (cur_token, "}"))
    {
        Node* next_stmt = Create_empty_node ();
        if (Check_nullptr (next_stmt))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, next_stmt is nullptr\n");
            return nullptr;
        }

        DEF_TYPE (next_stmt, SEQ);

        next_stmt->left = Get_statement (pos, tokens);

        if (Check_nullptr (next_stmt->left))
        {
            PROCESS_ERROR (SYNTAX_ERR, "gone into an endless loop. cur_token: %s\n", cur_token);
            return nullptr;
        }

        tmp_node->right = next_stmt;
        tmp_node = next_stmt;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }

    if (strcmp (cur_token, "}"))
    {
        PROCESS_ERROR (SYNTAX_ERR, "block must end with }.\n"
                                   "cur_token: %s", cur_token);
        return nullptr;
    }

    (*pos)++;
    
    return node;
}

//=================================================================================================

static Node* Get_statement (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char* cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (Is_reserved_word (cur_token))
    {
        if (!strcmp (cur_token, Name_lang_type_node [NVAR]))
            return Definition_variable (pos, tokens);

        if (!strcmp (cur_token, Name_lang_type_node [WHILE]))
        {
            Node *node = Create_empty_node ();
            DEF_TYPE (node, WHILE);
            
            (*pos)++;
        
            node->left = Get_condition (pos, tokens);
            
            node->right = Get_statement (pos, tokens);

            if (Check_nullptr (node->right))
            {
                PROCESS_ERROR (SYNTAX_ERR, "No execution command after conditions,"
                                           " cur_token: %s\n", cur_token);
                return nullptr;
            }

            return node;
        }

        if (!strcmp (cur_token, Name_lang_type_node [IF]))
        {
            Node *node = Create_empty_node ();
            DEF_TYPE (node, IF);

            (*pos)++;
        
            node->left  = Get_condition (pos, tokens);
            node->right = Get_branch (pos, tokens);

            if (Check_nullptr (node->right))
            {
                PROCESS_ERROR (SYNTAX_ERR, "No execution command after conditions,"
                                           " cur_token: %s\n", cur_token);
                return nullptr;
            }

            return node;
        }

        if (!strcmp (cur_token, Name_lang_type_node [RET]))
        {
            Node *node = Create_empty_node ();
            DEF_TYPE (node, RET);
            (*pos)++;
            
            node->right = Parce_expression (pos, tokens);

            if (Check_nullptr (node->right))
            {
                PROCESS_ERROR (SYNTAX_ERR, "there must always be a return value\n");
                return nullptr;
            }

            return node;
        }
    }

    else
    {
        if (!strcmp (cur_token, "{"))
        {
            return Get_block (pos, tokens);
        }

        return Get_assignment (pos, tokens);
    }

    return nullptr;
}

//=================================================================================================

static Node* Get_assignment (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = nullptr;
    char* cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (!Is_name (cur_token)) 
        return nullptr;

    (*pos)++;

    char* next_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    if (strcmp (next_token, Name_lang_type_node [ASS]))
    {
        PROCESS_ERROR (SYNTAX_ERR, "The variable must be followed by an assignment.\n"
                                    "cur_token: %s, next_token: %s, pos = %d", cur_token, next_token, *pos);
        return nullptr;
    }

    (*pos)++;

    node = Create_empty_node ();
    DEF_TYPE (node, ASS);
    CHANGE_DATA (node, obj, cur_token);

    node->right = Parce_expression (pos, tokens);
    if (Check_nullptr (node->right))
    {
        PROCESS_ERROR (SYNTAX_ERR, "After assignment must not be an emptu token\n"
                                    "cur_token: %s, next_token: %s, pos = %d", cur_token, next_token, *pos);
        return nullptr;
    }

    return node;
}

//=================================================================================================

static Node* Get_condition (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    if (strcmp (cur_token, "(")) 
    {
        PROCESS_ERROR (SYNTAX_ERR, "The condition must start with \'(\'");
        return nullptr;
    }

    (*pos)++;

    Node *node = Get_logical_or (pos, tokens);
   
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    
    if (Is_compar_oper (cur_token))
    {
        Node* tmp_node = Create_operation_node (Is_compar_oper (cur_token), nullptr, nullptr);
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        (*pos)++;
        tmp_node->right = Get_logical_or (pos, tokens);
        tmp_node->left  = node;
        node            = tmp_node;
    }

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (strcmp (cur_token, ")")) 
    {
        PROCESS_ERROR (SYNTAX_ERR, "The condition must end by \')\',"
                                   " cur_token: %s\n", cur_token);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//=================================================================================================

static Node* Get_branch (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Create_empty_node ();
    DEF_TYPE (node, BRANCH);

    node->left = Get_statement (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    if (!strcmp (cur_token, Name_lang_type_node [ELSE]))
    {
        (*pos)++;
        node->right = Get_statement (pos, tokens);
    }

    return node;
}

//=================================================================================================

static int Is_compar_oper (const char *token)
{
    assert (token != nullptr && "token is nullptr");

    if (!strcmp (token, Name_lang_operations [MORE]))    return MORE;
    if (!strcmp (token, Name_lang_operations [NOTMORE])) return NOTMORE;

    if (!strcmp (token, Name_lang_operations [LESS]))    return LESS;
    if (!strcmp (token, Name_lang_operations [NOTLESS])) return NOTLESS;
    
    if (!strcmp (token, Name_lang_operations [EQ]))    return MORE;
    if (!strcmp (token, Name_lang_operations [NOTEQ])) return MORE;

    return 0;
}

//=================================================================================================

static Node* Definition_variable (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (strcmp (cur_token, Name_lang_type_node [NVAR])) return nullptr;
   
    (*pos)++;

    Node *node = Create_empty_node ();
    DEF_TYPE (node, NVAR);

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (Is_reserved_word (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a variable name", cur_token);
        return nullptr;
    }

    (*pos)++;

    CHANGE_DATA (node, obj, cur_token);

    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (!strcmp (cur_token, Name_lang_type_node [ASS]))
        (*pos)++;

    node->right = Parce_expression (pos, tokens);

    return node;
}

//=================================================================================================

static Node* Parce_expression (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Get_logical_or (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    if (!Is_terminator (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "Expression does not end \';\'.\n"
                                   "Current token: \'%s\', pos = %d.", cur_token, *pos);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//=================================================================================================

static Node* Get_logical_or (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node* node = Get_logical_and (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
 
    while (!strcmp (cur_token, Name_lang_operations [OR]))
    {
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        tmp_node->left = node;
        node = tmp_node;

        (*pos)++;

        node->right = Get_logical_and (pos, tokens);

        node = Create_operation_node (OR, node->left, node->right);   
        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }

    return node;
}

//=================================================================================================

static Node* Get_logical_and (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node* node = Get_expression (pos, tokens);

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
 
    while (!strcmp (cur_token, Name_lang_operations [AND]))
    {
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        tmp_node->left = node;
        node = tmp_node;

        (*pos)++;

        node->right = Get_expression (pos, tokens);

        node = Create_operation_node (AND, node->left, node->right);   
        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }

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
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        tmp_node->left = node;
        node = tmp_node;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
        (*pos)++;

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
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        tmp_node->left = node;
        node = tmp_node;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
        (*pos)++;

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
    
    if (!strcmp (cur_token, Name_lang_operations [DF]))
    {
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        tmp_node->right = node;
        node = tmp_node;

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

    if (!strcmp (cur_token, Name_lang_operations [SUB]))
    {
        node = Create_operation_node (MUL, nullptr, nullptr);
        node->left = Create_value_node (-1, nullptr, nullptr);

        (*pos)++;
        node->right = Get_priority (pos, tokens);

        if (Check_nullptr (node->right))
        {
            PROCESS_ERROR (SYNTAX_ERR, "After unary minus must be num/var/(expression)\n");
            return nullptr;
        }

        return node;
    }

    if (!strcmp (cur_token, Name_lang_operations [NOT]))
    {
        node = Get_logical_not (pos, tokens);
        return node;
    }

    if (Is_name (cur_token))
    {
        char *next_token = (char*) Array_get_ptr_by_ind (tokens, *pos + 1);

        if (!strcmp (next_token, "("))
            return Get_call_function (pos, tokens);
    }

    node = Get_priority (pos, tokens);

    return node;
}

//=================================================================================================

static Node* Get_logical_not (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Create_operation_node (NOT, nullptr, nullptr);

    (*pos)++;
    node->right = Get_priority (pos, tokens);

    if (Check_nullptr (node->right))
    {
        PROCESS_ERROR (SYNTAX_ERR, "After operation not must be num/var/(expression)\n");
        return nullptr;
    }
    
    return node;
}

//=================================================================================================

static Node* Get_call_function (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    char *cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    Node* node = Create_empty_node ();
    DEF_TYPE (node, CALL);
    CHANGE_DATA (node, obj, cur_token);

    if (Is_reserved_word (cur_token))
    {
        PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a variable name", cur_token);
        return nullptr;
    }    

    (*pos) += 2;
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    node->right = Get_arg (pos, tokens);
    cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);

    while (!strcmp (cur_token, ","))
    {
        Node* tmp_node = Create_node ();
        if (Check_nullptr (tmp_node))
        {
            PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory allocation error, tmp_node is nullptr\n");
            return nullptr;
        }

        (*pos)++;

        tmp_node = Get_arg (pos, tokens);

        tmp_node->right = node->right;
        node->right = tmp_node;

        cur_token = (char*) Array_get_ptr_by_ind (tokens, *pos);
    }
        
    if (strcmp(cur_token, ")"))
    {
        PROCESS_ERROR (SYNTAX_ERR, "not \')\' after \'(\',"
                                    " cur_token: |%s|.\npos = %d", cur_token, *pos);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//=================================================================================================

static Node* Get_arg (int *pos, const Array_struct *tokens)
{
    assert (tokens != nullptr && "tokens is nullptr");
    assert (pos != nullptr && "pos is nullptr");

    Node *node = Create_empty_node ();
    DEF_TYPE (node, ARG);

    node->left = Get_expression (pos, tokens);

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

        node = Get_logical_or (pos, tokens);

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
        if (isdigit (cur_token[0]))
        {
            double val = strtod (cur_token, nullptr);
            if (errno == ERANGE)
                PROCESS_ERROR (STR_CONVERT_ERR, "Unable to convert from string to double number, cur_token: %s", cur_token);
            
            node = Create_value_node (val, nullptr, nullptr);

            (*pos)++;
        }

        if (Is_name (cur_token))
        {
            if (Is_reserved_word (cur_token))
            {
                PROCESS_ERROR (SYNTAX_ERR, "a reserved word \"%s\" has been used as a variable name", cur_token);
                return nullptr;
            }

            node = Create_object_node (cur_token, nullptr, nullptr);
            (*pos)++;
        }
    }

    return node;
}

//=================================================================================================

static bool Is_reserved_word (const char *token)
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

static inline int Is_name (const char *token)
{
    assert (token != nullptr && "token is nullptr");

    return (isalpha (token[0]) || token[0] == '_');
}

//=================================================================================================

static inline bool Is_terminator (const char *token)
{
    assert (token != nullptr && "token is nullptr");
    
    return (!strcmp (token, ";"));    
} 

//=================================================================================================
