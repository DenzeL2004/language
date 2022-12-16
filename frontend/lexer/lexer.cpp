#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"

#include "../../src/log_info/log_errors.h"
#include "../../src/Generals_func/generals.h"



static char* Cut_word           (const char **buffer);

static char* Cut_num            (const char **buffer);

static char* Cut_symbol         (const char **buffer);

static void  Skip_space_symbols (const char **buffer);


//=================================================================================================

int Lexical_analysis_str (const char *buffer, Array_struct *tokens)
{
    assert (buffer != nullptr && "buffer is nullptr");
    assert (tokens != nullptr && "tokens is nullptr");

    int id = 0;

    while (*buffer != '\0')
    {
        if (isspace (*buffer))
            Skip_space_symbols (&buffer);
        
        if (*buffer == '\0') break;

        if (id == tokens->capacity)
            Array_recalloc (tokens, id * 2);
        
        if (isalpha (*buffer))
            Array_set_ptr_by_ind (tokens, id, Cut_word (&buffer));

        else if (isdigit (*buffer))
            Array_set_ptr_by_ind (tokens, id, Cut_num (&buffer));

        else 
            Array_set_ptr_by_ind (tokens, id, Cut_symbol (&buffer));
        
        if (Check_nullptr (tokens->data[id]))
            return PROCESS_ERROR (ANALYS_STR_ERR, "token's id = %d, buffer = |%s|\n", id, *buffer);

        id++;    
    }

    return id;
} 

//=================================================================================================

static char* Cut_word (const char **buffer)
{
    assert (buffer != nullptr && "buffer is nullptr");

    int len_word = 0;

    while (isalnum (**buffer) || **buffer == '_')
    {
        len_word++;
        (*buffer)++;
    }
    
    char *cut_str = My_strndup (*buffer - len_word, len_word);
    
    if (Check_nullptr (cut_str))
    {
        PROCESS_ERROR (CUT_STR_ERR, "Failed to cut the word. Failed place: %10s\n", buffer - len_word);
        return nullptr;
    }

    return cut_str;
}

//=================================================================================================

static char* Cut_num (const char **buffer)
{
    assert (buffer != nullptr && "buffer is nullptr");

    int len_num = 0;
    
    while (isdigit (**buffer) || **buffer == '.')
    {
        len_num++;
        (*buffer)++;
    }

    char *cut_str = My_strndup (*buffer - len_num, len_num);
    
    if (Check_nullptr (cut_str))
    {
        PROCESS_ERROR (CUT_STR_ERR, "Failed to cut the word. Failed place: %10s\n", *buffer - len_num);
        return nullptr;
    }

    return cut_str;
} 

//=================================================================================================

static char* Cut_symbol (const char **buffer)
{
    assert (buffer != nullptr && "buffer is nullptr");

    char *cut_str = My_strndup (*buffer, 1);
    
    if (Check_nullptr (cut_str))
    {
        PROCESS_ERROR (CUT_STR_ERR, "Failed to cut the word. Failed place\n: %10s", *buffer);
        return nullptr;
    }

    (*buffer)++;

    return cut_str;
} 

//=================================================================================================

static void Skip_space_symbols (const char **buffer)
{
    assert (buffer != nullptr && "buffer is nullptr");

    while (isspace (**buffer))
        (*buffer)++;

    return;  
} 

//=================================================================================================

