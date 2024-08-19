#define NUM_OF_CHAR_IN_CODING_WORD 2

/* Get a machine word.
   Convert it to two coding word.
   return the coding words */
void coding_machine_word(int machine_word, char* coding_word);

/* Get object file stream,
       machine instruction words and machine instruction words counter,
       machine data words and machine data words counter.
    Write to the object file coding words with length of machine instructions words and length of machine data words.
    Write to the object file coding words of machine instruction addresses and values,
    Write to the object file coding words of machine data addresses and values. */
void write_coding_machine_words(FILE* object_file,
    int* machine_instruction_words, int machine_instruction_words_ctr,
    int* machine_data_words, int machine_data_words_ctr);
