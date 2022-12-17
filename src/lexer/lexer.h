#ifndef _LEXER_H_
#define _LEXER_H_

#include "../array/array.h"

enum Lexer_func_err
{
    ANALYS_STR_ERR  = -1,
    CUT_STR_ERR     = -3,
};

int Lexical_analysis_str (const char *buffer, Array_struct *tokens);

#endif  //_LEXER_H_