#define MAX_LENGTH_OF_ERROR_TEXT  200

/* Get error text
       index of the assembly statement
       assembly statement
    Print the error details with a tabular from*/
void print_error(char* error_text, int assembly_statement_index, char* assembly_statement);

/* Get a string of characters
   Check if the string represents a valid label
   Return valid flag: 1 - the string represents a valid integer label 0- the string doesn'represnt a valid label.
          optional error text */
int validate_label(char* label, char *error_text);

/* Get a string of characters
   Check if the string represents a valid integer number
   Return valid flag: 1 - the string represents a valid integer number 0- the string doesn'represnt a valid number.
          optional error text */
int validate_integer_number(char* integer_number_string, char* error_text);

/* Get an array of caharcters
   Check if the array represents valid string with quotation marks at the atartand the end
   Return valid flag: 1 - the array represents a valid string  0- the string doesn'represnt a valid string.
          optional error text */
int validate_string(char* string, char* error_text);

/* Get string of characters that represents list of items seperated by commas after a sepecified n  umber of items separated by blanks,
       number of first items to be seperated by blanks.
   Check that there is one comma (additional blanks and tabs are allowed) between each two items in a string
   after a sepecified number of items separated by blanks.
   Return valid flag: 1- the string represents a valid list of items 0- the string doesn't represent a valid. number of items.
          optional error text */
int validate_commas(char* list_of_items, int num_of_first_items_with_seperated_blanks, char* error_text);
