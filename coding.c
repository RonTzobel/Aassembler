#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "coding.h"

/* parameters for coding 5 bits value to an ascii character*/
#define NUM_OF_BITS_FOR_CODING_TO_ONE_CHAR 5
#define MASK_5_BITS 0x1F                       


/* A table for coding 5 bits value to an ascii character 
   the index of each character represents the coded 5 bits value (from 0 to 31) */
char coding_chars[] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>',
                       'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                       'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
                       'u', 'v' };

/* Get a machine word. 
   Convert it to two coding word. 
   return the coding words */
void coding_machine_word(int machine_word, char *coding_word)
{
    coding_word[0] = coding_chars[((machine_word >> NUM_OF_BITS_FOR_CODING_TO_ONE_CHAR)) & MASK_5_BITS];
    coding_word[1] = coding_chars[machine_word & MASK_5_BITS]; 
}

/* Get object file stream,
       machine instruction words and machine instruction words counter,
       machine data words and machine data words counter.
    Write to the object file coding words with length of machine instructions words and length of machine data words.
    Write to the object file coding words of machine instruction addresses and values,
    Write to the object file coding words of machine data addresses and values. */
void write_coding_machine_words(FILE* object_file,
    int* machine_instruction_words, int machine_instruction_words_ctr,
    int* machine_data_words, int machine_data_words_ctr)
{
    char coding_machine_instruction_words_length[NUM_OF_CHAR_IN_CODING_WORD]; /* coding words for length of machine instruction words */
    char coding_machine_data_words_length[NUM_OF_CHAR_IN_CODING_WORD]; /* coding words for length of machine data words */
    char coding_words[MAX_NUMBER_OF_MACHINE_WORDS][NUM_OF_CHAR_IN_CODING_WORD]; /* coding words */
    char coding_address_words[MAX_NUMBER_OF_MACHINE_WORDS][NUM_OF_CHAR_IN_CODING_WORD]; /* coding address words*/
    int i, j, k; /* indexs for machine instruction word, machine data words, and coding words */

    coding_machine_word(machine_instruction_words_ctr, coding_machine_instruction_words_length);
    coding_machine_word(machine_data_words_ctr, coding_machine_data_words_length);

    /* Write to the object file coding words with length of machine instructions words and length of machine data words */
    fprintf(object_file, " ");
    if (coding_machine_instruction_words_length[0] != '!')
        fprintf(object_file, "%c", coding_machine_instruction_words_length[0]);
    fprintf(object_file, "%c ", coding_machine_instruction_words_length[1]);
    if (coding_machine_data_words_length[0] != '!')
        fprintf(object_file, "%c", coding_machine_data_words_length[0]);
    fprintf(object_file, "%c\n", coding_machine_data_words_length[1]);

    /* Write to the object file coding words of machine addresses of instructions and  machine instruction words */
    k = 0;
    for (i = 0; i < machine_instruction_words_ctr; i++)
    {
        coding_machine_word(k + MACHINE_CODE_FIRST_ADDRESS, coding_address_words[k]);
        coding_machine_word(machine_instruction_words[i], coding_words[k++]);
        fprintf(object_file, "%c%c  %c%c\n", coding_address_words[k - 1][0], coding_address_words[k - 1][1],
            coding_words[k - 1][0], coding_words[k - 1][1]);
    }

    /* Write to the object file coding words of machine addresses of data and machine data words */
    for (j = 0; j < machine_data_words_ctr; j++)
    {
        coding_machine_word(k + MACHINE_CODE_FIRST_ADDRESS, coding_address_words[k]);
        coding_machine_word(machine_data_words[j], coding_words[k++]);
        fprintf(object_file, "%c%c  %c%c\n", coding_address_words[k - 1][0], coding_address_words[k - 1][1],
            coding_words[k - 1][0], coding_words[k - 1][1]);
    }
}
