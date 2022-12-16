#include <stdio.h>
#include <string>

#include "backend.h"

#include "../src/log_info/log_errors.h"

int main (int argc, char *argv[])
{   
    
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif

    Tree ast_tree = {};
    if (Tree_ctor (&ast_tree))

    switch (argc)
    {
        case 1:
            return PROCESS_ERROR (EXIT_FAILURE, "No file to read.\n"
                                                "Specify the file from which you want to read\n");
        case 2:
        {
            if (Read_source_file (&ast_tree, argv[1]))
                return PROCESS_ERROR (EXIT_FAILURE, "Read file in ast_tree fail\n");
            break;
        }

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