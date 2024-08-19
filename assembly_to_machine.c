#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "macro.h"
#include "first_pass.h"
#include "second_pass.h"
#include "coding.h"
#include "validation.h"

/* Machine operation codes */
#define MOV_OPCODE  0
#define CMP_OPCODE  1
#define ADD_OPCODE  2
#define SUB_OPCODE  3
#define NOT_OPCODE  4
#define CLR_OPCODE  5
#define LEA_OPCODE  6
#define INC_OPCODE  7
#define DEC_OPCODE  8
#define JMP_OPCODE  9
#define BNE_OPCODE 10
#define GET_OPCODE 11
#define PRN_OPCODE 12
#define JSR_OPCODE 13
#define RTS_OPCODE 14
#define HLT_OPCODE 15
#define NUM_OF_OPCODES 16

/*  Machine structure index */
#define STRUCT_FIELD1_INDEX         1
#define STRUCT_FIELD2_INDEX         2
#define UNKNOWN_STRUCT_FIELD_INDEX  0

/* Assembly statement type */
#define ASSEMBLY_EMPTY_STATEMENT_TYPE        0
#define ASSEMBLY_COMMENT_STATEMENT_TYPE      1
#define ASSEMBLY_DIRECTIVE_STATEMENT_TYPE    2
#define ASSEMBLY_INSTRUCTION_STATEMENT_TYPE  3
#define ASSEMBLY_UNKNOWN_STATEMENT_TYPE     -1

