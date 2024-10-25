#ifndef _ASSEMBLER_FOR_MY_SPU_H__
#define _ASSEMBLER_FOR_MY_SPU_H__

#include "getoptions.h"
#include "spu.h"

struct asm_data {
    char* text;
    size_t text_size;
    asm_data_t* asm_code;
};
struct label{
    int ip;
    char* name;
};

const size_t n_labels = 16;

int prepare_to_translate(asm_data* data, streams_data* streams_info);

char* Compile_with_arg     (int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup);
char* translate_push_pop   (int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr);
char* translate_label_func (int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup);

char* handle_labels  (asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels);
char* get_label(char* curr);
int add_label(label* labels, char* label_name, int index);
int dump_labels(label* labels);

int fix_code(label* fixup, label* labels, asm_data_t* asm_code);
char* skip_until_space(char* curr);
char* skip_space  (char* curr, char* end);
char* skip_comment(char* curr, char* end);

int prepare_to_assemble(label** labels, label** fixup, asm_data_t* asm_code, size_t* asm_code_counter);
size_t assembler(char* curr, size_t text_size, asm_data_t* asm_code);

#endif
