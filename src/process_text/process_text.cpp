#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h> 
#include <sys/stat.h>
#include <io.h>

#include "../Generals_func/generals.h"
#include "../log_info/log_errors.h"

#include "process_text.h"

static int _Read_file_to_buffer (int fdin, Text_info *text);

static int _Create_buffer       (int fdin, Text_info *text);

static int _Work_with_bufer     (int fdin, Text_info *text);

static long _Get_file_size      (int fdin);

static int _Read_to_buffer      (int fdin, char *buf, long text_size);


//======================================================================================

int Text_read (int fdin, Text_info *text)
{
    assert (fdin >= 0 && "fdin is negative number");
    assert (text != nullptr && "struct Text_info is nullptr");
    
    if (_Work_with_bufer (fdin, text)){
        Log_report ("Buffer initialization error\n");
        return ERR_INIT_BUF;
    }

    return 0;
}

//======================================================================================

static int _Work_with_bufer (int fdin, Text_info *text)
{
    assert (fdin >= 0 && "fdin is negative number");
    assert (text != nullptr && "struct Text_info is nullptr");
    
    if (_Create_buffer (fdin, text)){
        Log_report ("Buffer initialization error\n");
        return ERR_INIT_BUF;
    }

    if (_Read_file_to_buffer (fdin, text)){
        Log_report ("Buffer initialization error\n");
        return ERR_INIT_BUF;
    }

    return 0;
}

//======================================================================================

static int _Read_file_to_buffer (int fdin, Text_info *text)
{
    assert (fdin >= 0 && "fdin is negative number");
    assert (text != nullptr && "struct Text_info is nullptr");

    int real_read_char = _Read_to_buffer (fdin, text->text_buf, text->text_size);    

    if(real_read_char == -1){
        Log_report ("File read error\n");
        return ERR_FILE_READING;
    }

    if (*(text->text_buf + real_read_char - 1) != '\0')                               
        *(text->text_buf + real_read_char) = '\0';   


    return 0;
}

//======================================================================================

static int _Create_buffer (int fdin, Text_info *text)
{
    assert (fdin >= 0 && "fdin is negative number");

    text->text_size = _Get_file_size (fdin) + 1;
    if (text->text_size == 1L){
        Log_report ("File read error");     
        return ERR_FILE_READING;
    }

    text->text_buf = (char*) calloc (text->text_size, sizeof(char));
    if (text->text_buf == nullptr){
        Log_report ("Buffer initialization error\n");
        return ERR_INIT_BUF;
    } 

    return 0;
} 

//======================================================================================

static long _Get_file_size (int fdin)
{
    assert (fdin >= 0 && "fdin is negative number");

    struct stat file_info = {};
    fstat (fdin, &file_info);

    return file_info.st_size;
}

//======================================================================================

static int _Read_to_buffer (int fdin, char *buf, long text_size)
{
    assert (fdin >= 0 && "fdin is negative number");
    assert (buf  != nullptr && "Buffer is nullptr");
    
    return read (fdin, buf, text_size);
}

//======================================================================================

int Free_buffer (Text_info *text)
{
    assert (text != nullptr && "Text_info text is nullptr");

    if (text->text_buf == nullptr){
        Log_report ("buffer memory clear error\n");
        return ERR_RE_ClEARING_MEMORY;
    }

    free (text->text_buf);
    text->text_buf = nullptr;

    return 0;
}

//======================================================================================

int Text_write (FILE *fpout, int cnt_lines, Line *lines)
{
    assert (fpout != nullptr && "file fpout is nullptr");

    for (int cur_line = 0; cur_line < cnt_lines; cur_line++){
        fprintf (fpout, "%s\n", (lines+cur_line)->str);
    }

    return 0;
}

//======================================================================================