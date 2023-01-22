#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "../AST_tree/AST_tree.h"
#include "../AST_tree/AST_draw_tree.h"

#include "../config/language_config.h"
#include "../config/language_dsl.h"

enum Convert_func_err
{
    UNKNOWN_TYPE_ERR   = -1,
};

int Create_convert_file (const Tree *ast_tree, const char* name_output_file = Default_convert_output);

//#define LOOK_CONVERT_AST_TREE

#endif //_CONVERT_H_