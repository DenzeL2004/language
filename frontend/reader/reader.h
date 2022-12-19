#ifndef _READER_H_
#define _READER_H_

#include "../../config/language_config.h"

#include "../../AST_tree/AST_tree.h"

#include "../../src/array/array.h"


enum Reader_func_err
{
    PARCE_ERR       = -1,
    SYNTAX_ERR      = -2,

    STR_CONVERT_ERR = -3, 
};


int Parce_source_file (Tree *ast_tree, const Array_struct *tokens, const int cnt_tokens);


#endif  //_READER_H_