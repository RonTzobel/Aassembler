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

/* Files types extensions */
#define ASSEMBLY_FILE_EXTENSION ".as"
#define EXPANDED_MACROS_FILE_EXTENSION ".am"
#define OBJECT_FILE_EXTENSION ".ob"
#define ENTRIES_FILE_EXTENSION ".ent"
#define EXTERNALS_FILE_EXTENSION ".ext"

#define MAX_LENGTH_OF_FILE_NAME 50

FILE* open_file(char* file_name, char* mode);
void close_file(FILE* file);
void close_file_and_remove_if_empty(FILE* file, char* file_name);

/* This program gets input of assembly files names.
   It reads each assembly file and for each them it performs:
        macro expansion ( if there is any macro),
        first pass where the assembly statemants are converted to machine words without resolve of symbols,
        and second pass where the symbols are resolved. 
   For each assembly file four output files are created:
        the expanded macro file (".am"),
        the object file (".obj") with the machine adresses and machine words.
        The entries file (".ent") with internal addresses for symbols which are refrenced by external files,
        The external file (".ext") with internal adresses that reference to external symbols. 
    The macros are managed by a macros table which is a linked list of memory allocated macros definitions.
    The symbols are managed by a symbols table which is a linked list of memory allocated symbols definitions.
    The machine addresses and the machine words are represented in the output files with ascii characters which are coded
    from each 5 bits of 10 bits words.
    In each step the input assembly file is checked for errors. If errors are detected, proper messages are sent 
    to the standard output, and the process is stopped at the point it that it can't continue. */
int main(int argc, char* argv[])
{ 
    int machine_instruction_words[MAX_NUMBER_OF_MACHINE_WORDS]; /* machine instruction words converted from assembly statements */
    int machine_data_words[MAX_NUMBER_OF_MACHINE_WORDS];    /* machine data words converted from assembly statements */
    int machine_instruction_words_ctr; /* counter for machine instruction words */
    int machine_data_words_ctr; /* cu0nter for machine data words*/ 
    int valid = 1; /* valid indication flag */
    int files_index; /* index for input assembly files */

    char input_file_name[MAX_LENGTH_OF_FILE_NAME]; /* file name (without extension) from the program parameters */
    char assembly_file_name[MAX_LENGTH_OF_FILE_NAME]; /* assembly file name*/
    char expanded_macro_file_name[MAX_LENGTH_OF_FILE_NAME]; /* expanded macro file name*/
    char object_file_name[MAX_LENGTH_OF_FILE_NAME]; /* object file name */
    char entries_file_name[MAX_LENGTH_OF_FILE_NAME]; /* entries file name */
    char externals_file_name[MAX_LENGTH_OF_FILE_NAME]; /* externals file name*/

    FILE* assembly_file = NULL; /* assembly file stream */
    FILE* expanded_macros_file = NULL; /* expanded macro file stream */
    FILE* object_file = NULL; /* object file stream  */
    FILE* entries_file = NULL; /* entries file stream */
    FILE* externals_file = NULL;/* externals file stream */

    if (argc < 2)
        printf("No assembly file in the program arguments");

    /* Get list of file names from the program parameters */ 
    else for (files_index = 1; files_index < argc; files_index++)
    {
       /* Get each input assembly file name 
         and set the proper extensions to the input file and the outpu files*/
        strcpy(input_file_name, argv[files_index]);
        strcpy(assembly_file_name, strcat(input_file_name, ASSEMBLY_FILE_EXTENSION));
        strcpy(input_file_name, argv[files_index]);
        strcpy(expanded_macro_file_name, strcat(input_file_name, EXPANDED_MACROS_FILE_EXTENSION));
        strcpy(input_file_name, argv[files_index]);
        strcpy(object_file_name, strcat(input_file_name, OBJECT_FILE_EXTENSION));
        strcpy(input_file_name, argv[files_index]);
        strcpy(entries_file_name, strcat(input_file_name, ENTRIES_FILE_EXTENSION));
        strcpy(input_file_name, argv[files_index]);
        strcpy(externals_file_name, strcat(input_file_name, EXTERNALS_FILE_EXTENSION));

   	printf("Assembler execution of file %s\n", assembly_file_name);    

        /* Open the assembly source file for reading 
           and the macro expanded file for writing */
        if ((assembly_file = open_file(assembly_file_name, "r")) == NULL ||
            (expanded_macros_file = open_file(expanded_macro_file_name, "w")) == NULL)
            valid = 0;

        /* Read the source assembly file
           and write the assembly statements with macros expansions to the expanded macros file. */
        else
            valid = expand_macros(assembly_file, expanded_macros_file);

        /* Close the files that are involved in the macro expanansions operation */
        close_file(assembly_file);
        close_file(expanded_macros_file);

        /* Read the macro expanded file and perform assembly first pass */
        if (valid)
        {
            if ((expanded_macros_file = open_file(expanded_macro_file_name, "r")) == NULL)
                valid = 0;
            else
                valid = first_pass(expanded_macros_file,
                    machine_instruction_words, machine_data_words,
                    &machine_instruction_words_ctr, &machine_data_words_ctr);
            close_file(expanded_macros_file);
        }

        /* Read the macro expanded file and perform assembly second pass
         while writing entry symbols to the entries file and external symbols to the external file*/
        if (valid)
        {
            if ((expanded_macros_file = open_file(expanded_macro_file_name, "r")) == NULL ||
                (entries_file = open_file(entries_file_name, "w")) == NULL ||
                (externals_file = fopen(externals_file_name, "w")) == NULL)
                valid = 0;
            else
                valid = second_pass(expanded_macros_file, entries_file, externals_file,
                    machine_instruction_words, &machine_instruction_words_ctr);

            close_file(expanded_macros_file);
            close_file_and_remove_if_empty(entries_file, entries_file_name);
            close_file_and_remove_if_empty(externals_file, externals_file_name);
	}

        /* Free the symbols table after using of the second pass function for resolving symbols */
        free_symbols_Table();

        /* Write the coding machine words to the object file */
        if (valid && (object_file = open_file(object_file_name, "w")) != NULL)
        {
            write_coding_machine_words(object_file,
                machine_instruction_words, machine_instruction_words_ctr,
                machine_data_words, machine_data_words_ctr);

            close_file(object_file);
        }
 
    }

    return 0;	
}

/* Get a file name and the mode for opening it - "r" for read, "w" for write.
   Try to open the file.
   Return the file stream or the error text if the open is failed. */
FILE* open_file(char* file_name, char* mode)
{
    FILE* file; /* file stream */
    char reading_writing[10]; /* file open mode. "r" for read, "w" for write */

    if ((file = fopen(file_name, mode)) == NULL) /* try to open the file */
    {
        /* Return error text for file open failure */
        if (strcmp(mode, "r"))
            strcpy(reading_writing, "reading"); 
        else
            strcpy(reading_writing, "writing");
        printf("Can't open file %s for %s", file_name, reading_writing);
    }

    return file;
}

/* Get a file stream.
   Close the file if the stream is not null (the file is open) */
void close_file(FILE* file)
{
    if (file != NULL)
        fclose(file);
}

/* Get a file stream.
   Close the file if the stream is not null (the file is open).
   Remove the file if it is empty */
void close_file_and_remove_if_empty(FILE* file, char* file_name)
{
    int file_size;

    if (file != NULL)
    {
        /* Get the file size */
        fseek(file, 0L, SEEK_END);
        file_size = ftell(file);

        fclose(file);

        /* Remove the file if it is empty */
        if (file_size == 0)
            remove(file_name);
    }
}


