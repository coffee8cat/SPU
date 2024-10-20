#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "my_stack.h"
#include "processor.h"
#include "spu.h"

int check_compatibility(FILE* stream)
{
    int version = 0;
    int signature = 0;
    fread(&version, 1, sizeof(int), stream);
    fread(&signature, 1, sizeof(int), stream);
    printf("v: %d; sgn: %d\n", version, signature);
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

    printf("%d\n", proc -> code_size);
    fread(proc -> cmd_array, proc -> code_size, sizeof(int), stream);

    return proc -> cmd_array;
}

processor_data proc_ctor(size_t code_size)
{
    processor_data proc = {};
    proc.cmd_array = (int*)calloc(code_size, sizeof(int));
    proc.registers = (int*)calloc(8, sizeof(int));
    proc.ip = 0;
    proc.code_size = code_size;
    stack_init(&proc.stack, 8, 4);
    return proc;
}

int proc_dump(processor_data* proc)
{
    printf("---PROCESSOR DUMP---\n\n");
    STACK_DUMP(&proc -> stack, __func__);
    printf("ip: %d\n", proc -> ip);
    printf("CMD ARRAY:\n");
    for (size_t i = 0; i < proc->code_size; i++)
    {
        printf("%4d ", i);
    }
    printf("\n");
    for (size_t i = 0; i < proc->code_size; i++)
    {
        printf("%4d ", proc->cmd_array[i]);
    }
    printf("\nREGISTERS\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("%d: %d\n", i, proc->registers[i]);
    }
    return 0;
}

int processor(processor_data* proc)
{
    assert(proc);

    while (true)
    {
        int cmd = proc -> cmd_array[proc -> ip] & CMD_MASK;
        //printf("{cmd} = [%d]\n", cmd);
        switch (cmd)
        {
            case PUSH: {stack_push(&proc -> stack, get_push_arg(proc));
                        break;}
            case POP:  {int arg = 0;
                        int reg = proc -> cmd_array[proc -> ip + 1];
                        stack_pop(&proc -> stack, &arg);
                        *get_pop_arg(proc) = arg;
                        break;}
            case JMP:  {proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        break;}
            case JA:   {int a = 0;
                        int b = 0;
                        stack_pop(&proc -> stack, &a);
                        stack_pop(&proc -> stack, &b);
                        //printf("a = %d, b = %d\n", a, b);

                        if (b > a)
                            proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        else
                            proc -> ip = proc -> ip + 2;

                        //stack_push(&proc -> stack, b);
                        //stack_push(&proc -> stack, a);
                        break;}

            case ELEM_IN: {int arg = 0;
                           fscanf(stdin, "%d", &arg);
                           //stack_push(&proc -> stack, arg);
                           proc -> ip++;
                           break;}
            case ELEM_OUT:{int arg = 0;
                           stack_pop(&proc -> stack, &arg);
                           printf("%d\n", arg);
                           proc -> ip++;
                           break;}

            case ADD: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b + a);
                       //printf("ADDING\n");
                       //STACK_DUMP(&proc->stack, __func__);
                       proc -> ip++;
                       break;}
            case SUB: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b - a);
                       proc -> ip++;
                       break;}
            case MULT:{int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b * a);
                       proc -> ip++;
                       break;}
            case DIV: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b / a);
                       proc -> ip++;
                       break;}

            case SQRT:{int a = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_push(&proc -> stack, sqrt(a));
                       proc -> ip++;
                       break;}
            case COS: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, cos(a));
                       proc -> ip++;
                       break;}
            case SIN: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, sin(a));
                       proc -> ip++;
                       break;}

            case DUMP:{STACK_DUMP(&proc -> stack, __func__);
                       proc -> ip++;
                       break;}

            case HLT:{return 0;}
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", cmd);
                     break;}
        }
        //proc_dump(proc);
        int a = 0;
        //scanf("%d", &a);
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

int* get_pop_arg(processor_data* proc)
{
    assert(proc);

    char arg_type = proc -> cmd_array[proc -> ip] & TYPE_MASK;
    proc -> ip++;
    int* arg_value = NULL;
    //if (arg_type & NUM_ARG_MASK) { arg_value = &proc -> cmd_array[(proc -> ip)++];}
    if (arg_type & REG_ARG_MASK) { arg_value = &proc -> registers[proc -> cmd_array[(proc -> ip)++]];}
    //if (arg_type & MEM_ARG_MASK) { arg_value = &proc -> RAM[*arg_value];}

    return arg_value;
}
