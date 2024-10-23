#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "my_stack.h"
#include "spu.h"

struct processor_t
{
    proc_data_t* cmd_array;
    proc_data_t RAM[100];
    proc_data_t registers[8];
    size_t ip;
    size_t code_size;
    stack_t data_stack;
    stack_t call_stack;
};

const size_t draw_n_lines     = 10;
const size_t draw_line_length = 10;

int check_compatibility(FILE* stream);
int* make_cmd_array(processor_t* proc, FILE* stream);
processor_t proc_ctor(size_t code_size);
int proc_dump(processor_t* proc);
int draw_RAM(processor_t* proc);
int processor(processor_t* proc);

int get_push_arg(processor_t* proc);
int* get_pop_arg(processor_t* proc);

#endif
