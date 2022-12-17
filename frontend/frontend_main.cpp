#include <stdio.h>
#include <string>

#include "frontend.h"

#include "../src/log_info/log_errors.h"

int main (int argc, char *argv[])
{   
    
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif

    Frontend_struct source = {};

    if (Frontend_struct_ctor (&source))
        return PROCESS_ERROR (FRONTEND_CTOR_ERR, "Ctor tree error\n");

    char *name_output_file = (char*) Default_frontend_output;
    switch (argc)
    {
        case 1:
            return PROCESS_ERROR (EXIT_FAILURE, "No file to read.\n"
                                                "Specify the file from which you want to read\n");
        case 2:
            if (Read_source_file (&source, argv[1]))
                return PROCESS_ERROR (EXIT_FAILURE, "Read file in ast_tree fail\n");
            break;
        
        case 3:
            if (Read_source_file (&source, argv[1]))
                return PROCESS_ERROR (EXIT_FAILURE, "Read file in ast_tree fail\n");
            name_output_file = argv[2];
            break;

        default:
            return PROCESS_ERROR (EXIT_FAILURE, "Too many command line arguments\n");
    }

    Write_database (&source.ast_tree);

    if (Frontend_struct_dtor (&source))
        return PROCESS_ERROR (FRONTEND_DTOR_ERR, "Ctor tree error\n");

    
    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif

    return EXIT_SUCCESS;
}