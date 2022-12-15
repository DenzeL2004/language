#ifndef _FRONTEND_H_
#define _FRONTEND_H_

#include "../config/AST_tree/AST_tree.h"
#include "../config/AST_tree/AST_draw_tree.h"

#include "../src/array/array.h"

static const char* Extension = ".dog";

enum Backend_func_err
{
    
};

struct Frontend_struct 
{
    Array_struct tokens = {};
    int cnt_tokens = 0;

    Tree ast_tree = {};
};

#endif //_FRONTEND_H_