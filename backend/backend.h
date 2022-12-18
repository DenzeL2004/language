#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "../config/AST_tree/AST_tree.h"
#include "../config/AST_tree/AST_draw_tree.h"

#include "../config/language_config.h"
#include "../config/language_dsl.h"

#include "../src/name_table/name_table.h"

enum Backend_func_err
{
    READ_AST_FORMAT     = -1,

    READ_NODE_ERR       = -2,

    INCORRECT_CNT_ARG   = -3,   

    INVALID_CALL        = -4,
    COMPILATION_ERR     = -5,

    NAMESPACE_CTOR_ERR  = -6,
    NAMESPACE_DTOR_ERR  = -7,

    UNINIT_VAR_ERR      = -8,

    REDEFINITION_ERR    = -9,    
};

struct Namespace_struct
{
    int free_cell = 0;

    Name_table name_table = {};
};

int Namespace_struct_ctor (Namespace_struct *namespace_struct);

int Namespace_struct_dtor (Namespace_struct *namespace_struct);

Namespace_struct* Dup_namespace (const Namespace_struct *namespace_struct);


int Load_ast_tree (Tree *ast_tree, const char *name_input_file);

int Create_asm_file (const Tree *ast_tree, const char* name_output_file = Default_backend_output);

//#define LOOK_BACKEND_AST_TREE

#endif //_BACKEND_H_