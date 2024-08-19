#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"
#include "assembly_to_machine.h"
#include "validation.h"

#define  MAX_LENGTH_OF_LABEL 30

/* Get a string of characters
   Check if the string represents a valid integer number 
   Return valid flag: 1 - the string represents a valid integer number 0- the string doesn'represnt a valid number.
          optional error text */
int validate_integer_number(char* integer_number_string, char* error_text)
{
    int valid = 1; /* valid indication flag*/
    unsigned int i; /* index of characters in the string */

    /* Empty string */
    if (strlen(integer_number_string) < 1)
        valid = 0;

    /* First character is not a sign or a digit */
    else if (!isdigit(integer_number_string[0]) &&
        integer_number_string[0] != '+' &&
        integer_number_string[0] != '-')
    {
        strcpy(error_text, "Invalid non-sign and non-digit character in integer number.");
        valid = 0;
    }

    /* Sign only */
    else if ((integer_number_string[0] == '+' ||
        integer_number_string[0] == '-') &&
        strlen(integer_number_string) < 2)
    {
        strcpy(error_text, "Invalid integer number. Contains only sign.");
        valid = 0;
    }

    /* Non-digit character */
    else for (i = 1; i < strlen(integer_number_string) && valid; i++)
        if (!isdigit(integer_number_string[i]))
        {
            strcpy(error_text, "Invalid non-digit character in integer number.");
            valid = 0;
        }
    
    return valid;
}

/* Get an array of caharcters
   Check if the array represents valid string with quotation marks at the atartand the end
   Return valid flag: 1 - the array represents a valid string  0- the string doesn'represnt a valid string.
          optional error text */
int validate_string(char* string, char *error_text)
{
    int valid = 1; /* valid indication flag*/

    /* String length error */
    if (strlen(string) < 3)
    {
        strcpy(error_text, "Invalid length of string.");
        valid = 0;
    }
    else if (string[0] != '\"' || string[strlen(string) - 1] != '\"')
    {
 	strcpy(error_text, "Invalid string. Missing quotation marks.");
        valid = 0;
    }

    return valid;
}


/* Get string of characters that represents list of items seperated by commas after a sepecified n  umber of items separated by blanks,
       number of first items to be seperated by blanks.
   Check that there is one comma (additional blanks and tabs are allowed) between each two items in a string
   after a sepecified number of items separated by blanks.
   Return valid flag: 1- the string represents a valid list of items 0- the string doesn't represent a valid. number of items.
          optional error text */
int validate_commas(char* list_of_items, int num_of_first_items_with_seperated_blanks, char* error_text)
{
    int valid = 1; /* valid indication flag */
    int comma_required = 0; /* indication flag that a comma is required after an item in the string*/
    int items_ctr = 0; /* counter for items in the string */
    int item = 0; /* indication flag for item in the string */
    unsigned int i; /* index for characters in the string */

    for (i = 0; i < strlen(list_of_items) && valid; i++)
    {
        if (list_of_items[i] == ',')
        {
            /* Extra comma exists in the string where it is not required */
            if (!comma_required || i == strlen(list_of_items) - 1)
            {
                strcpy(error_text, "Invalid extra comma.");
                valid = 0;
            }
            else
                comma_required = 0;
            item = 0;
        }
        else
        {
            if (list_of_items[i] != ' ' &&
                list_of_items[i] != '\t')
            {
                /* Comma doesn't exist in the string where it is required */
                if (comma_required)
                {
                    strcpy(error_text, "Missing required comma.");
                    valid = 0;
                }
                else if (!item)
                {
                    ++items_ctr;
                    item = 1;
                }

                /* Check if comma is required after the current position in the string */
                if (items_ctr > num_of_first_items_with_seperated_blanks &&
                    i < strlen(list_of_items) - 1 &&
                    (list_of_items[i + 1] == ' ' ||
                        list_of_items[i + 1] == '\t' ||
                        list_of_items[i + 1] == ','))
                    comma_required = 1;
            }
            else
                item = 0;
        }
    }   

    return valid;
}

/* Get a string of characters
   Check if the string represents a valid label
   Return valid flag: 1 - the string represents a valid integer label 0- the string doesn'represnt a valid label.
          optional error text */
int validate_label(char* label, char *error_text)
{
    int valid = 1; /* valid indication flag */
    char label_without_colon[MAX_LENGTH_OF_ASSEMBLY_ARGUMENT]; /* the label without colon at the end */
    unsigned int i; /* index of chracters in the label*/

    /* Check if the length of the label is valid */
    if (strlen(label) < 2 || strlen(label) - 1 > MAX_LENGTH_OF_LABEL)
    {
        strcpy(error_text, "invalid length of label.");
        valid = 0;
    }

    /* Check if first character of the label is valid */
    else if (!isalpha(label[0]) || label[strlen(label) - 1] != ':')
    {
        strcpy(error_text, "invalid first character of label. Must be alphabetic.");
        valid = 0;
    }

    /* Check if the lable doesn't end with colon */
    else if (label[strlen(label) - 1] != ':')
    {
        strcpy(error_text, "invalid last character of label. must be ':'.");
        valid = 0;
    }

    /* Check if the label contains non-alphanuemeric character*/
    else for (i = 1; i < strlen(label) - 1 && valid; i++)
        if (!isalnum(label[i]))
        {
            strcpy(error_text, "invalid non-alphanumeic character in label.");
            valid = 0;
        }

    /* Check that the label is not the same as of an assembly saved word */
    if (valid)
    {
        strcpy(label_without_colon, label);
        label_without_colon[strlen(label_without_colon) - 1] = '\0';
        if (get_machine_operation_code(label_without_colon) != UNKNOWN_OPCODE ||
            get_directive_type(label_without_colon) != ASSEMBLY_DIRECTIVE_STATEMENT_UNKNOWN_TYPE ||
            get_machine_register_code(label_without_colon) != UKNOWN_REGISTER)
        {
            strcpy(error_text, "invalid label. Identical to saved word.");
            valid = 0;
        }
    }

    return valid;
}

/* Get error text
       index of the assembly statement
       assembly statement 
    Print the error details with a tabular from*/
void print_error(char* error_text, int assembly_statement_index, char* assembly_statement)
{
    char tabs[10]; /* tab characters to seperate between the fields of the error text */

    if (strlen(error_text) < 25)
        strcpy(tabs, "\t\t\t\t\t\t");
    else if (strlen(error_text) < 30)
        strcpy(tabs, "\t\t\t\t\t");
    else if (strlen(error_text) < 40)
        strcpy(tabs, "\t\t\t\t");
    else if (strlen(error_text) < 50)
        strcpy(tabs, "\t\t\t");
    else if (strlen(error_text) < 55)
        strcpy(tabs, "\t\t");
    else
        strcpy(tabs, "\t");

    printf(" %s%sLine: %2d  Statement: \"%s\"\n", error_text, tabs, assembly_statement_index+1, assembly_statement);
}
