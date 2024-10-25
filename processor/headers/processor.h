#ifndef _PROCESSOR_H__
#define _PROCESSOR_H__

#include "my_stack.h"
#include "spu.h"

const size_t default_RAM_SIZE = 200;
const size_t REGS_NUMBER = 8;

struct processor_t
{
    asm_data_t* cmd_array;
    proc_data_t RAM[default_RAM_SIZE];
    proc_data_t registers[REGS_NUMBER];
    size_t ip;
    size_t code_size;
    stack_t data_stack;
    stack_t call_stack;
};

const size_t draw_n_lines     = 10;
const size_t draw_line_length = 20;

int check_compatibility(FILE* stream);
asm_data_t* make_cmd_array(processor_t* proc, FILE* stream);
processor_t proc_ctor(size_t code_size);
int proc_dtor(processor_t* proc);
int proc_dump(processor_t* proc);
int draw_RAM(processor_t* proc);
int execute_cmds(processor_t* proc);

proc_data_t get_push_arg(processor_t* proc);
proc_data_t* get_pop_arg(processor_t* proc);

#endif
