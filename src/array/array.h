#ifndef _STACK_H_
#define _STACK_H_

#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>


#include "../log_info/log_errors.h"

static const int Init_array_capacity = 10;


struct Array_struct
{

    int capacity = 0;

    void **data = nullptr;
};

enum Array_func_err
{
    ARRAY_CTOR_ERR      = -1,
    ARRAY_DTOR_ERR      = -2,

    ARRAY_GET_ERR       = -3,
    ARRAY_SET_ERR       = -4,

    ARRAY_RECALLOC_ERR  = -5,

    ARRAY_GET_CAPACITY  = -6, 
};

enum Array_err
{
    BAD_DATA_PTR         = 1,

    CAPACITY_LOWER_ZERO  = 4,
};


int Array_ctor (Array_struct *array, int capacity);

int Array_dtor (Array_struct *array);


void* Array_get_ptr_by_ind (const Array_struct *array, const int ind);

int Array_set_ptr_by_ind   (Array_struct *array, const int ind, void *ptr);


int Array_recalloc (Array_struct *array, int new_capacity);

int Array_get_capacity (const Array_struct *array);


int Array_dump (const Array_struct *array,
                 const char* file_name, const char* func_name, int line);

#endif