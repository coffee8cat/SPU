#ifndef _ASSEMBLER_FOR_MY_SPU_H__
#define _ASSEMBLER_FOR_MY_SPU_H__

struct label{
    size_t ip;
    char* name;
}

int check_compatibility(FILE* stream, int* asm_code);
int translate_push (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_in   (int* asm_code, size_t* asm_code_counter, char** array);
int translate_pop  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_JMP  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_JA   (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int handle_labels  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int assembler(char** array, int* asm_code, size_t num_of_cmds);

#endif
