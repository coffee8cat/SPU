#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "my_stack.h"

struct processor_data
{
    int* cmd_array;
    int* RAM;
    int* registers;
    size_t ip;
    size_t code_size;
    stack_t data_stack;
    stack_t call_stack;
};

int check_compatibility(FILE* stream);
int* make_cmd_array(processor_data* proc, FILE* stream);
processor_data proc_ctor(size_t code_size);
int proc_dump(processor_data* proc);
int processor(processor_data* proc);

int get_push_arg(processor_data* proc);
int* get_pop_arg(processor_data* proc);

#endif
