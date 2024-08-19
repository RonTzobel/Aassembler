/* Sizes related to assemly statements and machine words */
#define MAX_LENGTH_OF_ASSEMBLY_STATEMENT 81
#define MAX_NUMBER_OF_ASSEMBLY_STATEMENTS 40
#define MAX_NUMBER_OF_MACHINE_WORDS 256
#define MAX_NUM_OF_ASSEMBLY_STATEMENT_OPERANDS 2

/* Sizes related to assembly statement arguments */
#define MAX_NUM_OF_ASSEMBLY_STATEMENT_ARGUMENTS  50
#define MAX_LENGTH_OF_ASSEMBLY_ARGUMENT  40

/* Addressing method types */
#define ADDRESSING_METHOD_IMMEDIATE  0
#define ADDRESSING_METHOD_DIRECT     1
#define ADDRESSING_METHOD_STRUCT     2
#define ADDRESSING_METHOD_REGISTER   3
#define UNKNOWN_ADDRESSING_METHOD   -1
#define NUM_OF_ADDRESING_METHODS     4

#define MACHINE_CODE_FIRST_ADDRESS  100

/* Positions of fields in the machine word bits */
#define MACHINE_WORD_OPCODE_START_BIT 6
#define MACHINE_WORD_SRC_ADDRESSING_METHOD_START_BIT 4
#define MACHINE_WORD_DEST_ADDRESSING_METHOD_START_BIT 2
#define MACHINE_WORD_A_R_E_START_BIT 0
#define MACHINE_WORD_ADDRESS_START_BIT 2
#define MACHINE_WORD_SRC_REGISTER_START_BIT 6
#define MACHINE_WORD_DEST_REGISTER_START_BIT 2

/* A R E  coding types */
#define A_R_E_CODING_ABSOLUTE     0
#define A_R_E_CODING_EXTERNAL     1
#define A_R_E_CODING_RELOCATABLE  2

/* Machine registers codes */
#define r0  0
#define r1  1
#define r2  2
#define r3  3
#define r4  4
#define r5  5
#define r6  6
#define r7  7
#define UKNOWN_REGISTER  -1

#define MAX_NUM_OF_ASSAEMNLY_STATEMENT_DATA 50

/* Assembly statement types */
#define ASSEMBLY_STATEMENT_INSTRUCTION_TYPE 0
#define ASSEMBLY_STATEMENT_DIRECTIVE_TYPE  0

/* Assembly directive statements types */
#define ASSEMBLY_DIRECTIVE_STATEMENT_DATA_TYPE    0
#define ASSEMBLY_DIRECTIVE_STATEMENT_STRUCT_TYPE  1
#define ASSEMBLY_DIRECTIVE_STATEMENT_STRING_TYPE  2
#define ASSEMBLY_DIRECTIVE_STATEMENT_ENTRY_TYPE   3
#define ASSEMBLY_DIRECTIVE_STATEMENT_EXTERN_TYPE  4
#define ASSEMBLY_DIRECTIVE_STATEMENT_UNKNOWN_TYPE -1

/* Assembly symbols types */
#define SYMBOL_CODE_TYPE 0
#define SYMBOL_DATA_TYPE 1
#define SYMBOL_EXTERN_TYPE 2
#define SYMBOL_UNKNOWN_TYPE -1

#define UNKNOWN_OPCODE -1

/* Get an assembly statement.
   Check its fields.
   Return its arguments
          the index of the first argument (1- if the statement has a label, 0- if thers isn't a label)
          optional error text */
int get_assembly_statement_arguments(char* assembly_statement,
    char assembly_arguments[][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT],
    int* first_argument_index_not_Include_label,
    char* error_text);

/* Get the assembly operation argument.
   Convert it to machine operation code.
   Return the machine operation code*/
int get_machine_operation_code(char* assembly_operation);

/* Get a directive statement name (started with '.' character)
   check the name.
   Return the proper directive type*/
int get_directive_type(char* directive_name);

/* Get an assembly statement operand
   Check if it has an immediate value
   Return optional immediate value,
          optional error text,
          an indication flag that the operand has an immediate value */
int get_assembly_immediate_value(char* assembly_operand, int* immediate_value, char* error_text);

/* Get assembly statement operand.
   Check if it has aregister name
   Return the register machine code */
int get_machine_register_code(char* assembly_operand);

/* Get an assembly statement operand
   Check if it has structure parameters
   Return the structure name,
          the structure fields index,
          an indication flag that the operand has structure parameters */
int get_assembly_struct_parameters(char* assembly_operand, char* struct_name, int* struct_field_index);

/* Get an assembly statement operand
   Check if it has an address name
   Return the optional address name,
          an indication flag that the operand has an address name */
int get_assembly_address_name(char* assembly_operand, char* address_name);

/* Get an assembly statement label
   Remove the ':' character from the end of the label.
   Return the label synbol without the ':' character. */
void get_symbol_from_label(char* label, char* symbol);

/* Get an assembly statement
   Check if it is a directive statement
   return: 1-directive statement 0-undirective statement*/
int assembly_statement_is_directive(char* assembly_statement);


/* Get an assembly directive statement
   check its arguments 
   Return the directive type,
          the data values in the assembly directive statement,
          the number of data values in the assembly directive statement,
          the symbol of the assembly entry or extern directive statement
          the optional error text */
int assembly_directive_statement_to_machine_code(char* assembly_statement, int* directive_type,
                                                      int* data, int* data_count, char* symbol, char *error_text);

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
    int* immediate_values, int* machine_registercodes,
    char address_names[][MAX_LENGTH_OF_ASSEMBLY_ARGUMENT], int* struct_fields_indexes,
    char* symbol1, char* symbol2, char* symbol3,
    char *error_text);

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
    char* address_name, int* struct_field_index,
    char* symbol, char* error_text);

/* Get machine registers codes of an assembly statement with two registers operands. 
   Build and return one machine words with the registers codes positioned in the proper places */
int build_two_registers_addressing_machine_words(int machine_register_code_1, int machine_register_code_2);

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
    int* addressing_word_1, int* addressing_word_2);


