#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "my_stack.h"
#include "processor.h"
#include "spu.h"

processor_data* proc_ctor()
{
    processor_data proc = {};
    proc.ip = 0;
    stack_init(proc.stack, 8, 4);
}

int check_compatibility(FILE* stream)
{
    int version = 0;
    int signature = 0;
    fscanf(stream, "%d", &version);
    fscanf(stream, "%d", &signature);
    if (version != CURRENT_VERSION)
    {
        fprintf(stderr, "INVALID VERSION\n");
        return 1;
    }
    if (signature != PROC_SIGNATURE)
    {
        fprintf(stderr, "INVALID SIGNATURE\n");
        return 2;
    }
    return 0;
}

int* make_cmd_array(processor_data* proc, FILE* stream)
{
    assert(proc);
    assert(stream);

    size_t num_of_cmds = 0;
    fscanf(stream, "%d", &num_of_cmds);
    proc -> cmd_array = (int*)calloc(2 * num_of_cmds, sizeof(int)); //
    size_t curr = 0;
    while (fscanf(stream, "%d", proc-> cmd_array + curr) != EOF)
    {
        curr++;
    }
    for (size_t i = 0; i <= curr; i++)
    {
        fprintf(stdout, "[%2d] = [%d]\n", i, proc -> cmd_array[i]);
    }
    return proc -> cmd_array;
}

int processor(processor_data* proc)
{
    assert(proc);

    while (proc -> cmd_array[proc -> ip] != HLT)
    {
        int cmd = proc -> cmd_array[proc -> ip] & CMD_MASK;
        printf("{cmd} = [%d]\n", cmd);
        switch (cmd)
        {
            case PUSH: {stack_push(&proc -> stack, get_push_arg(&proc));
                        break;}
            case POP:  {int arg = 0;
                        int reg = proc -> cmd_array[proc -> ip + 1];
                        stack_pop(&proc -> stack, &arg);
                        proc -> registers[reg] = arg;
                        proc -> ip++;
                        break;}
            case JMP:  {proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        proc.ip++;
                        break;}
            case JA:   {break;}

            case ELEM_IN: {int arg = 0;
                           fscanf(stdin, "%d", &arg);
                           stack_push(&proc -> stack, arg);
                           proc.ip++;
                           break;}
            case ELEM_OUT:{int arg = 0;
                           stack_pop(&proc -> stack, &arg);
                           printf("%d\n", arg);
                           proc.ip++;
                           break;}

            case ADD: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b + a);
                       proc.ip++;
                       break;}
            case SUB: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b - a);
                       proc.ip++;
                       break;}
            case MULT:{int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b * a);
                       proc.ip++;
                       break;}
            case DIV: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b / a);
                       proc.ip++;
                       break;}

            case SQRT:{int a = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_push(&proc -> stack, sqrt(a));
                       proc.ip++;
                       break;}
            case COS: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, cos(a));
                       proc.ip++;
                       break;}
            case SIN: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, sin(a));
                       proc.ip++;
                       break;}

            case DUMP:{STACK_DUMP(&proc -> stack, __func__);
                       proc.ip++;
                       break;}
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", cmd);
                     break;}
        }
    }
}

int get_push_arg(processor_data* proc)
{
    assert(proc);

    char arg_type = proc -> cmd_array[proc -> ip] & TYPE_MASK;
    proc -> ip++;
    int arg_value = 0;
    if (arg_type & NUM_ARG_MASK) { arg_value =  proc -> cmd_array[(proc -> ip)++];}
    if (arg_type & REG_ARG_MASK) { arg_value += proc -> registers[proc -> cmd_array[(proc -> ip)++]];}
    if (arg_type & MEM_ARG_MASK) { arg_value =  proc -> RAM[arg_value];}

    return arg_value;
}
