#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbols.h"

symbol_definition* first_symbol = NULL; /* the first symbol in a linked list of allocated symbols */
symbol_definition* last_symbol = NULL; /* the last symbol in a linked list of allocated symbols */


/* Get symbol type, 
       symbol name,
       symbol value.
    Allocate a memory for the symbol parametes,
    Link the symbol parameters to a linked list of symbols 
    Return an optional error text */
void insert_symbol_to_table(int symbol_type, char *symbol_name, int symbol_value, char* error_text)
{
    symbol_definition* symbol; /* a pointer to the symbol parameter */
    error_text[0] = '\0';

    symbol = get_symbol(symbol_name);

    /* check if the symbol is already exists in the linked list */
    if (symbol != NULL)
    {
        strcpy(error_text, "Symbol is already defined: \"");
        strcat(error_text, symbol_name);
        strcat(error_text, "\".");
    }


    else
    {
        /* Allocate a memory for the symbol parameters */
        symbol = malloc(sizeof(symbol_definition));
        if (symbol == NULL)
            strcpy(error_text, "Allocation error while trying to add a new symbol to table.");
        else
        {
            symbol->symbol_type = symbol_type;
            strcpy(symbol->symbol_name, symbol_name);
            symbol->symbol_value = symbol_value;
            
            /* Link the new symbol to the linked list */
            if (last_symbol != NULL)
                last_symbol->next_symbol = symbol;
            else
                first_symbol = symbol;
            symbol->next_symbol = NULL;
            last_symbol = symbol;
        }
    }
}

/* Get a symbol name.  
   Find the symbol in the linked list of symbols.
   Return the symbol type,
          the symbol address value,
          and optional error text */
int get_symbol_value_from_table(char* symbol_name, int* symbol_type, char* error_text)
{ 
    symbol_definition* symbol; /* a pointer to the symbol in the linked list of symbols */ 
    int symbol_value = 0; /* the symbol address value*/
    error_text[0] = '\0';

    /* Check if the symbol exists in the linked list of symbols */
    symbol = get_symbol(symbol_name);
    if (symbol == NULL)
    {
        strcpy(error_text, "Symbol is not found: \"");
        strcat(error_text, symbol_name);
        strcat(error_text, "\".");
    }

    /*  Get the symbol address value and the symbol type */
    else
    {
        symbol_value = symbol->symbol_value;
        *symbol_type = symbol->symbol_type;
    }

    return symbol_value;
}


/* Get a symbol name.
   Find the symbol in the linked list of symbols.
   Return a pointer to thge symbol parameters. */
symbol_definition* get_symbol(char* symbol_name)
{
    symbol_definition* found_symbol = NULL; /* a pointer to the fount symbol in the linked list*/
    symbol_definition* table_symbol = first_symbol; /* a pointer to the first symbol in the linked list*/

    /* Search the symbol parameters in the linked list of symbols according to its name */
    while (table_symbol != NULL && found_symbol == NULL)
    {
        if (strcmp(table_symbol->symbol_name, symbol_name) == 0)
            found_symbol = table_symbol;

        table_symbol = table_symbol->next_symbol;
    }

    /* Return a pointer to the found symbol parameters */
    return found_symbol;
}


/* Get an offset value,
       and a symbol type.
   Add the offset to all the address values of the symbols of this type in the linked list of symbols */
void add_offset_to_symbols_values(int offset, int symbol_type)
{
    symbol_definition* table_symbol = first_symbol; /* a pointer to symbol parameters in the linked lst of symbols*/

    /* Add the offset to all the address values of the symbols of this type in the linked list of symbols */
    while (table_symbol != NULL)
    {
        if (table_symbol->symbol_type == symbol_type)
            table_symbol->symbol_value += offset;
        table_symbol = table_symbol->next_symbol;
    }
}

/* Free all the allocated symbols in the linked list of symbols */
void free_symbols_Table()
{
    symbol_definition* table_symbol = first_symbol; /* a pointer to symbol parameters */
    symbol_definition* next_symbol; /* a pointer to the next symbol parameter in the linked list of symbols */

    /* free the memory allocation of the symbols in the linked list of symbols*/
    while (table_symbol != NULL)
    {
        next_symbol = table_symbol->next_symbol;

        free(table_symbol);

        table_symbol = next_symbol;
    }

    /* Reset the pointers to the first symbol and the last symbol in the linked list of symbols */
    first_symbol = NULL;
    last_symbol = NULL;
}

