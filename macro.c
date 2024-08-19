#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "macro.h"
#include "validation.h"

void allocate_macro(char* macro_name, char* error_text);

macro_definition* first_macro_definiton = NULL; /* pointer to linked list of allocated macro definitions */
macro_definition* last_macro_definition = NULL; /* pointer to last macro in linked list of allocated macro definitions */
macro_definition* current_macro_definition = NULL; /* pointer to the current macro definition in the linked list */
int current_macro_statements_ctr = 0; /* number of statements in the current macro definition*/

/* Get the assembly file stream 
   Read the assembly file and expand the macros hat it contains (if there is any macro).
   Write the assembly statements with the expanded macros to the expanded macro file
   Return a valid indication flag for macros expansions */
int expand_macros(FILE* assembly_file, FILE* expanded_macro_file)
{
    char assembly_statement[MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* the current assembly staement */
    char macro_name[MAX_LENGTH_OF_MACRO_NAME]; /* the name of the current macro */
    char macro_statement[MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* the current statement of the current macro */
    int macro_definition = 0; /* indication flag for macro definition in the assembly statements */
    char error_text[MAX_LENGTH_OF_ERROR_TEXT]; /* optional error text */
    int valid = 1; /* valid indication flag for macros expansions */
    int i = 0; /* index for assembly statements */

    /* Read each assembly statement until EOF */
    while (fgets(assembly_statement, MAX_LENGTH_OF_ASSEMBLY_STATEMENT, assembly_file) != NULL && valid)
    {
        if (strlen(assembly_statement) > 0 && assembly_statement[strlen(assembly_statement) - 1] == '\n')
            assembly_statement[strlen(assembly_statement) - 1] = '\0'; /* replace the newline character with '\0'*/

        error_text[0] = '\0';

        /* Check if the current assembly statement is a start of a macro definition */
        if (is_start_of_macro(assembly_statement, macro_name, error_text))
        {
            if (error_text[0] != '\0') 
		valid = 0;

            /* Add the macro to the linked list of macro and indicate that the macro definition is started*/
	    else
	    {	
		add_macro_to_table(macro_name, error_text);
		macro_definition = 1;
	    }
        }
       
        /* Check if macro definition is started */
        else if (macro_definition)
        {
            /* Indicate end of macro definition*/
            if (is_end_of_macro(assembly_statement, macro_name, error_text))
                macro_definition = 0;

            /* Continue adding statements to the macro definition */
            else
                add_macro_statement_to_table(macro_name, assembly_statement, error_text);
        }

        /* Check if amacro is references by the assembly statement */
        else if (is_macro_reference(assembly_statement, error_text) && error_text[0] == '\0')
        {
            /* Expand the macro */
            get_next_macro_statement(assembly_statement, macro_statement);
            while (macro_statement[0] != '\0')
            {
                macro_statement[strlen(macro_statement) + 1] = '\0';
                macro_statement[strlen(macro_statement)] = '\n'; /* return the new line character */                
                fputs(macro_statement, expanded_macro_file);      
                get_next_macro_statement(macro_name, macro_statement);
            }
        }

        /*  Write to the expanded macro file an assembly statement that is not related to any macro */
        else
        {
            assembly_statement[strlen(assembly_statement) + 1] = '\0';
            assembly_statement[strlen(assembly_statement)] = '\n'; /* return the new line character */
            fputs(assembly_statement, expanded_macro_file);
        }

        /* Write the error text when there is any error in the macros expansions */
        if (error_text[0] != '\0')
        {
            print_error(error_text, i, assembly_statement);
            valid = 0;
        }

        ++i;
    }

    /* Free the allocated macros in the linked lis of macros*/
    free_macros_Table();

    return valid;
}

/* Get assembly statement. 
   Check if it is a start of macro definition. 
   Return indication flag: 1- it is a start of macro definition, 0-it isn't a start of macro definition,
           optional error text for invalid macro definition */
int is_start_of_macro(char *assembly_statement, char* macro_name, char* error_text)
{
    int start_of_macro = 0; /* indication flag for start of macro definition.*/
    char assembly_statement_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* The arguments of the current assembly statement */
    int assembly_arguments_count; /*  number of arguments in the assembly statement */
    int first_argument_index_not_Include_label = 0; /* the first argument in the assembly statement (label is not considerd) */   

    /* Get the arguments of the assembly statement */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_statement_arguments,
        &first_argument_index_not_Include_label, error_text);
    error_text[0] = '\0'; /* detect errors that only related to macros definitions */

    /* Check if it is a start of macro definition */
    if (assembly_arguments_count > 0 &&
        strcmp(assembly_statement_arguments[0], "macro") == 0)
    {
        start_of_macro = 1;

        /* Check for errors in the macro definition */
        if (assembly_arguments_count != 2)
            strcpy(error_text, "Invalid number of arguments in macro statement.");
        else
        {
            strcpy(macro_name, assembly_statement_arguments[1]);
            if (get_machine_operation_code(macro_name) != UNKNOWN_OPCODE)
                strcpy(error_text, "Invalid macro name. Identical to instruction name.");
            else if (get_directive_type(macro_name) != ASSEMBLY_DIRECTIVE_STATEMENT_UNKNOWN_TYPE)
                strcpy(error_text, "Invalid macro name. Identical to directive name.");
        }
    }  

    return start_of_macro;
}

/* Get assembly statement.
   Check if it is end of macro definition.
   Return indication flag : 1 - it is end of macro definition, 0 - it isn't end of macro definition,
   optional error text for invalid end of macro definition */
int is_end_of_macro(char* assembly_statement, char* macro_name, char* error_text)
{
    int end_of_macro = 0; /* indication flag for end of macro definition.*/

    char assembly_statement_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* arguments of the assembly statement */
    int assembly_arguments_count = 0; /* number of arguments in the assembly statement */
    int first_argument_index_not_Include_label = 0; /* first argument index (label is not considerd) */ 

    /* Get the arguments of the assembly statement */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_statement_arguments,
        &first_argument_index_not_Include_label, error_text);
    error_text[0] = '\0'; /* detect errors that only related to macros definitions */

    /* Check if it is end of macro statement */
    if (assembly_arguments_count > 0 &&
        strcmp(assembly_statement_arguments[0], "endmacro") == 0)
    {
        end_of_macro = 1;
        if (assembly_arguments_count != 1)
            strcpy(error_text, "Number of arguments in macro statement is not valid.");
    }

    return end_of_macro;
}

