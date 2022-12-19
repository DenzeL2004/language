#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>


#include "array.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"

static uint64_t Array_verifier (const Array_struct *array);

int Array_ctor (Array_struct *array, int capacity)
{
    assert (array != nullptr && "array is nullptr");

    if (capacity <= 0)
        return PROCESS_ERROR (ARRAY_CTOR_ERR, "trying to create an array of negative capacity\n");

    array->capacity  = capacity; 

    array->data = (void**) calloc (capacity, sizeof (void*));
    if (Check_nullptr(array->data))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Alloc memory\n");

    return 0;
}

//=======================================================================================================

int Array_dtor (Array_struct *array)
{
    assert (array != nullptr && "array is nullptr");

    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return ARRAY_DTOR_ERR;
    }

    array->capacity = 0;

    free (array->data);

    return 0;
}

//=======================================================================================================

void* Array_get_ptr_by_ind (const Array_struct *array, const int ind)
{
    assert (array != nullptr && "array is nullptr");
    
    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return nullptr;
    }

    if (ind >= array->capacity || ind < 0)
    {
        PROCESS_ERROR (ARRAY_GET_ERR, "invalid index: %d", ind);
        return nullptr;
    }

    return array->data[ind];
}

//=======================================================================================================

int Array_set_ptr_by_ind (Array_struct *array, const int ind, void *ptr)
{
    assert (array != nullptr && "array is nullptr");
    assert (ptr   != nullptr && "ptr is nullptr");

    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return ARRAY_SET_ERR;
    }
    
    if (ind >= array->capacity)
        Array_recalloc (array, array->capacity * 2);
        
    if (ind < 0)
        return PROCESS_ERROR (ARRAY_SET_ERR, "invalid index: %d, capacity = %d\n", ind, array->capacity);
         
    array->data[ind] = ptr;

    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return ARRAY_SET_ERR;
    }

    return 0;
}

//=======================================================================================================

int Array_recalloc (Array_struct *array, int new_capacity)
{
    assert (array != nullptr && "array is nullptr");

    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return ARRAY_SET_ERR;
    }

    array->data = (void**) realloc (array->data, new_capacity * sizeof (void*));
    if (Check_nullptr(array->data))
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Alloc memory\n");

    for (int id = array->capacity; id < new_capacity; id++)
       array->data[id] = nullptr;

    array->capacity = new_capacity;
    
    return 0;
}

//=======================================================================================================

int Array_get_capacity (const Array_struct *array)
{
    assert (array != nullptr && "array is nullptr");

    if (Array_verifier (array))
    {
        Array_dump (array, LOG_ARGS);
        return ARRAY_GET_CAPACITY;
    }

    return array->capacity;
}

//=======================================================================================================

int Array_dump (const Array_struct *array,
                 const char* file_name, const char* func_name, int line)
{
    assert (array != nullptr && "array is nullptr");

    uint64_t err_code = Array_verifier (array);

    FILE *fp_logs = Get_log_file_ptr ();

    fprintf (fp_logs, "ARRAY ERROR\nCaused an error in file %s, function %s, line %d\n\n", LOG_VAR);

    fprintf (fp_logs, "=================================================\n\n");

    
    fprintf (fp_logs, "array pointer to data is |%p|\n\n", (char*) array->data);

    fprintf (fp_logs, "array capacity  = %d\n", array->capacity);


    fprintf (fp_logs, "\n");

    if (err_code & BAD_DATA_PTR) fprintf (fp_logs, "array pointer data is BAD.\n");

    if (err_code & CAPACITY_LOWER_ZERO)  fprintf (fp_logs, "array capacity is a negative number.\n");

    fprintf (fp_logs, "=================================================\n\n");

    return 0;
}

//=======================================================================================================

static uint64_t Array_verifier (const Array_struct *array)
{
    assert (array != nullptr && "array is nullptr");   

    uint64_t err_code = 0;

    if (Check_nullptr ((char*) array)) err_code |= BAD_DATA_PTR;

    if (array->capacity  < 0) err_code |= CAPACITY_LOWER_ZERO;

    return err_code;    
}

//=======================================================================================================