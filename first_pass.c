#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "first_pass.h"
#include "validation.h"

/* Get the file stream after macro expansion (the ".am" file).
   Perfom the first pass - converting the assembly statements to machine words without resolving symbols.
   Return the machine instruction words and the instruction words count
          and the machine data words and the machine data words count
          and a valid flag for the first pass */
int first_pass(FILE* expanded_macro_file, 
    int* machine_instruction_words, int* machine_data_words,
    int* machine_instruction_words_ctr, int* machine_data_words_ctr)
{
    char assembly_statement[MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* an assembly statement string*/
    int opcode = 0; /* The machine operation code according to undirective assmbly statemnt */
    int operands_count = 0; /* the number of operands in an assembly statement */
    int addressing_methods[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* the machine addressing methods according to undirective assembly statement operands */
    int immediate_values[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* machine immediate values according to undirective assembly statemsnt operands */
    int machine_register_codes[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* machine registers codes according to undirective assembly operands */
    char address_names[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* machine address names according to undirective assembly statement operands */
    int struct_fields_indexes[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* indexes of structure fields according to undirective assembly statement operands */

    int directive_type; /* The type of directive assembly statement */
    int data[MAX_NUM_OF_ASSAEMNLY_STATEMENT_DATA]; /* machine data words */
    int data_count; /* machine data words count */

    char symbol1[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* label in an assembly statement*/
    char symbol2[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* symbol of first operand of undirective assembly statement */
    char symbol3[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* symbol of the second operand of undirective assembly statement */

    int machine_addressing_word_1; /* The first machine word that is built from an assembly statement */
    int machine_addressing_word_2; /* The second optional machine word that is built from an assembly statement */
    int machine_addressing_words_count; /* the number of machine words that are built from one assembly statement*/

    char error_text[MAX_LENGTH_OF_ERROR_TEXT]; /* error text */
  
    int IC = 0; /* machine code instructions counter */
    int DC = 0; /* machine_code data counter */
    int statement_valid = 1; /* valid flag for an assembly statement */
    int total_statements_valid = 1; /* valid flag for all the assembly statements */
    int i = 0; /* index of the assemby statements */
    int j; /* index of ab assembly statement operands */

    *machine_instruction_words_ctr = 0; /* number of machine instruction words */
    *machine_data_words_ctr = 0; /* number of machine data words */


    /* Get each assembly statement from the expanded macro file stream
       and convert it to machine words without resolving symbols */
    while (fgets(assembly_statement, MAX_LENGTH_OF_ASSEMBLY_STATEMENT, expanded_macro_file) != NULL)
    {
        assembly_statement[strlen(assembly_statement) - 1] = '\0'; /* replace the newline character with '\0'*/
        error_text[0] = '\0'; /* initialize the error text */

        /* Handle an assembly directive statement */
        if (assembly_statement_is_directive(assembly_statement))
        {
            /* Convert an assembly directive statement to machine words */
            statement_valid = assembly_directive_statement_to_machine_code(assembly_statement,
                                                         &directive_type, data, &data_count, symbol1, error_text);
     
            /* Insert unresolved symbols to the symbols table */
            if (statement_valid && symbol1[0] != '\0')
            {
                if (directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_DATA_TYPE ||
                    directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_STRING_TYPE ||
                    directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_STRUCT_TYPE)
                    insert_symbol_to_table(SYMBOL_DATA_TYPE, symbol1, DC, error_text);
                else if (directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_EXTERN_TYPE)
                    insert_symbol_to_table(SYMBOL_EXTERN_TYPE, symbol1, 0, error_text);

                /* Handle error while trying to insert a symbol to the symbols table*/
                if (error_text[0] != '\0')
                    statement_valid = 0;
            }

            /*  If there isn't any error add the machine words to th list of machine data words */
            if (statement_valid != 0)
                for (j = 0; j < data_count; j++)
                    machine_data_words[DC++] = data[j];
        }

        /* Handle an assembly undirective statement*/
        else
        {
            /* Convert an assembly undirective statement to machine words */
            statement_valid = assembly_undirective_statement_to_machine_code(assembly_statement, i,
                &opcode,
                &operands_count,
                addressing_methods,
                immediate_values, machine_register_codes,
                address_names, struct_fields_indexes,
                symbol1, symbol2, symbol3,
                error_text);

            if (opcode != UNKNOWN_OPCODE)
            {
                /* Insert unresolved symbols to the symbols table */
                if (statement_valid != 0 && symbol1[0] != '\0')
                {
                    insert_symbol_to_table(SYMBOL_CODE_TYPE, symbol1, IC, error_text);
                    if (error_text[0] != '\0')
                        statement_valid = 0;
                } 

                if (statement_valid != 0)
                {
                    /* Build the first machine word for an assembly instruction statement */
                    machine_instruction_words[IC] = opcode << MACHINE_WORD_OPCODE_START_BIT;
                    if (operands_count == 1)
                        machine_instruction_words[IC] |= addressing_methods[0] << MACHINE_WORD_DEST_ADDRESSING_METHOD_START_BIT;
                    else if (operands_count == 2)
                    {
                        machine_instruction_words[IC] |= addressing_methods[0] << MACHINE_WORD_SRC_ADDRESSING_METHOD_START_BIT;
                        machine_instruction_words[IC] |= addressing_methods[1] << MACHINE_WORD_DEST_ADDRESSING_METHOD_START_BIT;
                    }
                    ++IC;

                    /* Build the next machine words for an assembly instruction statement */
                    if (operands_count == 2 &&
                        addressing_methods[0] == ADDRESSING_METHOD_REGISTER &&
                        addressing_methods[1] == ADDRESSING_METHOD_REGISTER)
                        machine_instruction_words[IC++] = build_two_registers_addressing_machine_words(machine_register_codes[0],
                            machine_register_codes[1]);
                    else for (j = 0; j < MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS; j++)
                    {
                        if ((operands_count > 0 && j == 0) ||
                            (operands_count > 1 && j == 1))
                        {
                            /* Build the addressing part of the machine words*/
                            machine_addressing_words_count = build_addressing_machine_words(
                                j,
                                addressing_methods[j], immediate_values[j],
                                machine_register_codes[j], struct_fields_indexes[j],
                                &machine_addressing_word_1, &machine_addressing_word_2);
                            machine_instruction_words[IC++] = machine_addressing_word_1;
                            if (machine_addressing_words_count > 1)
                                machine_instruction_words[IC++] = machine_addressing_word_2;
                        }
                    }
                }
            }    
        }

        /* Handle an error in building machine words  from an assembly statement */
        if (statement_valid == 0)           
        {
            total_statements_valid = 0;
            if (error_text[0] !='\0')
                print_error(error_text, i, assembly_statement);
        }

        ++i; /* advance the index to the next assembly statement */
    }

    /* Don't return anything if there is at least one error */  
    if (total_statements_valid != 0)
    { 
        /* Add offset to the data labels according the last instruction counter
           and the first address of the machine program */
        add_offset_to_symbols_values(IC + MACHINE_CODE_FIRST_ADDRESS, SYMBOL_DATA_TYPE);

        /* Set offset to instructions labels according to the first address of the machine program */
        add_offset_to_symbols_values(MACHINE_CODE_FIRST_ADDRESS, SYMBOL_CODE_TYPE);

        /* set the instruction counter and the the data counter */
        *machine_instruction_words_ctr = IC;
        *machine_data_words_ctr = DC;
    }

    return total_statements_valid;
}