/* Get assembly statement.
   Check if it is a reference to macro.
   Return indication flag : 1 - it is a reference to macro, 0 - it isn't a reference to macro,
   optional error text for invalid reference to macro */
int is_macro_reference(char* assembly_statement, char* error_text)
{
    int macro_reference = 0; /* indication flag for a reference to macro .*/

    char assembly_statement_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* arguments of the assembly statement */
    int assembly_arguments_count = 0; /* number of arguments in the assembly statement */
    int first_argument_index_not_Include_label = 0; /* first argument index (label is not considerd) */
 
    /* Get the assembly statement arguments */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_statement_arguments,
        &first_argument_index_not_Include_label, error_text);
    error_text[0] = '\0'; /* detect errors that only related to macros definitions */

    /* Check only assembly statement that is not empty statement nor remark statement */
    if (assembly_arguments_count > 0)
    {
        /* Check if it is a macro reference */
        if (get_macro(assembly_statement_arguments[0]) != NULL)
        {
            macro_reference = 1;
            if (assembly_arguments_count != 1)
                strcpy(error_text, "Invalid number of arguments in a reference to macro.");
        }
    }

    return macro_reference;
}

/* Get a macro name
   Alocate a memory for the macro and add it to the linked list of macros
   return optional error text */
void add_macro_to_table(char* macro_name, char* error_text)
{
    error_text[0] = '\0';

    /* Check if the macro already exists in the linked list of macros */
    if (get_macro(macro_name) != NULL)
        strcpy(error_text, "Macro is already defined.    ");

    /*  Alocate a memory for the macro and add it to the linked list of macros */
    else
        allocate_macro(macro_name, error_text);   
}


/* Get a macro name
   Alocate a memory for the macro and add it to the linked list of macros
   return optional error text */
