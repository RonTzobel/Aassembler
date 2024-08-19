#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "second_pass.h"
#include "coding.h"
#include "validation.h"

/* Get the expanded macros file stream,
       the entries file stream, 
       the externals file stream,
       and the machine instructions words and the machine instruction words count that were built in the first pass,
   Read the expandes macro file and resolve the unresolved symbols from the first pass.
   Write entries addresses to the entries file.
   Write external addresses to the externals file.
   Return the machine instructions words with the resolved symbols and a valid flag for the second pass.  */
int second_pass(FILE* expanded_macro_file, FILE* entries_file, FILE* externals_file,
    int* machine_instruction_words, int* machine_instruction_words_ctr)
{   
    char assembly_statement[MAX_LENGTH_OF_ASSEMBLY_STATEMENT]; /* an assembly statement string*/
    int opcode = 0; /* The machine operation code according to undirective assmbly statemnt */
    int operands_count = 0; /* the number of operands in an assembly statement */
    int addressing_methods[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* the machine addressing methods according to undirective assembly statement operands */
    int immediate_values[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* machine immediate values according to undirective assembly statemsnt operands */
    int machine_register_codes[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* machine registers codes according to undirective assembly operands */
    char address_names[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* machine address names according to undirective assembly statement operands */
    int struct_fields_indexes[MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS]; /* indexes of structure fields according to undirective assembly statement */

    char symbol1[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* label in an assembly statement*/
    char symbol2[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* symbol of first operand of undirective assembly statement */
    char symbol3[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* symbol of the second operand of undirective assembly statement */
    char symbol[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* symbol of the first or the second operand of undirective assembly statement*/

    int machine_addressing_word_1; /* The first machine word that is built from an assembly statement */
    int machine_addressing_word_2; /* The second optional machine word that is built from an assembly statement */
    int machine_addressing_words_count; /* the number of machine words that are built from one assembly statement*/    

    char error_text[MAX_LENGTH_OF_ERROR_TEXT]; /* error text */

    int IC = 0; /* machine code instructions counter */ 
    int statement_valid = 1; /* valid flag for an assembly statement */
    int total_statements_valid = 1; /* valid flag for all the assembly statements */
    int directive_type; /* directive statement type*/
    int data[MAX_NUM_OF_ASSAEMNLY_STATEMENT_DATA]; /* machine data words */
    int data_count; /* machine data words count */
    int symbol_value_2 = 0; /* optional symbol address value of the first operand */
    int symbol_value_3 = 0; /* optional symbol adress value of the second operand */
    int symbol_value = 0; /* symbol address value */
    char coding_symbol_value[NUM_OF_CHAR_IN_CODING_WORD]; /* symbol address value after coding */
    int symbol_type_2 = 0;  /* symbol type of first operand */
    int symbol_type_3 = 0;  /* symbol type of second operand */
    int symbol_type = 0;    /* symbol type */
    int i = 0; /* index of the assemby statements */
    int j; /* index of ab assembly statement operands */

    *machine_instruction_words_ctr = 0;    /* number of machine instruction words */

    /* Get each assembly statement from the expanded macros file */
    while (fgets(assembly_statement, MAX_LENGTH_OF_ASSEMBLY_STATEMENT, expanded_macro_file) != NULL)
    {
        assembly_statement[strlen(assembly_statement) - 1] = '\0'; /* replace the newline character with '\0'*/
        error_text[0] = '\0';

        /* Handel directive assembly statement */
        if (assembly_statement_is_directive(assembly_statement))
        {
            statement_valid = assembly_directive_statement_to_machine_code(assembly_statement,
                                                        &directive_type, data, &data_count, symbol1, error_text);

            if (directive_type == ASSEMBLY_DIRECTIVE_STATEMENT_ENTRY_TYPE)
            {
                /* Try to resolve the symbol of the entry statement */
                if (get_symbol(symbol1) == NULL)
                {
                    strcpy(error_text, "Undefined symbol in entry directive statement.");
                    statement_valid = 0;
                }
                else
                {
                    /* Write the entry symbol and its value to to the entry symbols file */
                    symbol_value = get_symbol_value_from_table(symbol1, &symbol_type, error_text);
                    if (error_text[0] == '\0')
                    {
                        coding_machine_word(symbol_value, coding_symbol_value);
                        fprintf(entries_file, "%s\t%c%c\n", symbol1, coding_symbol_value[0], coding_symbol_value[1]);
                    }
                }
            }     
        }

        /* Handle undirective assembly statement */
        else 
        {
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
                /* Try to resolve a symbol in the first operand */
                if (statement_valid != 0 && symbol2[0] != '\0')
                {
                    symbol_value_2 = get_symbol_value_from_table(symbol2, &symbol_type_2, error_text);
                    if (error_text[0] != '\0')
                        statement_valid = 0;                    
                }

                /* Try to resolve a symbol in the second operand */
                if (statement_valid != 0 && symbol3[0] != '\0')
                {
                    symbol_value_3 = get_symbol_value_from_table(symbol3, &symbol_type_3, error_text);
                    if (error_text[0] != '\0')
                        statement_valid = 0;
                }

                if (statement_valid != 0)
                {
                    /* count the first machine word for an assembly instruction statement */
                    ++IC;

                    /* Get the next machine words for an assembly instruction statement */
                    if (operands_count == 2 &&
                        addressing_methods[0] == ADDRESSING_METHOD_REGISTER &&
                        addressing_methods[1] == ADDRESSING_METHOD_REGISTER)
                        ++IC; /* count the machine word for two registers operands */

                    else for (j = 0; j < MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS; j++)
                    {
                        if ((operands_count > 0 && j == 0) ||
                            (operands_count > 1 && j == 1))
                        {
                            machine_addressing_words_count = build_addressing_machine_words(
                                j,
                                addressing_methods[j], immediate_values[j],
                                machine_register_codes[j], struct_fields_indexes[j],
                                &machine_addressing_word_1, &machine_addressing_word_2);

                            /* Set the missing symbol value from the first pass */
                            if (addressing_methods[j] == ADDRESSING_METHOD_DIRECT ||
                                addressing_methods[j] == ADDRESSING_METHOD_STRUCT)
                            {
                                symbol_value = j == 0 ? symbol_value_2 : symbol_value_3;
                                machine_instruction_words[IC] |= symbol_value << MACHINE_WORD_ADDRESS_START_BIT;
 
                                /* Write the extern symbol and the adress code of the instruction that uses it */
                                symbol_type = j == 0 ? symbol_type_2 : symbol_type_3;
                                if (symbol_type == SYMBOL_EXTERN_TYPE)
                                {
                                    if (j == 0)
                                        strcpy(symbol, symbol2);
                                    else
                                        strcpy(symbol, symbol3);
                                    coding_machine_word(IC + MACHINE_CODE_FIRST_ADDRESS, coding_symbol_value);
                                    fprintf(externals_file, "%s\t%c%c\n", symbol, coding_symbol_value[0], coding_symbol_value[1]);
                                }

                                /* Set the external / relactoable direct addressing indication */
                                if (addressing_methods[j] == ADDRESSING_METHOD_DIRECT)
                                {
                                    if (symbol_type == SYMBOL_EXTERN_TYPE)
                                        machine_instruction_words[IC] |= A_R_E_CODING_EXTERNAL << MACHINE_WORD_A_R_E_START_BIT;
                                    else
                                        machine_instruction_words[IC] |= A_R_E_CODING_RELOCATABLE << MACHINE_WORD_A_R_E_START_BIT;
                                }

                                ++IC;
                            }
                            else
                                ++IC;    /* count the operand first machine word when it isn't a symbol */

                            if (machine_addressing_words_count > 1)
                                ++IC; /*  count the operand second machine word (for struct index only)  */            
                        }
                    }
                }
            }
        }

        /* Set the valid flag it ther is any error in the second pass*/
        if (statement_valid == 0)
        {
            total_statements_valid = 0;
            if (error_text[0] != '\0')
                print_error(error_text, i, assembly_statement);
        }

        /* Return the number of the machine instruction words */
        if (total_statements_valid != 0)
            *machine_instruction_words_ctr = IC;

        ++i; /* advance the index of assembly statements*/
    }

    return total_statements_valid;
}
