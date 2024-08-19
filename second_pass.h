/* Get the expanded macros file stream,
       the entries file stream,
       the externals file stream,
       and the machine instructions words and the machine instruction words count that were built in the first pass,
   Read the expandes macro file and resolve the unresolved symbols from the first pass.
   Write entries addresses to the entries file.
   Write external addresses to the externals file.
   Return the machine instructions words with the resolved symbols and a valid flag for the second pass.  */
int second_pass(FILE* expanded_macro_file, FILE* entries_file, FILE* externals_file,
    int* machine_instruction_words, int* machine_instruction_words_ctr);