void allocate_macro(char* macro_name, char* error_text)
{
    macro_definition* macro_def;
    error_text[0] = '\0';

    macro_def = malloc(sizeof(macro_definition));
    if (macro_def == NULL)
        strcpy(error_text, "Allocation error while trying to add a new macro to table.");
    else
    {
        /* Initialize the macro fields */
        strcpy(macro_def->macro_name, macro_name);
        macro_def->num_of_statements = 0;

        /* Link the macro to the table*/
        if (last_macro_definition != NULL)
            last_macro_definition->next_macro_definition = macro_def;
        else
            first_macro_definiton = macro_def;
        macro_def->next_macro_definition = NULL;
        last_macro_definition = macro_def;
        current_macro_definition = macro_def;
    }
}

/* Get macro name,
       macro statement 
   If it is needed, allocate a memory for the new statement
   Add the statement to the macro statements in the linked list of macros   
   return optional error text */
void add_macro_statement_to_table(char* macro_name, char* macro_statement, char* error_text)
{ 
    error_text[0] = '\0';

    /* If there is enough space for the new statement,
       add the statement to the macro statements in the linked list of macros */
    if (last_macro_definition->num_of_statements < MAX_NUM_OF_MACRO_ALLOCATED_STATEMENTS)
        strcpy(last_macro_definition->assembly_statements[last_macro_definition->num_of_statements++], macro_statement);

    /* If there is not enough space for the new statement,
       allocate a memory for the new statement */
    else
    {
        allocate_macro(macro_name, error_text);
        if (error_text[0] == '\0')
            strcpy(last_macro_definition->assembly_statements[last_macro_definition->num_of_statements++], macro_statement);
    }
}

/* Get macro name,
   Search the macro in the linked list of macros.
   return a pointer to the macro definitions
   return optional error text */
macro_definition* get_macro(char* macro_name)
{
    macro_definition* macro_def = NULL; /* a pointer to the found macro definitions*/
    macro_definition* table_macro_def = first_macro_definiton; /* a pointer to definitions of one of the macros*/
    current_macro_definition = NULL; /* a pointer to the macro definitions */
   
    /* Search the linked list of macros to find the  required macro*/
    while (table_macro_def != NULL && macro_def == NULL)
    {
        if (strcmp(table_macro_def->macro_name, macro_name) == 0)
            macro_def = table_macro_def;
        current_macro_definition = table_macro_def;
        table_macro_def = table_macro_def->next_macro_definition;
    }

    return macro_def;
}

/* Get a macro name 
   search for the next statement of macro in the linked list of macro 
   Return the next statement of the macro (empty statement if there isn't additional statement) */
void get_next_macro_statement(char* macro_name, char* statement)
{      
    statement[0] = '\0';

    if (current_macro_definition != NULL)
    {
        /* Get the next statement from the next macro definitions when there isn't enough space in the current macro*/
        if (current_macro_statements_ctr >= current_macro_definition->num_of_statements)
        {
            current_macro_definition = current_macro_definition->next_macro_definition;
            if (current_macro_definition != NULL &&
                strcmp(current_macro_definition->macro_name, macro_name) == 0 &&
                current_macro_definition->num_of_statements > 0)
            {
                current_macro_statements_ctr = 0;
                strcpy(statement, last_macro_definition->assembly_statements[current_macro_statements_ctr++]);
            }                
        }

        /* Get the next statement from the current macro when there is enough space frorthe next statement*/
        else
            strcpy(statement, last_macro_definition->assembly_statements[current_macro_statements_ctr++]);
    }    
}

/* Free the allocated linked list of macros */
void free_macros_Table()
{
    macro_definition* table_macro = first_macro_definiton;
    macro_definition* next_macro;

    /* free each allocated macro definitions in the linked list of macro*/
    while (table_macro != NULL)
    {
        next_macro = table_macro->next_macro_definition;

        free(table_macro);

        table_macro = next_macro;
    }

    /* Reset the pointers and counters that are related to the linked list of the macros */
    first_macro_definiton = NULL;
    last_macro_definition = NULL;
    current_macro_definition = NULL;
    current_macro_statements_ctr = 0;
}
/*********************************************************************************************************/

