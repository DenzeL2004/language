#ifndef _LANGUAGE_CONFIG_H_
#define _LANGUAGE_CONFIG_H_

#include "language_dsl.h"

static const char* Default_frontend_output = "ast_tree.txt";

static const char* Default_backend_output = "asm_output.txt";

static const char* Default_convert_output = "unknown.dog";


//================================================================================================


enum Node_type
{
    UNKNOWN_T = 0,

    DEFS      =  1,
    NFUN      =  2,
    NVAR      =  3,

    BLOCK     =  4, 

    SEQ       =  5,


    IF        =  6,
    BRANCH    =  7,

    WHILE     =  8,

    FUNC      =  9,
    ARG       = 10, 
    CALL      = 11, 
    PAR       = 12,     
    RET       = 13,

    VAR       = 14,
    CONST     = 15,
    OP        = 16,

    ASS       = 17,

    ELSE      = 18,

};

static const char* Name_type_node[] =           //Type node names in ast format
{
    "UNKNOWN_T", 

    "DEFS", "NFUN", "NVAR",      

    "BLOCK", 

    "SEQ",

    "IF", "BRANCH",

    "WHILE",

    "FUNC", "ARG", 
    "CALL", "PAR", "RET",

    "VAR", "CONST", "OP",

    "ASS",

    "ELSE"
};

static const char* Name_lang_type_node[] =     //Type node names in my language
{
    "UNKNOWN_T", 

    "DEFS", "learnCommand", "breed",      

    "BLOCK", 

    "SEQ",

    "if", "BRANCH",

    "goodBoy",

    "FUNC", "ARG", 
    "CALL", "PAR", "bring",

    "VAR", "CONST", "OP",

    "nickname",

    "else"
};


//================================================================================================

enum Operations
{
    UNKNOWN_OP  =  0,

    ADD         =  1,
    SUB         =  2,
    MUL         =  3,
    DIV         =  4,   

    AND         =  5,
    OR          =  6,
    NOT         =  7,

    DF          =  8,

    GT          =  9,
    NLT         = 10,
    LT          = 11,
    NGT         = 12,
    EQ          = 13,
    NEQ         = 14,

    NEG         = 15,
};

static const char* Name_operations[] =          //Operation names in ast format
{
    "UNKNOWN_OP",

    "ADD", "SUB", "MUL", "DIV",          

    "AND", "OR", "NOT",         

    "DF",          

    "GT", "NLT", 
    "LT", "NGT",     
    "EQ", "NEQ",

    "NEG"         
};

static const char* Name_lang_operations[] =     //Operation names in language
{         
    "UNKNOWN_OP",

    "lick", "bite", "scratch", "tear",          

    "and", "or", "not",         

    "\'",          

    "more", "nless", 
    "less", "nmore",     
    "equal", "nequal",

    "bite"        
};


//================================================================================================

static const char *Name_standard_func[] =
{
    "sqrt", "abs",

    "read", "print",

    "set_pixel", "flush"
};

static const int Cnt_param_standart_func[] =
{
    1, 1, 
    0, 1, 
    3, 0
};

//================================================================================================

#endif  //_LANGUAGE_CONFIG_H_ 