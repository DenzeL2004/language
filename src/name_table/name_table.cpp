#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "name_table.h"
#include "../Generals_func/generals.h"
#include "../log_info/log_errors.h"


static int Object_init (Object *object, const char* name, const int type);

static inline int Check_cnt_object (Name_table *name_table);

static int Recalloc_name_table (Name_table *name_table);

//======================================================================================

int Name_table_ctor (Name_table *name_table)
{
    assert (name_table != nullptr && "str is nullptr");

    name_table->cnt_object      = 0;
    name_table->capacity        = Init_number_objects;
    name_table->objects = (Object*) calloc (Init_number_objects, sizeof(Object));

    if (Check_nullptr (name_table->objects))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Memory was not allocated for the array of objects\n");
    

    return 0;
}

//======================================================================================

int Name_table_dtor (Name_table *name_table)
{
    assert (name_table != nullptr && "str is nullptr");

    for (int id = 0; id < name_table->cnt_object; id++)
        free (name_table->objects[id].data);

    name_table->cnt_object  = -1;
    name_table->capacity    = -1;

    if (Check_nullptr (name_table->objects))
        return PROCESS_ERROR (NAME_TABLE_DTOR_ERR, "An error occurred in Name_table deconstruction\n");
    else
        free (name_table->objects);

    return 0;
}

//======================================================================================

int Add_object (Name_table *name_table, const char *name_new_object, const int type) 
{
    assert (name_table != nullptr && "name_table is nullptr");
    assert (name_new_object != nullptr && "name_new_object is nullptr");

    if (Find_id_object (name_table, name_new_object) != Not_init_object)
        return Init_object;

    if (Check_cnt_object (name_table))
    {
        if (Recalloc_name_table (name_table))
            return PROCESS_ERROR (ADD_OBJECT_ERR, "Memory reallocation error");
    }

    int id = name_table->cnt_object;

    name_table->cnt_object++;

    Object_init (name_table->objects + id, name_new_object, type);

    return id;
}

//======================================================================================

int Del_object (Name_table *name_table, const int id) 
{
    assert (name_table != nullptr && "name_table is nullptr");
    
    if (name_table->cnt_object <= id)
        return PROCESS_ERROR (DELETE_OBJECT_ERR, "id: %d >=  cnt_ object: %d", id, name_table->cnt_object);

    free(name_table->objects[id].data);
    name_table->objects[id].name = nullptr;
        
    name_table->objects[id].name_hash = 0;
    name_table->objects[id].type = OBJ_UNKNOWN_T;

    name_table->cnt_object--;

    return id;
}

//======================================================================================

static int Object_init (Object *object, const char* name, const int type) 
{
    assert (object != nullptr && "object is nullptr");

    object->name      = name;
    object->name_hash = Get_str_hash (name);

    object->type      = type;
    
    return 0;
}


//======================================================================================

static inline int Check_cnt_object (Name_table *name_table)
{
    return name_table->cnt_object == name_table->capacity;
}

//======================================================================================

static int Recalloc_name_table (Name_table *name_table)
{
    assert (name_table != nullptr && "name_table is nullptr");
    
    name_table->capacity += Additional_objects;

    realloc (name_table->objects, name_table->capacity * sizeof (Object));

    if (Check_nullptr (name_table->objects)) 
        return PROCESS_ERROR (RECALLOC_MEMORY_ERR, "Error allocating memory for objects\n");
    
    return 0;
}

//======================================================================================

int Find_id_object (const Name_table *name_table, const char *name_object) 
{
    assert (name_table  != nullptr && "name_table is nullptr");
    assert (name_object != nullptr && "name_new_object is nullptr");

    int64_t new_string_hash = Get_str_hash (name_object);

    for (int id = 0; id < name_table->cnt_object; id++) 
    {   
        if ((name_table->objects + id)->name_hash == new_string_hash) 
        {
            return id;
        }
    }

    return Not_init_object;
}

//======================================================================================
