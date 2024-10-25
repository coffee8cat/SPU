#ifndef _ASSEMBLER_FOR_MY_SPU_H__
#define _ASSEMBLER_FOR_MY_SPU_H__

struct label{
    int ip;
    char* name;
};
const size_t n_labels = 16;

char* translate_push_pop   (char cmd, int* asm_code, size_t* asm_code_counter, char* curr);
char* translate_label_func (char cmd, int* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup);

char* handle_labels  (int* asm_code, size_t* asm_code_counter, char* curr, label* labels);
char* get_label(char* curr);
int add_label(label* labels, char* label_name, int index);
int dump_labels(label* labels);

int fix_code(label* fixup, label* labels, int* asm_code);
char* skip_until_space(char* curr);
char* skip_space  (char* curr, char* end);
char* skip_comment(char* curr, char* end);

size_t assembler(char* curr, size_t text_size, int* asm_code);

#endif
