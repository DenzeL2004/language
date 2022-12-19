#ifndef _FRONTEND_H_
#define _FRONTEND_H_

#include "../AST_tree/AST_tree.h"
#include "../AST_tree/AST_draw_tree.h"

#include "../src/array/array.h"

static const char* Extension = ".dog";

enum Frontend_func_err
{
    INVALID_EXTENSION = -1,

    FRONTEND_CTOR_ERR = -2,
    FRONTEND_DTOR_ERR = -3,
};

struct Frontend_struct 
{
    Array_struct tokens = {};
    int cnt_tokens = 0;

    Tree ast_tree = {};
};

int Frontend_struct_ctor (Frontend_struct *frontend_struct);

int Frontend_struct_dtor (Frontend_struct *frontend_struct);

int Read_source_file (Frontend_struct *frontend_struct, const char *name_input_file);

int Write_database (const Tree *ast_tree, const char* name_output_file = Default_frontend_output);

//#define LOOK_FRONTEND_AST_TREE_ 

#endif //_FRONTEND_H_