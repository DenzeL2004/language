#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h> 

#include "backend.h"

#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"


#include "../src/array/array.h"

#include "../src/process_text/process_text.h"
#include "../src/lexer/lexer.h"


static Node* Build_tree (int *pos, const Array_struct *tokens);

static Node* Read_node_from_buffer (int *pos, const Array_struct *tokens);

static int Define_operation (const char *token);

static int Define_type (const char *token);



static int Namespace_check     (const Tree *ast_tree);

static int Check_function_call (const Node *node, Name_table *function_names);



static int Write_begin_program (FILE *fpout);



static int Compile          (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_defs     (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_nvar     (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_nfunc    (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_call     (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_ret      (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_oper     (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_const    (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_var      (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_assig    (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_seq      (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_block    (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_par      (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_arg      (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_if       (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_branch   (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);

static int Compile_while    (FILE *fpout, const Node *node, Namespace_struct *cur_namespace);


//======================================================================================================

int Namespace_struct_ctor (Namespace_struct *namespace_struct) 
{
    assert (namespace_struct != nullptr && "frontend struct is nullptr");

    if (Name_table_ctor (&namespace_struct->name_table))
        return PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Ctor Name_table \'namespace_struct->name_table\' error\n");
    
    namespace_struct->free_cell = 0;

    return 0;
}

//======================================================================================================

int Namespace_struct_dtor (Namespace_struct *namespace_struct) 
{
    assert (namespace_struct != nullptr && "frontend struct is nullptr");

    namespace_struct->free_cell = 0;

    if (Name_table_dtor (&namespace_struct->name_table))
        return PROCESS_ERROR (NAME_TABLE_DTOR_ERR, "Dtor \'namespace_struct->name_table\' error\n");

    return 0;
}

//======================================================================================================

Namespace_struct* Dup_namespace (const Namespace_struct *namespace_struct)
{
    assert (namespace_struct != nullptr && "frontend struct is nullptr");

    Namespace_struct *dup_namespace = (Namespace_struct*) calloc (1, sizeof (Namespace_struct));

    if (Name_table_ctor (&dup_namespace->name_table))
    {
        PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Ctor Name_table \'dup_namespace->name_table\' error\n");
        return nullptr;
    }

    for (int id = 0; id < namespace_struct->name_table.cnt_object; id++)
    {
        const char *name   = namespace_struct->name_table.objects[id].name;
        const int type_obj = namespace_struct->name_table.objects[id].type;
        int add_id = Add_object (&dup_namespace->name_table, name, type_obj);

        dup_namespace->name_table.objects[add_id].data = calloc (1, sizeof (int)); 
    
        int val = *((int*) namespace_struct->name_table.objects[add_id].data);
        *((int*) dup_namespace->name_table.objects[add_id].data) = val;
    }

    dup_namespace->free_cell = namespace_struct->free_cell;

    return dup_namespace;
}

//======================================================================================================

int Create_asm_file (const Tree *ast_tree, const char* name_output_file)
{
    assert (ast_tree !=  nullptr && "ast_tree is nullptr");

    if (Namespace_check (ast_tree))
        return PROCESS_ERROR (COMPILATION_ERR, "\n");

    FILE* fpout = Open_file_ptr (name_output_file, "w");
    if (Check_nullptr (fpout))
        return PROCESS_ERROR (ERR_FILE_OPEN, "open output file \'%s\' error", name_output_file);

    Write_begin_program (fpout);
    
    Namespace_struct cur_namespace = {};
    if (Namespace_struct_ctor (&cur_namespace))
        return PROCESS_ERROR (NAMESPACE_CTOR_ERR, "Ctor Name_table \'cur_namespace\' error\n");
    
    Compile (fpout, ast_tree->root, &cur_namespace);

    if (Namespace_struct_dtor (&cur_namespace))
        return PROCESS_ERROR (NAMESPACE_DTOR_ERR, "Ctor Name_table \'cur_namespace\' error\n");

    if (Close_file_ptr (fpout))
        return PROCESS_ERROR (ERR_FILE_OPEN, "open output file \'%s\' error", name_output_file);

    return 0;
}

//======================================================================================================

static int Namespace_check (const Tree *ast_tree)
{
    assert (ast_tree != nullptr && "ast_tree is nullptr");

    Name_table function_names = {};

    if (Name_table_ctor (&function_names))
        return PROCESS_ERROR (NAME_TABLE_CTOR_ERR, "Ctor Name_table \'function_names\' error\n");

    int cnt_words = sizeof (Name_standard_func)/ sizeof (Name_standard_func [0]);
    for (int id = 0; id < cnt_words; id++)
    {
        int id_func = Add_object (&function_names, Name_standard_func [id], OBJ_FUNC);
        function_names.objects[id].data = calloc (1, sizeof (int)); 
    
        *((int*) function_names.objects[id].data) = Cnt_param_standart_func [id];
    }

    int check_res = Check_function_call (ast_tree->root, &function_names);
    if (check_res != 0)
        PROCESS_ERROR (INVALID_CALL, "invalid function call\n");

    
    if (Name_table_dtor (&function_names))
        return PROCESS_ERROR (NAME_TABLE_DTOR_ERR, "Dtor \'function_names\' error\n");   

    return 0;
}

//======================================================================================================

static int Check_function_call (const Node *node, Name_table *function_names)
{
    assert (node != nullptr && "node is nullptr");
    assert (function_names != nullptr && "function is nullptr");

    int cnt = 0;

    if (!Check_nullptr (node->left))
        cnt += Check_function_call (node->left, function_names);

    if (!Check_nullptr (node->right))
        cnt += Check_function_call (node->right, function_names);
    
    if (GET_TYPE (node) == ARG || GET_TYPE (node) == PAR) cnt++;

    if (GET_TYPE (node) == NFUNC)
    {
        int id = Find_id_object (function_names, GET_DATA (node, obj));
        if (id != Not_init_object)
            PROCESS_ERROR(REDEFINITION_ERR, "redefinition function \'%s\'\n", GET_DATA (node, obj));

        id = Add_object (function_names, GET_DATA (node, obj), OBJ_FUNC);
        function_names->objects[id].data = calloc (1, sizeof (int)); 

        *((int*) function_names->objects[id].data) = cnt;
        cnt = 0;
    }

    if (GET_TYPE (node) == CALL)
    {
        int id = Find_id_object (function_names, GET_DATA (node, obj));
        if (id != Not_init_object)
        {
            int cnt_param = *((int*) function_names->objects[id].data);
            if (cnt != cnt_param)
                PROCESS_ERROR (INCORRECT_CNT_ARG, "\nFunction \'%s\' function has the wrong number of arguments: %d. "
                               "Must be: %d\n", GET_DATA (node, obj), cnt, cnt_param);
        }

        cnt = 0;
    }    
    
    return cnt;
}

//======================================================================================================

static int Write_begin_program (FILE *fpout)
{
    assert (fpout != nullptr && "fpout is nullptr");

    fprintf (fpout, "push 0\n");
    fprintf (fpout, "pop rax\n");

    fprintf (fpout, "call main\n");

    fprintf (fpout, "hlt\n\n");

    int fdin = Open_file_discriptor ("../language/config/standart_func.txt", O_RDONLY);
    if (fdin < 0)
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening file\n");

    Text_info text = {};
    
    if (Text_read (fdin, &text))
        return PROCESS_ERROR (ERR_FILE_READING, "Error reading into Text_info structure\n");

    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE,"Error close input file \n");
    
    fprintf (fpout, "%s\n", text.text_buf);

    Free_buffer (&text);

    return 0;
}

//======================================================================================================

static int Compile (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    if (Check_nullptr ((char*) node))
        return 0;
    
    switch ((GET_TYPE (node)))
    {
        case DEFS:  Compile_defs    (fpout, node, cur_namespace);
            break;
        
        case NVAR:  Compile_nvar    (fpout, node, cur_namespace);
            break;

        case NFUNC: Compile_nfunc   (fpout, node, cur_namespace);
            break;
        
        case PAR:   Compile_par   (fpout, node, cur_namespace);
            break;

        case ARG:   Compile_arg   (fpout, node, cur_namespace);
            break;
        
        case CALL:  Compile_call    (fpout, node, cur_namespace);
            break;

        case RET:   Compile_ret     (fpout, node, cur_namespace);
            break;

        case OP:    Compile_oper    (fpout, node, cur_namespace);
            break;
        
        case ASS:   Compile_assig   (fpout, node, cur_namespace);
            break;

        case CONST: Compile_const   (fpout, node, cur_namespace);
            break;

        case VAR:   Compile_var     (fpout, node, cur_namespace);
            break;

        case SEQ:   Compile_seq     (fpout, node, cur_namespace);
            break;

        case BLOCK: Compile_block   (fpout, node, cur_namespace);
            break;

        case IF:    Compile_if      (fpout, node, cur_namespace);
            break;

        case BRANCH: Compile_branch (fpout, node, cur_namespace);
            break;

        case WHILE: Compile_while   (fpout, node, cur_namespace);
            break;

        default:    
            break;
    } 

    return 0;
}

#define GET_MEM_CELL(id)                                    \
    *((int*) cur_namespace->name_table.objects[id].data)
    

//======================================================================================================

static int Compile_defs (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    Compile (fpout, node->left, cur_namespace);
    Compile (fpout, node->right, cur_namespace);

    return 0;
}

//======================================================================================================

static int Compile_nvar (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    if (!Check_nullptr (node->right))
    {
        Compile (fpout, node->right, cur_namespace);
        fprintf (fpout, "pop [%d+rax]", cur_namespace->free_cell);
    }  
    
    int id = Find_id_object (&cur_namespace->name_table, GET_DATA (node, obj));
    if (id != Not_init_object)
        return PROCESS_ERROR(REDEFINITION_ERR, "redefinition variable \'%s\'\n", GET_DATA (node, obj));

    id = Add_object (&cur_namespace->name_table, GET_DATA (node, obj), OBJ_VAR);
    cur_namespace->name_table.objects[id].data = calloc (1, sizeof (int));
    
    GET_MEM_CELL(id) = cur_namespace->free_cell; 
    cur_namespace->free_cell++; 

    fprintf (fpout, "   //Definition_variable_\'%s\'\n\n", GET_DATA (node, obj));

    return 0;
}

//======================================================================================================

static int Compile_nfunc (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "\n%s:\n", GET_DATA (node, obj));
    fprintf (fpout, "pop rlx    //save_pointer_to_where_to_return_after_the_function_terminates\n");
    fprintf (fpout, "pop rax    \n");

    Namespace_struct new_namespace = {};
    if (Namespace_struct_ctor (&new_namespace))
        return PROCESS_ERROR (NAMESPACE_CTOR_ERR, "Ctor Name_table \'new_namespace\' error\n");

    Compile (fpout, node->left,  &new_namespace);
    Compile (fpout, node->right, &new_namespace);

    if (Namespace_struct_dtor (&new_namespace))
        return PROCESS_ERROR (NAMESPACE_DTOR_ERR, "Ctor Name_table \'new_namespace\' error\n");

    return 0;
}

//======================================================================================================


static int Compile_par (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "\npop [%d+rax]", cur_namespace->free_cell);

    int id = Find_id_object (&cur_namespace->name_table, GET_DATA (node, obj));
    if (id != Not_init_object)
        return PROCESS_ERROR(REDEFINITION_ERR, "redefinition par \'%s\'\n", GET_DATA (node, obj));

    id = Add_object (&cur_namespace->name_table, GET_DATA (node, obj), OBJ_PARAM);
    cur_namespace->name_table.objects[id].data = calloc (1, sizeof (int));
    
    GET_MEM_CELL(id) = cur_namespace->free_cell; 
    cur_namespace->free_cell++; 

    fprintf (fpout, "   //Definition_param_\'%s\'\n\n", GET_DATA (node, obj));

    return 0;
}

//======================================================================================================

static int Compile_ret (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "//returned_value\n");
    Compile (fpout, node->right, cur_namespace);
    fprintf (fpout, "//returned_value\n\n");

    fprintf (fpout, "push rlx   //Get_pointer_to_where_to_return_after_the_function_terminates\n");
    fprintf (fpout, "ret\n\n");

    return 0;
}

//======================================================================================================

static int Compile_call (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "\n//Begin_call_function\n");

    fprintf (fpout, "push rlx   //Put_on_stack_pointer_to_where_to_return_after_the_function_terminates\n");
    fprintf (fpout, "push rax   //Put_on_stack_pointer_where_the_variables_of_the_current_function_were_located\n");

    fprintf (fpout, "\n//arguments_function_\'%s\':\n", GET_DATA (node, obj));

    Compile (fpout, node->right, cur_namespace);

    fprintf (fpout, "//arguments_function_\'%s\'\n\n", GET_DATA (node, obj));
    
    fprintf (fpout, "push %d\n", cur_namespace->free_cell);

    fprintf (fpout, "call %s\n", GET_DATA (node, obj));
    
    fprintf (fpout, "pop rbx    //Put_on_stack_returned_function_val\n");
    fprintf (fpout, "pop rax    //Save_pointer_where_the_variables_of_the_current_function_were_located\n");
    
    fprintf (fpout, "pop rlx    //Save_pointer_to_where_to_return_after_the_function_terminates\n");
    
    fprintf (fpout, "push rbx   //Put_on_stack_returned_function_val\n");

    fprintf (fpout, "//End_call_function\n\n");

    return 0;
}

//======================================================================================================

static int Compile_arg (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "\n//argument:\n");

    Compile (fpout, node->left, cur_namespace);
    Compile (fpout, node->right, cur_namespace);

    fprintf (fpout, "\n");

    return 0;
}

//======================================================================================================

static int Compile_if (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    static int cnt_if = 0; cnt_if++;
    int cur_if = cnt_if;

    fprintf (fpout, "\n//СONDITION%d_START\n", cur_if);

    Compile (fpout,  node->left,  cur_namespace);
    fprintf (fpout, "push 0\n");

    
    fprintf (fpout, "je if%d_false\n", cur_if);

    Namespace_struct *dup_namespace = Dup_namespace (cur_namespace);
    if (Check_nullptr (dup_namespace)) 
        return PROCESS_ERROR (ERR_MEMORY_ALLOC, "Failed duplicate \'cur_namespace\'\n");
    Compile (fpout,  node->right,  dup_namespace);

    fprintf (fpout, "\n//СONDITION%d_END\n", cur_if);

    if (Namespace_struct_dtor (dup_namespace))
        return PROCESS_ERROR (NAMESPACE_DTOR_ERR, "Dtor \'dup_namespace\' error\n");

    return 0;
}

//======================================================================================================

static int Compile_branch (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    static int cnt_if = 0; cnt_if++;
    int cur_if = cnt_if;

    Compile (fpout,  node->left,  cur_namespace);

    fprintf (fpout, "jump if%d_true_exit\n", cur_if);
    
    fprintf (fpout, "if%d_false:\n", cur_if);

    Compile (fpout,  node->right,  cur_namespace);

    fprintf (fpout, "if%d_true_exit:\n", cur_if);

    return 0;
}

//======================================================================================================

static int Compile_while (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    static int cnt_while = 0; cnt_while++;
    int cur_while = cnt_while;
    fprintf (fpout, "\n//WHILE%d_START\n", cur_while);

    Compile (fpout, node->left, cur_namespace);
    fprintf (fpout, "push 0\n");
    fprintf (fpout, "je skip_while%d\n\n", cur_while);

    fprintf (fpout, "while%d:\n", cur_while);

    Compile (fpout, node->right, cur_namespace);


    Compile (fpout, node->left, cur_namespace);
    fprintf (fpout, "push 1\n");
    fprintf (fpout, "je while%d   //out_condition\n\n", cur_while);


    fprintf (fpout, "skip_while%d:\n", cur_while);
    fprintf (fpout, "//WHILE%d_END\n\n", cur_while);

    return 0;
}

//======================================================================================================

static int Compile_oper (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    int oper = GET_DATA (node, operation);

    Compile (fpout,  node->left,  cur_namespace);
    Compile (fpout,  node->right, cur_namespace);

    fprintf (fpout, "%s\n", Name_operations [oper]);

    return 0;
}

//======================================================================================================

static int Compile_assig (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    Compile (fpout, node->right, cur_namespace);

    int id = Find_id_object (&cur_namespace->name_table, GET_DATA (node, obj));
    if (id == Not_init_object)
        return PROCESS_ERROR (UNINIT_VAR_ERR, "an uninitialized variable \'%s\' is used", GET_DATA (node, obj));

    int cell_memory = GET_MEM_CELL(id);;

    fprintf (fpout, "pop [%d+rax]", cell_memory);
    fprintf (fpout, "   //assigning_a_value_to_a_variable_\'%s\'\n\n", GET_DATA (node, obj));

    return 0;
}

//======================================================================================================

static int Compile_seq (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    Compile (fpout, node->left,  cur_namespace);
    
    if (GET_TYPE (node->left) == CALL) fprintf (fpout, "pop rbx\n");

    Compile (fpout, node->right, cur_namespace);

    return 0;
}

//======================================================================================================

static int Compile_block (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    Compile (fpout, node->right, cur_namespace);

    return 0;
}

//======================================================================================================

static int Compile_const (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    fprintf (fpout, "push %.3lf\n", GET_DATA (node, val));

    return 0;
}

//======================================================================================================

static int Compile_var (FILE *fpout, const Node *node, Namespace_struct *cur_namespace)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cur_namespace != nullptr && "name_table is nullptr");

    int id = Find_id_object (&cur_namespace->name_table, GET_DATA (node, obj));
    if (id == Not_init_object)
        return PROCESS_ERROR (UNINIT_VAR_ERR, "an uninitialized variable \'%s\' is used", GET_DATA (node, obj));

    int cell_memory = GET_MEM_CELL(id);;

    fprintf (fpout, "push [%d+rax]  //Put_on_stack_value_var_'%s'\n", cell_memory, GET_DATA (node, obj));
    
    return 0;
}

//======================================================================================================

int Load_ast_tree (Tree *ast_tree, const char *name_input_file)
{
    assert (ast_tree != nullptr && "ast_tree is nullptr");
    assert (name_input_file != nullptr && "name_input_file is nullptr");

    int fdin = Open_file_discriptor (name_input_file, O_RDONLY);
    if (fdin < 0)
        return PROCESS_ERROR (ERR_FILE_OPEN, "Error opening iputfile file named \"%s\"\n", name_input_file);

    Text_info text = {};
    
    if (Text_read (fdin, &text))
        return PROCESS_ERROR (ERR_FILE_READING, "Error reading into Text_info structure\n");

    if (Close_file_discriptor (fdin))
        return PROCESS_ERROR (ERR_FILE_CLOSE,"Error close input file named \"%s\"\n", name_input_file);
    
    Array_struct tokens = {};
    if (Array_ctor (&tokens, Init_array_capacity))
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokens ctor failed");

    int cnt_tokens =  Lexical_analysis_str (text.text_buf, &tokens);
    
    if (cnt_tokens <= 0)
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokenization failed with an error."
                              " Count tokens = %d\n", cnt_tokens);

    Free_buffer (&text);

    Array_recalloc (&tokens, cnt_tokens + 1);
    Array_set_ptr_by_ind (&tokens, cnt_tokens, strdup (""));
    
    int pos = 0;
    ast_tree->root = Build_tree (&pos, &tokens);

    if (Check_nullptr (ast_tree->root))   
        return PROCESS_ERROR (READ_AST_FORMAT, "Error read a tree with a pointer to the root |%p|",
                                               (char*) ast_tree->root);

    if (Array_dtor (&tokens))
        return PROCESS_ERROR (ANALYS_STR_ERR, "Tokens dtor failed");

    #ifdef LOOK_BACKEND_AST_TREE
        Draw_database (ast_tree);
    #endif

    return 0;
}

//======================================================================================================

#define GET_TOKEN(pos) (char*) Array_get_ptr_by_ind (tokens, (pos))

static Node* Build_tree (int *pos, const Array_struct *tokens)
{
    assert (pos != nullptr && "pos ins nullptr");
    assert (tokens != nullptr && "tokens ins nullptr");

    char *cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, "{"))
    {
        PROCESS_ERROR (READ_AST_FORMAT, "node must start with {\n"
                                        "cur token: |%s|", GET_TOKEN (*pos));
        return nullptr;
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);
    
    if (!strcmp (cur_token, "}"))
    {
        (*pos)++;
        return nullptr;
    }
    

    Node *node = Read_node_from_buffer (pos, tokens); 

    node->left  = Build_tree (pos, tokens); 
    
    cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, ","))
    {
        PROCESS_ERROR (READ_AST_FORMAT, "After the left son there should be \',\'.\n"
                                        "cur token: |%s|", cur_token);
        return nullptr;
    }

    (*pos)++;

    node->right = Build_tree (pos, tokens);

    cur_token = GET_TOKEN (*pos);
    if (strcmp (cur_token, "}")) 
    {
        PROCESS_ERROR (READ_AST_FORMAT, "After the left son there should be \'}\'.\n"
                                        "cur token: |%s|\n", cur_token);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//======================================================================================================

static Node* Read_node_from_buffer (int *pos, const Array_struct *tokens)
{
    assert (pos != nullptr && "pos ins nullptr");
    assert (tokens != nullptr && "tokens ins nullptr");

    char *cur_token = GET_TOKEN (*pos);

    Node *node = Create_empty_node ();
    DEF_TYPE (node, Define_type (cur_token));

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (strcmp (cur_token, ","))
    {
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_tolen: %s", cur_token);
        return nullptr;
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (!strcmp (cur_token, "NULL"))
        CHANGE_DATA (node, obj, nullptr);
    else if (isdigit (cur_token[0]))
        CHANGE_DATA (node, val, atof (cur_token));
    else
    {
        int op = Define_operation (cur_token);
        if (op == UNKNOWN_OP)
            CHANGE_DATA (node, obj, cur_token);
        else
            CHANGE_DATA (node, operation, op);
    }

    (*pos)++;
    cur_token = GET_TOKEN (*pos);

    if (strcmp (cur_token, ","))
    {
        PROCESS_ERROR (READ_NODE_ERR, "no \',\' after node data.\n cur_tolen: %s", cur_token);
        return nullptr;
    }

    (*pos)++;

    return node;
}

//======================================================================================================

static int Define_type (const char *token)
{
    assert (token != nullptr && "tokens is nullptr");

    int cnt_words = sizeof (Name_type_node)/ sizeof (Name_type_node [0]);
    for (int id = 0; id < cnt_words; id++)
        if (!strcmp (token, Name_type_node [id]))
            return id;

    return UNKNOWN_T;
}


//======================================================================================================

static int Define_operation (const char *token)
{
    assert (token != nullptr && "tokens is nullptr");

    int cnt_words = sizeof (Name_operations)/ sizeof (Name_operations [0]);
    for (int id = 0; id < cnt_words; id++)
        if (!strcmp (token, Name_operations [id]))
            return id;

    return UNKNOWN_OP;
}

//======================================================================================================