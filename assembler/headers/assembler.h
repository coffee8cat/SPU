#ifndef _ASSEMBLER_FOR_MY_SPU_H__
#define _ASSEMBLER_FOR_MY_SPU_H__

struct label{
    int ip;
    char* name;
};
const size_t n_labels = 16;


int check_compatibility(FILE* stream, int* asm_code);

int translate_push (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_in   (int* asm_code, size_t* asm_code_counter, char** array);
int translate_pop  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_JMP  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels, label* fixup);
int translate_JA   (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);
int translate_CALL (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd);

int handle_labels  (int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels);
char* get_label(char** array, size_t curr_cmd);
int add_label(label* labels, char* label_name, int index);
int dump_labels(label* labels);

int fix_code(label* fixup, label* labels, int* asm_code);
int assembler(char** array, int* asm_code, size_t num_of_cmds);

#endif
