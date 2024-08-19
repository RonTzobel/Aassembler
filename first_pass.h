/* Get the file stream after macro expansion(the ".am" file).
Perfom the first pass - converting the assembly statements to machine words without resolving symbols.
Return the machine instruction words and the instruction words count
and the machine data words and the machine data words count
and a valid flag for the first pass */
int first_pass(FILE* expanded_macro_file, 
    int* machine_instruction_words, int* machine_data_words,
    int* machine_instruction_words_ctr, int* machine_data_words_ctr);
