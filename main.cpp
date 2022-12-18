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
    sprintf (command, "frontend.exe source/%s.dog ast_format/ast_%s.txt", argv[1], argv[1]);

    if (system (command))
        return PROCESS_ERROR (EXIT_FAILURE, "frontend error\n");

   
    sprintf (command, "backend.exe ast_format/ast_%s.txt asm_format/asm_%s.txt", argv[1], argv[1]);

    if (system (command))
        return PROCESS_ERROR (EXIT_FAILURE, "backend error\n");

    return EXIT_SUCCESS;
}