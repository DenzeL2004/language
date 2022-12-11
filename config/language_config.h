#ifndef _LANGUAGE_CONFIG_H_
#define _LANGUAGE_CONFIG_H_

#include "language_dsl.h"

//================================================================================================

#define DEF_NODE_TYPE(name, num, ...)    \         
    name = (num),

enum Node_type
{
    #include "node_types.h"
};

#undef DEF_NODE_TYPE


#define DEF_NODE_TYPE(name, num, ...)    \         
    #name,

static const char* Name_type_node[] =           //Type node names in ast format
{
    #include "node_types.h"
};

#undef DEF_NODE_TYPE


#define DEF_NODE_TYPE(name, num, lang_name)      \         
    lang_name,

static const char* Name_lang_type_node[] =     //Type node names in my language
{
    #include "node_types.h"
};

#undef DEF_NODE_TYPE

//================================================================================================

#define DEF_NODE_OP(name, num, ...)      \         
    name = (num),

enum Operations
{
    #include "operations.h"
};

#undef DEF_NODE_OP


#define DEF_NODE_OP(name, num, ...)      \         
    #name,

static const char* Name_operations[] =          //Operation names in ast format
{
    #include "operations.h"
};

#undef DEF_NODE_OP


#define DEF_NODE_OP(name, num, lang_name)      \         
    lang_name,

static const char* Name_lang_operations[] =     //Operation names in my language
{
    #include "operations.h"
};

#undef DEF_NODE_OP
//================================================================================================

#endif  //_LANGUAGE_CONFIG_H_ 