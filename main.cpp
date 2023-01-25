#include <stdio.h>
#include <string>

#include "src/Generals_func/generals.h"
#include "src/log_info/log_errors.h"


int main (int argc, char *argv[])
{   
    
    #ifdef USE_LOG
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;
    #endif

    switch (argc)
    {
        case 1:
            return PROCESS_ERROR (EXIT_FAILURE, "No file to read.\n"
                                                "Specify the file from which you want to read\n");
        case 2: 
            break;

        default:
            return PROCESS_ERROR (EXIT_FAILURE, "Too many command line arguments\n");
    }

    char command[Init_buffer] = "";

    sprintf (command, "./front resource/%s.dog temp/ast_format/ast_%s.ast", argv[1], argv[1]);
    if (system (command)) return PROCESS_ERROR (EXIT_FAILURE, "frontend error\n");
    
    sprintf (command, "./back temp/ast_format/ast_%s.ast temp/asm_format/asm_%s.txt", argv[1], argv[1]);
    if (system (command)) return PROCESS_ERROR (EXIT_FAILURE, "backend error\n");
    
    sprintf (command, "./assembler_run temp/asm_format/asm_%s.txt temp/bin/%s.bin", argv[1], argv[1]);
    if (system (command)) return PROCESS_ERROR (EXIT_FAILURE, "assembler error\n");

    sprintf (command, "./proc_run temp/bin/%s.bin", argv[1]);
    if (system (command)) return PROCESS_ERROR (EXIT_FAILURE, "processor error\n");


    #ifdef USE_LOG
        if (Close_logs_file ())
            return OPEN_FILE_LOG_ERR;
    #endif

    return EXIT_SUCCESS;
}