/* Assembly instruction definition structure */
struct assembly_instructions_definitions_struct
{
    int opcode; /* operation code*/
    int num_of_operands; /* number of operands */
    int src_operand_allowed_addressing_methods[NUM_OF_ADDRESING_METHODS]; /* allowed addressing methods in the first operand */
    int dest_operand_allowed_addressing_methods[NUM_OF_ADDRESING_METHODS]; /* allowed addressing methods in the second operand */
} assembly_instructions_definitions[] = {
      {MOV_OPCODE, 2,
      {ADDRESSING_METHOD_IMMEDIATE,
       ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER},
       {ADDRESSING_METHOD_DIRECT,
        ADDRESSING_METHOD_STRUCT,
        ADDRESSING_METHOD_REGISTER,
        UNKNOWN_ADDRESSING_METHOD}},

      {CMP_OPCODE, 2,
      {ADDRESSING_METHOD_IMMEDIATE,
       ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER},
       {ADDRESSING_METHOD_IMMEDIATE,
        ADDRESSING_METHOD_DIRECT,
        ADDRESSING_METHOD_STRUCT,
        ADDRESSING_METHOD_REGISTER}},

      {ADD_OPCODE, 2,
      {ADDRESSING_METHOD_IMMEDIATE,
       ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER},
       {ADDRESSING_METHOD_DIRECT,
        ADDRESSING_METHOD_STRUCT,
        ADDRESSING_METHOD_REGISTER,
        UNKNOWN_ADDRESSING_METHOD}},

      {SUB_OPCODE, 2,
      {ADDRESSING_METHOD_IMMEDIATE,
       ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER},
       {ADDRESSING_METHOD_DIRECT,
        ADDRESSING_METHOD_STRUCT,
        ADDRESSING_METHOD_REGISTER,
        UNKNOWN_ADDRESSING_METHOD}},

     {NOT_OPCODE, 1,
     {ADDRESSING_METHOD_DIRECT,
      ADDRESSING_METHOD_STRUCT,
      ADDRESSING_METHOD_REGISTER,
      UNKNOWN_ADDRESSING_METHOD},
      {UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD}},

      {CLR_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
        {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {LEA_OPCODE, 2,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD},
       {ADDRESSING_METHOD_DIRECT,
        ADDRESSING_METHOD_STRUCT,
        ADDRESSING_METHOD_REGISTER,
        UNKNOWN_ADDRESSING_METHOD}},

      {INC_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {DEC_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {JMP_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {BNE_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {GET_OPCODE, 1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {PRN_OPCODE, 1,
      {ADDRESSING_METHOD_IMMEDIATE,
       ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {JSR_OPCODE,  1,
      {ADDRESSING_METHOD_DIRECT,
       ADDRESSING_METHOD_STRUCT,
       ADDRESSING_METHOD_REGISTER,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

      {RTS_OPCODE, 0,
      {UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD,
       UNKNOWN_ADDRESSING_METHOD},
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD}},

       {HLT_OPCODE, 0,
       {UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD,
        UNKNOWN_ADDRESSING_METHOD},
        {UNKNOWN_ADDRESSING_METHOD,
         UNKNOWN_ADDRESSING_METHOD,
         UNKNOWN_ADDRESSING_METHOD,
         UNKNOWN_ADDRESSING_METHOD}}
};

/* Get machine registers codes of an assembly statement with two registers operands. 
   Build and return one machine words with the registers codes positioned in the proper places */
int build_two_registers_addressing_machine_words(int machine_register_code_1, int machine_register_code_2)
{
    int address_word; /* addressing machine word */

    /* position the registers codes in the proper places of the machine word */
    address_word = machine_register_code_1 << MACHINE_WORD_SRC_REGISTER_START_BIT;
    address_word |= machine_register_code_2 << MACHINE_WORD_DEST_REGISTER_START_BIT;

    return address_word;
}

/* Get an assembly operand index
       the addressing method of the operand,
       the optional immediate value of the operand,
       the optional machine register of the operand,
       and the optional structure index of the operand.    
   Build one addressing machine word with the proper parameters positioned in the proper places. 
   Build a second optional machine word with the proper parameters positioned in the proper places.
   Return the built machie words and the number of them*/
int build_addressing_machine_words(int operand_index,
                                int addressing_method, int immediate_value, 
                                int machine_register_code, int struct_field_index,
                                int *address_word_1, int *address_word_2)
{
    int address_words_count = 1; /* the number of built machine words*/
    int start_bit = 0; /* the start position of a field in the machine words bits */
     
    switch (addressing_method)
    {
        /* Build two machine words for immediate value operand */
        case ADDRESSING_METHOD_IMMEDIATE:
            *address_word_1 = A_R_E_CODING_ABSOLUTE << MACHINE_WORD_A_R_E_START_BIT;
            *address_word_1 |= immediate_value << MACHINE_WORD_ADDRESS_START_BIT;
            break;

        /* Build two machine words for register operand */
        case ADDRESSING_METHOD_REGISTER:
            *address_word_1 = A_R_E_CODING_ABSOLUTE << MACHINE_WORD_A_R_E_START_BIT;
            start_bit = operand_index == 0 ? MACHINE_WORD_SRC_REGISTER_START_BIT : MACHINE_WORD_DEST_REGISTER_START_BIT;
            *address_word_1 |= machine_register_code << start_bit;
            break;

        /* Build one machine word for direct operand value, with a place for the second pass */
        case ADDRESSING_METHOD_DIRECT:
            *address_word_1 = 0; /* address and A,R,E would be set in the second pass */
            break;

        /* Build two machine words for a structure operand,
           first word with a place for the second pass, 
           second worrd with the structure field index */
        case ADDRESSING_METHOD_STRUCT:
            *address_word_1 = A_R_E_CODING_RELOCATABLE << MACHINE_WORD_A_R_E_START_BIT; /* adress would be set in the second pass */
            *address_word_2 = struct_field_index << MACHINE_WORD_ADDRESS_START_BIT;
            address_words_count = 2;
            break;        
    }

    return address_words_count;
}


/* Get an assembly statement
   Check if it is a directive statement
   return: 1-directive statement 0-undirective statement*/
int assembly_statement_is_directive(char* assembly_statement)
{
    int directive = 0; /* directive statement indication flag */

    int first_argument_index_not_Include_label = 0;

    char assembly_statement_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT];
    char error_text[MAX_LENGTH_OF_ERROR_TEXT];
    int assembly_arguments_count;

    /* Get the assembly statement arguments */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_statement_arguments,
        &first_argument_index_not_Include_label, error_text);

    /* Check if it is a directive statement  */
    if (assembly_arguments_count > first_argument_index_not_Include_label &&
        assembly_statement_arguments[first_argument_index_not_Include_label][0] == '.')
        directive = 1;

    return directive;
}

/* Get an assembly directive statement
   check its arguments 
   Return the directive type,
          the data values in the assembly directive statement,
          the number of data values in the assembly directive statement,
          the symbol of the assembly entry or extern directive statement
          the optional error text */
int assembly_directive_statement_to_machine_code(char* assembly_statement, int* directive_type,
                                                     int* data, int* data_count, char* symbol, char* error_text)
{
    int valid = 1; /* valid flag of the assembly directive statement*/
    int i; /* index of the arguments in the assembly directive statement */
    char assembly_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* assembly directive statement argument*/
    int assembly_arguments_count; /* number of arguments in the assembly directive statement */
    int first_argument_index_not_Include_label; /* index of the first argument: 0- if the statement hasen't a label, 1- if there is a label */
    char first_argument_not_Include_label[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* the first aregument (label is not considered) */
    symbol[0] = '\0'; /* symbol of the directive statement */

    /* Get the assembly arguments */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_arguments,
        &first_argument_index_not_Include_label, error_text);

    if (error_text[0] != '\0')
        valid = 0;
    else
    {
        /* Get assembly statement label */
        if (first_argument_index_not_Include_label == 1)
            get_symbol_from_label(symbol, assembly_arguments[0]);

        /* Get the first argument (label is not included) */
        strcpy(first_argument_not_Include_label, assembly_arguments[first_argument_index_not_Include_label]);

        /* Get the directive type */
        *directive_type = get_directive_type(first_argument_not_Include_label);        
        if (*directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_UNKNOWN_TYPE)
	{
	    strcpy(error_text, "Unknown directive type.           ");
            valid = 0;
	}

        /* Get the data according to the assembly statement directive type */
        *data_count = 0;
        switch (*directive_type)
        {
            /* Get data integer values of data directive statement*/
            case ASSEMBLY_DIRECTIVE_STATEMENT_DATA_TYPE:
                if ((valid = validate_commas(assembly_statement, first_argument_index_not_Include_label + 1, error_text)))
                    for (i = first_argument_index_not_Include_label + 1; i < assembly_arguments_count && valid; i++)
                        if ((valid = validate_integer_number(assembly_arguments[i], error_text)))
                            data[(*data_count)++] = atoi(assembly_arguments[i]);
                break;

            /* Get the string data of a string assembly statement */
            case ASSEMBLY_DIRECTIVE_STATEMENT_STRING_TYPE:
                if ((valid = validate_commas(assembly_statement, first_argument_index_not_Include_label + 1, error_text)))
                    if ((valid = validate_string(assembly_arguments[first_argument_index_not_Include_label + 1], error_text)))
                    {
                        for (i = 1; i < (int)strlen(assembly_arguments[first_argument_index_not_Include_label + 1]) - 1; i++)
                            data[(*data_count)++] = assembly_arguments[first_argument_index_not_Include_label + 1][i];
                        data[(*data_count)++] = '\0';
                    }
                break;

            /* Get the values (integer value and a string) of the structure assembly statement */
            case ASSEMBLY_DIRECTIVE_STATEMENT_STRUCT_TYPE:
                if ((valid = validate_commas(assembly_statement, first_argument_index_not_Include_label + 1, error_text)))
                    if ((valid = validate_integer_number(assembly_arguments[first_argument_index_not_Include_label + 1], error_text)))
                        if ((valid = validate_string(assembly_arguments[first_argument_index_not_Include_label + 2], error_text)))
                        {
                            data[(*data_count)++] = atoi(assembly_arguments[first_argument_index_not_Include_label + 1]);
                            for (i = 1; i < (int)strlen(assembly_arguments[first_argument_index_not_Include_label + 2]) - 1; i++)
                                data[(*data_count)++] = assembly_arguments[first_argument_index_not_Include_label + 2][i];
                            data[(*data_count)++] = '\0';
                        }
                break;

            /* Get the symbol of the assembly entry or extern directive statement */    
            case ASSEMBLY_DIRECTIVE_STATEMENT_ENTRY_TYPE:
            case ASSEMBLY_DIRECTIVE_STATEMENT_EXTERN_TYPE:
                if ((valid = validate_commas(assembly_statement, first_argument_index_not_Include_label + 1, error_text)))
                    /* If there is label to the the entry or extern statement then it is overidden and ignored */
                    strcpy(symbol, assembly_arguments[first_argument_index_not_Include_label + 1]);
                break;
        }
    }

    return valid;
}

/* Get a directive statement name (started with '.' character)
   check the name.
   Return the proper directive type*/
int get_directive_type(char *directive_name)
{
    int directive_type; /* the assembly directive type */

    if (strcmp(directive_name, ".data") == 0)
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_DATA_TYPE;
    else if (strcmp(directive_name, ".struct") == 0)
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_STRUCT_TYPE;
    else if (strcmp(directive_name, ".string") == 0)
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_STRING_TYPE;
    else if (strcmp(directive_name, ".entry") == 0)
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_ENTRY_TYPE;
    else if (strcmp(directive_name, ".extern") == 0)
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_EXTERN_TYPE;
    else  
        directive_type = ASSEMBLY_DIRECTIVE_STATEMENT_UNKNOWN_TYPE;  

    return directive_type;
} 

/* Get an assembly statement label 
   Remove the ':' character from the end of the label. 
   Return the label synbol without the ':' character. */
void get_symbol_from_label(char* symbol, char* label)
{
    strcpy(symbol, label);

    /* Remove the ':' character from the end of the label*/
    if (strlen(symbol) > 1)
        symbol[strlen(symbol) - 1] = '\0';
}

/* Get an assembly undirective statement. 
   Check its arguments.
   Return the machine operation code,
           the number of operands,
           the operands addressing methods,
           the optional immediate values,
           the optional address names,
           the optional structures fields indexs,
           the optional label symbol,
           the optional first operand symbol,
           the optional second operand symbol,
           the otional error text */
int assembly_undirective_statement_to_machine_code(char* assembly_statement, int assembly_statement_index,
										int* opcode,
										int* operands_count,
										int* addressing_methods,
										int* immediate_values, int* machine_register_codes,
										char address_names[][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT], int* struct_fields_indexs,
                                        char *symbol1, char *symbol2, char* symbol3,
                                        char* error_text)
{
    int valid = 1; /* valid flag */

    char assembly_arguments[MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* the arguments of the undirective assembly statement */
    char assembly_operation[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* the assembly operation string */
    int assembly_arguments_count; /* the nuber of arguments in the assembly undirective statement */
    int first_argument_index_not_Include_label; /* the first argument in the assembly statement (label is not considerd */

    symbol1[0] = '\0';
    symbol2[0] = '\0';
    symbol3[0] = '\0';

    /* Get the assembly statement arguments */
    assembly_arguments_count = get_assembly_statement_arguments(assembly_statement, assembly_arguments,
                                                                     &first_argument_index_not_Include_label ,error_text);

    /* Handle only an assembly statenment that is not a comment statement and not an empty statement
       and without an error in getting arguments from it */
    if (assembly_arguments_count > 0)
    {
        /* Get the assembly atatement label */
        if (first_argument_index_not_Include_label == 1)
            get_symbol_from_label(symbol1, assembly_arguments[0]);

        /* Get the assembly statement operation */
        strcpy(assembly_operation, assembly_arguments[first_argument_index_not_Include_label]);
        *opcode = get_machine_operation_code(assembly_operation);
        if (*opcode == UNKNOWN_OPCODE)
            strcpy(error_text, "Unknown operation.              ");

        /* Get the assembly statement operands */
        if (error_text[0] == '\0')
        {
            *operands_count = assembly_instructions_definitions[*opcode].num_of_operands;

            /* Validate seperators (blanks, tabs and commas) in the assembly statement */
            validate_commas(assembly_statement, first_argument_index_not_Include_label + 1, error_text);

            /* Validate the number of arguments in the assembly statement */
            if (assembly_arguments_count != *operands_count + 1 + first_argument_index_not_Include_label)
                strcpy(error_text, "Invalid number of arguments.        ");

            /* Get the assembly statement operands addressing methods */
            if (error_text[0] == '\0' && *operands_count > 0)
            {
                addressing_methods[0] = get_addressing_method_parameters(assembly_arguments[first_argument_index_not_Include_label + 1],
                    assembly_instructions_definitions[*opcode].src_operand_allowed_addressing_methods,
                    &immediate_values[0], &machine_register_codes[0],
                    address_names[0], &struct_fields_indexs[0],
                    symbol2, error_text);
                if (error_text[0] == '\0' && *operands_count > 1)
                    addressing_methods[1] = get_addressing_method_parameters(assembly_arguments[first_argument_index_not_Include_label + 2],
                        assembly_instructions_definitions[*opcode].dest_operand_allowed_addressing_methods,
                        &immediate_values[1], &machine_register_codes[1],
                        address_names[1], &struct_fields_indexs[1],
                        symbol3, error_text);
            }
        }
        else
            *opcode = UNKNOWN_OPCODE;
    }
    else
        *opcode = UNKNOWN_OPCODE;


    if (error_text[0] != '\0')
        valid = 0;    

    return valid;
}

/* Get an assembly statement.
   Check its fields.
   Return its arguments 
          the index of the first argument (1- if the statement has a label, 0- if thers isn't a label) 
          optional error text */
int get_assembly_statement_arguments(char* assembly_statement, 
                           char assembly_arguments[][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT],
                           int *first_argument_index_not_Include_label,
                           char *error_text)
{
    char saved_assembly_statement[MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* for storing the assembly statement*/
    int assembly_arguments_count = 0; /* the number of arguments*/
    char* argument; /* an argument string */   
    *first_argument_index_not_Include_label = 0; /* the first argument index (label is not considerd */

    strcpy(saved_assembly_statement, assembly_statement);
    error_text[0] = '\0';

    /* If the statement is a comment then do nothing */
    if (assembly_statement[0] != ';')
    {
        argument = strtok(assembly_statement, " \t,");
        while (argument != NULL && error_text[0] == '\0')
        {
            if (strlen(argument) < MAX_LENGTH_OF_ASSEMBLY_ARGUMENT)
            {
                strcpy(assembly_arguments[assembly_arguments_count++], argument);
                argument = strtok(NULL, " \t,");
            }
            else
                strcpy(error_text, "Argument too long in assembly statement.");
        }

        if (error_text[0] == '\0')
        {           

            if (assembly_arguments_count > MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS)
                strcpy(error_text, "Too much argumets in assembly statement.");

            /* Check if there is a label at the start of the assembly statement */
            else if (assembly_arguments_count > 0 &&
                assembly_arguments[0][strlen(assembly_arguments[0]) - 1] == ':')
            {
                *first_argument_index_not_Include_label = 1;
                validate_label(assembly_arguments[0], error_text);
            }

        }       

        if (error_text[0] != '\0')
            assembly_arguments_count = 0;
    }

    strcpy(assembly_statement, saved_assembly_statement);

    return assembly_arguments_count;
}

/* Get the assembly operation argument.
   Convert it to machine operation code.
   Return the machine operation code*/
int get_machine_operation_code(char* assembly_operation)
{
    int operation_code = UNKNOWN_OPCODE; /* the machine operation code */

    if (strcmp(assembly_operation, "mov") == 0)
        operation_code = MOV_OPCODE;
    else if (strcmp(assembly_operation, "cmp") == 0)
        operation_code = CMP_OPCODE;
    else if (strcmp(assembly_operation, "add") == 0)
        operation_code = ADD_OPCODE;
    else if (strcmp(assembly_operation, "sub") == 0)
        operation_code = SUB_OPCODE;
    else if (strcmp(assembly_operation, "not") == 0)
        operation_code = NOT_OPCODE;
    else if (strcmp(assembly_operation, "clr") == 0)
        operation_code = CLR_OPCODE;
    else if (strcmp(assembly_operation, "lea") == 0)
        operation_code = LEA_OPCODE;
    else if (strcmp(assembly_operation, "inc") == 0)
        operation_code = INC_OPCODE;
    else if (strcmp(assembly_operation, "dec") == 0)
        operation_code = DEC_OPCODE;
    else if (strcmp(assembly_operation, "jmp") == 0)
        operation_code = JMP_OPCODE;
    else if (strcmp(assembly_operation, "bne") == 0)
        operation_code = BNE_OPCODE;
    else if (strcmp(assembly_operation, "get") == 0)
        operation_code = GET_OPCODE;
    else if (strcmp(assembly_operation, "prn") == 0)
        operation_code = PRN_OPCODE;
    else if (strcmp(assembly_operation, "jsr") == 0)
        operation_code = JSR_OPCODE;
    else if (strcmp(assembly_operation, "rts") == 0)
        operation_code = RTS_OPCODE;
    else if (strcmp(assembly_operation, "hlt") == 0)
        operation_code = HLT_OPCODE; 

    return operation_code;
}

/* Get an assembly statement operand,
       and the allowed addressing methods.
   Check the operand.
   Return the addrssing method
          the optional immediate value,
          the optional machine registers code,
          the optional address name,
          the optional structures index,
          the optional symbol,
          the optional error text.*/
int get_addressing_method_parameters(char* assembly_operand, int* allowed_addressing_methods,
    int* immediate_value, int* machine_register_code,
    char *address_name, int* struct_field_index,
    char* symbol, char* error_text)
{
    int addressing_method = UNKNOWN_ADDRESSING_METHOD; /* the operand addressing method */

    /* Check if the operand has an immediate value */
    if (get_assembly_immediate_value(assembly_operand, immediate_value, error_text) != 0)
        addressing_method = ADDRESSING_METHOD_IMMEDIATE;

    /* Check if the operand has a register name */
    else if ((*machine_register_code = get_machine_register_code(assembly_operand)) != UKNOWN_REGISTER)
        addressing_method = ADDRESSING_METHOD_REGISTER;

    /* Check if the operand has a structure parameters */
    else if (get_assembly_struct_parameters(assembly_operand, address_name, struct_field_index) != 0)
    {
        if (*struct_field_index == STRUCT_FIELD1_INDEX || *struct_field_index == STRUCT_FIELD2_INDEX)
        {
            addressing_method = ADDRESSING_METHOD_STRUCT;
            strcpy(symbol, address_name);
        }
        else
            strcpy(error_text, "Invalid index of structure.");
    }

    /* Check if the operand has an address name */
    else if (get_assembly_address_name(assembly_operand, address_name) > 0)
    {
        addressing_method = ADDRESSING_METHOD_DIRECT;
        strcpy(symbol, assembly_operand);
    }

    return addressing_method;
}

/* Get an assembly statement operand.
   Check if it has aregister name
   Return the register machine code */
int get_machine_register_code(char* assembly_operand)
{
    int register_code = UKNOWN_REGISTER; /* the register machine code */

    if (strcmp(assembly_operand, "r0") == 0)
        register_code = 0;
    else if (strcmp(assembly_operand, "r1") == 0)
        register_code = 1;
    else if (strcmp(assembly_operand, "r2") == 0)
        register_code = 2;
    else if (strcmp(assembly_operand, "r3") == 0)
        register_code = 3;
    else if (strcmp(assembly_operand, "r4") == 0)
        register_code = 4;
    else if (strcmp(assembly_operand, "r5") == 0)
        register_code = 5;
    else if (strcmp(assembly_operand, "r6") == 0)
        register_code = 6;
    else if (strcmp(assembly_operand, "r7") == 0)
        register_code = 7;

    return register_code;
}

/* Get an assembly statement operand
   Check if it has an immediate value
   Return optional immediate value,
          optional error text,
          an indication flag that the operand has an immediate value */
int get_assembly_immediate_value(char* assembly_operand, int* immediate_value, char* error_text)
{
    int is_immediate_value = 0; /* an indication flag that the operand has an immediate value */
    error_text[0] = '\0';

    /*  Check if the operand has a valid immediate value */
    if (assembly_operand[0] == '#' && strlen(assembly_operand) > 1)
    {
        if (validate_integer_number(&assembly_operand[1], error_text))
            *immediate_value = atoi(&assembly_operand[1]);
   
        is_immediate_value = 1;
    }

    return is_immediate_value;
}

/* Get an assembly statement operand
   Check if it has structure parameters
   Return the structure name,
          the structure fields index,
          an indication flag that the operand has structure parameters */
int get_assembly_struct_parameters(char* assembly_operand, char* struct_name, int* struct_field_index)
{
    int period_position = -1; /* the position of the period character in the operand characters */
    unsigned int i; /* index of the operand characters */

    int valid = 0;  /* indication flag that the operand has structure parameters */ 

    /* Find the optional period character position */
    for (i = 0; i < strlen(assembly_operand) && period_position == -1; i++)
    {
        if (assembly_operand[i] == '.')
            period_position = i;
    }
  
    /* Get the optional structure name and structure field index */
    if (period_position != -1)
    {
        strcpy(struct_name, assembly_operand);
        struct_name[period_position] ='\0';
        *struct_field_index = atoi(&assembly_operand[period_position + 1]);
        
        if (strlen(struct_name) > 0)
            valid = 1;
    }

    return valid;
}


/* Get an assembly statement operand
   Check if it has an address name
   Return the optional address name,
          an indication flag that the operand has an address name */
int get_assembly_address_name(char* assembly_operand, char* address_name)
{
    int valid = 0; /* indication flag that the operand has an address name */

    /* get the address name */
    if (strlen(assembly_operand) > 0)
    {
        strcpy(address_name, assembly_operand);
        valid = 1;
    }

    return valid;
}

