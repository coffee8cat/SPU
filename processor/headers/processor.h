#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "my_stack.h"

struct processor_data
{
    int* cmd_array;
    size_t ip;
    stack_t stack;
    int* registers;
};

#define CURRENT_VERSION 2
#define PROC_SIGNATURE 4341

int check_compatibility(FILE* stream);
int* make_cmd_array(processor_data* proc, FILE* stream);
int processor(processor_data* proc);

#endif
