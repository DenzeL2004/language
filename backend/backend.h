#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "../config/AST_tree/AST_tree.h"
#include "../config/AST_tree/AST_draw_tree.h"

#include "../config/language_config.h"
#include "../config/language_dsl.h"

enum Backend_func_err
{
    READ_AST_FORMAT     = -1,

    READ_NODE_ERR       = -2,

    INCORRECT_CNT_ARG   = -3,   

    INVALID_CALL        = -4,
    COMPILATION_ERR     = -5,
};

int Load_ast_tree (Tree *ast_tree, const char *name_input_file);

int Create_asm_file (const Tree *ast_tree, const char* name_output_file = Default_backend_output);

#define LOOK_BACKEND_AST_TREE

#endif //_BACKEND_H_