#define MAX_LENGTH_OF_SYMBOL_NAME 100

/* symbol definition straucture */
typedef struct symbol_definition
{
    int symbol_type; /* symbol type */
    char symbol_name[MAX_LENGTH_OF_SYMBOL_NAME]; /* symbol name */
    int symbol_value; /* symbol adrees value */
    struct symbol_definition* next_symbol; /* pointer to the next symbol definition in the linked list of symbols */
} symbol_definition;

/* Get a symbol name.
   Find the symbol in the linked list of symbols.
   Return a pointer to thge symbol parameters. */
symbol_definition* get_symbol(char* symbol_name);

/* Get symbol type,
    symbol name,
    symbol value.
    Allocate a memory for the symbol parametes,
    Link the symbol parameters to a linked list of symbols
    Return an optional error text */
void insert_symbol_to_table(int symbol_type, char* symbol_name, int symbol_value, char* error_text);

/*Get a symbol name.
Find the symbol in the linked list of symbols.
Return the symbol type,
the symbol address value,
and optional error text */
int get_symbol_value_from_table(char* symbol_name, int* symbol_type, char* error_text);

/* Get offset value,
       and symbol type.
   Add the offset to all the address values of the symbols of this type in the linked list of symbols */
void add_offset_to_symbols_values(int offset, int symbol_type);

/* Free all the allocated symbols in the linked list of symbols */
void free_symbols_Table();
