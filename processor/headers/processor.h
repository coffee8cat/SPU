#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "my_stack.h"

struct processor_data
{
    int* cmd_array;
    int* RAM;
    int* registers;
    size_t ip;
    stack_t stack;
};

int check_compatibility(FILE* stream);
int* make_cmd_array(processor_data* proc, FILE* stream);
int processor(processor_data* proc);

int get_push_arg(processor_data* proc);

#endif
