#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "convert.h"

#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"

static int Write_to_file  (FILE *fpout, const Node *node, const int shift = 0);


static int Write_defs     (FILE *fpout, const Node *node, const int shift = 0);

static int Write_nvar     (FILE *fpout, const Node *node, const int shift = 0);

static int Write_nfun     (FILE *fpout, const Node *node, const int shift = 0);

static int Write_narr     (FILE *fpout, const Node *node, const int shift = 0);


static int Write_call     (FILE *fpout, const Node *node, const int shift = 0);

static int Write_ret      (FILE *fpout, const Node *node, const int shift = 0);

static int Write_par      (FILE *fpout, const Node *node, const int shift = 0);

static int Write_arg      (FILE *fpout, const Node *node, const int shift = 0);



static int Write_const    (FILE *fpout, const Node *node, const int shift = 0, const int num_rep = 0);

static int Write_var      (FILE *fpout, const Node *node, const int shift = 0);

static int Write_arr      (FILE *fpout, const Node *node, const int shift = 0);



static int Write_assig    (FILE *fpout, const Node *node, const int shift = 0);

static int Write_seq      (FILE *fpout, const Node *node, const int shift = 0);

static int Write_block    (FILE *fpout, const Node *node, const int shift = 0);


static int Write_oper     (FILE *fpout, const Node *node, const int shift = 0);


static int Write_if       (FILE *fpout, const Node *node, const int shift = 0);

static int Write_branch   (FILE *fpout, const Node *node, const int shift = 0);


static int Write_while    (FILE *fpout, const Node *node, const int shift = 0);


//======================================================================================================

int Create_convert_file (const Tree *ast_tree, const char* name_output_file)
{
    assert (ast_tree !=  nullptr && "ast_tree is nullptr");

    #ifdef LOOK_CONVERT_AST_TREE
        Draw_database (ast_tree);
    #endif

    FILE* fpout = Open_file_ptr (name_output_file, "w");
    if (Check_nullptr (fpout))
        return PROCESS_ERROR (ERR_FILE_OPEN, "open output file \'%s\' error", name_output_file);

    Write_to_file (fpout, ast_tree->root, 0);

    if (Close_file_ptr (fpout))
        return PROCESS_ERROR (ERR_FILE_OPEN, "open output file \'%s\' error", name_output_file);

    return 0;
}

//======================================================================================================

static int Write_to_file (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");

    if (Check_nullptr ((char*) node))
        return 0;

    switch ((GET_TYPE (node)))
    {
        case DEFS:  Write_defs    (fpout, node, shift);
            break;
        
        case NVAR:  Write_nvar    (fpout, node, shift);
            break;

        case NARR:  Write_narr    (fpout, node, shift);
            break;

        case NFUN: Write_nfun     (fpout, node, shift);
            break;
        
        case PAR:   Write_par     (fpout, node, shift);
            break;

        case ARG:   Write_arg     (fpout, node, shift);
            break;
        
        case CALL:  Write_call    (fpout, node, shift);
            break;

        case RET:   Write_ret     (fpout, node, shift);
            break;

        case OP:    Write_oper    (fpout, node, shift);
            break;
        
        case ASS:   Write_assig   (fpout, node, shift);
            break;

        case CONST: Write_const   (fpout, node, shift);
            break;

        case VAR:   Write_var     (fpout, node, shift);
            break;

        case ARR:  Write_arr      (fpout, node, shift);
            break;

        case SEQ:   Write_seq     (fpout, node, shift);
            break;

        case BLOCK: Write_block   (fpout, node, shift);
            break;

        case IF:    Write_if      (fpout, node, shift);
            break;

        case BRANCH: Write_branch (fpout, node, shift);
            break;

        case WHILE: Write_while   (fpout, node, shift);
            break;

        default:    
            PROCESS_ERROR (UNKNOWN_TYPE_ERR, "UNKNOWN TYPE %d", GET_TYPE (node));
            break;
    } 

    return 0;
}
    

//======================================================================================================

static int Write_defs (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    Write_to_file (fpout, node->left, shift);
    if (GET_TYPE (node->left) == NVAR) fprintf (fpout, ";\n");

    Write_to_file (fpout, node->right, shift);

    if (!Check_nullptr (node->right))
        if (GET_TYPE (node->right) == NVAR) fprintf (fpout, ";\n");

    return 0;
}

//======================================================================================================

static int Write_nvar (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s %s", Name_lang_type_node [NVAR], GET_DATA (node, obj));

    if (!Check_nullptr (node->right))
    {
        fprintf (fpout, " %s ", Name_lang_type_node [ASS]);
        Write_to_file (fpout, node->right, shift);
    }  

    return 0;
}

//======================================================================================================

