#ifndef _NAME_TABLE_H_
#undef  _NAME_TABLE_H_

#include "stdint.h"

static const int Init_number_objects = 20;

static const int Additional_objects  = 20;

static const int Not_init_object     = -1;

static const int     Init_object     = -2;


enum Type_object
{
    OBJ_UNKNOWN_T   = -1, 
    
    OBJ_VAR         =  0,
    OBJ_FUNC        =  1,
    OBJ_PARAM       =  2,
};



enum Name_tabel_func_err
{
    NAME_TABLE_CTOR_ERR = -1,
    NAME_TABLE_DTOR_ERR = -2,

    RECALLOC_MEMORY_ERR = -4,

    ADD_OBJECT_ERR      = -5,

    DELETE_OBJECT_ERR   = -6,
};


struct Object
{
    const char *name = nullptr;
    int64_t name_hash = 0;

    int type = OBJ_UNKNOWN_T;

    void *data = nullptr; 
};

struct Name_table
{
    int cnt_object = 0;
    int capacity = 0;
    
    Object* objects = nullptr;
};


int Name_table_ctor (Name_table *name_table);

int Name_table_dtor (Name_table *name_table);


int Add_object (Name_table *name_table, const char *name_new_object, const int type);


int Find_id_object (const Name_table *name_table, const char *name_new_object);

int Del_object (Name_table *name_table, const int id);

#endif //#endif _NAME_TABLE_H_