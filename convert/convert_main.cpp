#include <stdio.h>
#include <string>

#include "convert.h"

#include "../src/log_info/log_errors.h"
#include "../AST_tree/ast_reader/ast_reader.h"

int main (int argc, char *argv[])
{   
    
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif

    Tree ast_tree = {};
    if (Tree_ctor (&ast_tree))
        PROCESS_ERROR (EXIT_FAILURE, "Tree ctor failed\n");

    char *name_output_file = (char*) Default_convert_output;
    switch (argc)
    {
        case 1:
            return PROCESS_ERROR (EXIT_FAILURE, "No file to read.\n"
                                                "Specify the file from which you want to read\n");
        case 2:
            if (Load_ast_tree (&ast_tree, argv[1]))
                return PROCESS_ERROR (EXIT_FAILURE, "Read file in ast_tree fail\n");
            break;

        case 3:
            if (Load_ast_tree (&ast_tree, argv[1]))
                return PROCESS_ERROR (EXIT_FAILURE, "Read file in ast_tree fail\n");
            name_output_file = argv[2];
            break;

        default:
            return PROCESS_ERROR (EXIT_FAILURE, "Too many command line arguments\n");
    }

    Create_convert_file (&ast_tree, name_output_file);

    if (Tree_dtor (&ast_tree))
        PROCESS_ERROR (EXIT_FAILURE, "Tree dtor failed\n");

    
    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif

    return EXIT_SUCCESS;
}