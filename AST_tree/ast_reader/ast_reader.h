#ifndef _AST_READER_
#define _AST_READER_

#include "../AST_tree.h"
#include "../AST_draw_tree.h"

#include "../../config/language_config.h"
#include "../../config/language_dsl.h"

enum AST_reader_func_err
{
    READ_AST_FORMAT     = -1,
    READ_NODE_ERR       = -2,
};


int Load_ast_tree (Tree *ast_tree, const char *name_input_file);

//#define LOOK_READ_AST_TREE

#endif //_AST_READER_