static int Write_narr (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s %s[", Name_lang_type_node [NARR], GET_DATA (node, obj));

    Write_const (fpout, node->left, shift, Int_num_rep);

    fprintf (fpout, "]");

    return 0;
}

//======================================================================================================

static int Write_nfun (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");

    fprintf (fpout, "%s %s (", Name_lang_type_node [NFUN], GET_DATA (node, obj));
    Write_to_file (fpout, node->left, shift);
    fprintf (fpout, ")\n");
    
    Write_to_file (fpout, node->right, shift);

    fprintf (fpout, "\n");

    return 0;
}

//======================================================================================================


static int Write_par (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s", GET_DATA (node, obj));

    if (!Check_nullptr (node->right))
    {
        fprintf (fpout, ", ");
        Write_to_file (fpout, node->right, shift);
    }

    return 0;
}

//======================================================================================================

static int Write_ret (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s ", Name_lang_type_node [RET]);
    Write_to_file (fpout, node->right, shift);

    return 0;
}

//======================================================================================================

static int Write_call (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s (", GET_DATA (node, obj));

    Write_to_file (fpout, node->right, shift);

    fprintf (fpout, ")");

    return 0;
}

//======================================================================================================

static int Write_arg (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");

    if (!Check_nullptr (node->right))
    {
        Write_to_file (fpout, node->right, shift); 
        fprintf (fpout, ", ");
    }

    Write_to_file (fpout, node->left, shift);

    return 0;
}

//======================================================================================================

static int Write_if (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   

    fprintf (fpout, "%s (", Name_lang_type_node [IF]);
    Write_to_file (fpout, node->left, shift);
    fprintf (fpout, ")\n");
    
    Write_to_file (fpout, node->right, shift);

    fprintf (fpout, "\n");

    return 0;
}

//======================================================================================================

static int Write_branch (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    int cur_type = GET_TYPE (node->left);
    if (cur_type != BLOCK) fprintf (fpout, "%*c", shift, ' ');
    Write_to_file (fpout,  node->left,  shift);
    if (cur_type != IF && cur_type != WHILE && cur_type != BLOCK) fprintf (fpout, ";\n");

    
    if (!Check_nullptr (node->right))   
    {
        fprintf (fpout, "%*c%s\n", shift, ' ', Name_lang_type_node [ELSE]);

        cur_type = GET_TYPE (node->right);
        if (cur_type != BLOCK) fprintf (fpout, "%*c", shift, ' ');
        Write_to_file (fpout,  node->right,  shift);
        if (cur_type != IF && cur_type != WHILE && cur_type != BLOCK) fprintf (fpout, ";\n");

    }

    return 0;
}

//======================================================================================================

static int Write_while (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s (", Name_lang_type_node [WHILE]);
    Write_to_file (fpout, node->left, shift);
    fprintf (fpout, ")\n");
    
    int cur_type = GET_TYPE (node->right);
    if (cur_type != BLOCK) fprintf (fpout, "%*c", shift, ' ');
    Write_to_file (fpout,  node->right,  shift);
    if (cur_type != IF && cur_type != WHILE && cur_type != BLOCK) fprintf (fpout, ";\n");

    fprintf (fpout, "\n");

    return 0;
}

//======================================================================================================

static int Write_oper (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");

    int oper = GET_DATA (node, operation);

    if (!Check_nullptr (node->left))
    {
        fprintf (fpout, "(");
        Write_to_file (fpout,  node->left,  shift);
        fprintf (fpout, ")");
    }

    fprintf (fpout, " %s ", Name_lang_operations [oper]);

    if (!Check_nullptr (node->right))
    {
        fprintf (fpout, "(");
        Write_to_file (fpout,  node->right,  shift);
        fprintf (fpout, ")");
    }

    return 0;
}

//======================================================================================================

static int Write_assig (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
    
    fprintf (fpout, "%s %s ", GET_DATA (node, obj), Name_lang_type_node[ASS]);
    Write_to_file (fpout, node->right, shift);

    return 0;
}

//======================================================================================================

static int Write_seq (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%*c", shift, ' ');

    Write_to_file (fpout, node->left,  shift); 
    
    int cur_type = GET_TYPE (node->left);
    if (cur_type != IF && cur_type != WHILE && cur_type != BLOCK)
        fprintf (fpout, ";\n");
    Write_to_file (fpout, node->right, shift);

    return 0;
}

//======================================================================================================

static int Write_block (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%*c{\n", shift, ' ');
        Write_to_file (fpout, node->right, shift + 2);
    fprintf (fpout, "%*c}\n", shift, ' ');

    return 0;
}

//======================================================================================================

static int Write_const (FILE *fpout, const Node *node, const int shift, const int num_rep)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    if (num_rep)
        fprintf (fpout, "%d", (int) GET_DATA (node, val));
    else
        fprintf (fpout, "%.3lf", GET_DATA (node, val));

    return 0;
}

//======================================================================================================

static int Write_var (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s", GET_DATA (node, obj));
    
    return 0;
}

//======================================================================================================

static int Write_arr (FILE *fpout, const Node *node, const int shift)
{
    assert (fpout != nullptr && "fpout is nullptr");
   
    fprintf (fpout, "%s[", GET_DATA (node, obj));

    Write_to_file (fpout, node->left, shift);

    fprintf (fpout, "]");

    if (!Check_nullptr (node->right))
    {
        fprintf (fpout, " %s ", Name_lang_type_node [ASS]);
        Write_to_file (fpout, node->right, shift);
    }  

    return 0;
}

//======================================================================================================
