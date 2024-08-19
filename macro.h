/* Sizes realted to macro definition*/
#define MAX_LENGTH_OF_MACRO_NAME 100
#define MAX_NUM_OF_MACRO_ALLOCATED_STATEMENTS 20

/* Macro definition structure */
typedef struct macro_definition
{
    char macro_name[MAX_LENGTH_OF_MACRO_NAME]; /* macro name */
    int num_of_statements;   /* numnber of macro statements */
    char assembly_statements[MAX_NUM_OF_MACRO_ALLOCATED_STATEMENTS][MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* macro statements */
    struct macro_definition* next_macro_definition; /* pointer to the next macro definition in the linked list of macros*/
} macro_definition;

/* Get the assembly file stream
   Read the assembly file and expand the macros hat it contains (if there is any macro).
   Write the assembly statements with the expanded macros to the expanded macro file
   Return a valid indication flag for macros expansions */
int expand_macros(FILE* assembly_file, FILE* expanded_macro_file);

/* Get assembly statement.
   Check if it is a start of macro definition.
   Return indication flag: 1- it is a start of macro definition, 0-it isn't a start of macro definition,
           optional error text for invalid macro definition */
int is_start_of_macro(char* assembly_statement, char* macro_name, char* error_text);

/* Get assembly statement.
   Check if it is end of macro definition.
   Return indication flag : 1 - it is end of macro definition, 0 - it isn't end of macro definition,
   optional error text for invalid end of macro definition */
int is_end_of_macro(char* assembly_statement, char* macro_name, char* error_text);

/* Get assembly statement.
   Check if it is a reference to macro.
   Return indication flag : 1 - it is a reference to macro, 0 - it isn't a reference to macro,
   optional error text for invalid reference to macro */
int is_macro_reference(char* assembly_statement, char* error_text);

/* Get a macro name
   Alocate a memory for the macro and add it to the linked list of macros
   return optional error text */
void add_macro_to_table(char* macro_name, char* error_text);

/* Get macro name,
       macro statement
   If it is needed, allocate a memory for the new statement
   Add the statement to the macro statements in the linked list of macros
   return optional error text */
void add_macro_statement_to_table(char* macro_name, char* macro_statement, char* error_text);

/* Get macro name,
   Search the macro in the linked list of macros.
   return a pointer to the macro definitions
   return optional error text */
macro_definition* get_macro(char* macro_name);

/* Get a macro name
   search for the next statement of macro in the linked list of macro
   Return the next statement of the macro (empty statement if there isn't additional statement) */
void get_next_macro_statement(char* macro_name, char* statement);

/* Free the allocated linked list of macros */
void free_macros_Table();
