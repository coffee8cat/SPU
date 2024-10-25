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

proc_data_t* make_cmd_array(processor_t* proc, FILE* stream)
{
    assert(proc);
    assert(stream);

    //printf("%d\n", proc -> code_size);
    fread(proc -> cmd_array, proc -> code_size, sizeof(int), stream);

    return proc -> cmd_array;
}

processor_t proc_ctor(size_t code_size)
{
    processor_t proc = {};
    proc.cmd_array = (proc_data_t*)calloc(code_size, sizeof(proc_data_t));
    if (proc.cmd_array == NULL)
    {
        fprintf(stderr, "CALLOC ERROR\n");
        return proc;
    }
    proc.ip = 0;
    proc.code_size = code_size;
    stack_init(&proc.data_stack, 8, sizeof(proc_data_t));
    stack_init(&proc.call_stack, 8, sizeof(proc_data_t));
    return proc;
}

int proc_dtor(processor_t* proc)
{
    assert(proc);
    memset(proc, 0, sizeof(processor_t));
    proc = NULL;
    return 0;
}

int proc_dump(processor_t* proc)
{
    printf("---PROCESSOR DUMP---\n\n");
    printf("\n---DATA STACK---\n");
    STACK_DUMP(&proc -> data_stack, __func__);
    printf("\n---CALL STACK---\n");
    STACK_DUMP(&proc -> call_stack, __func__);
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
    draw_RAM(proc);
    return 0;
}

int draw_RAM(processor_t* proc)
{
    assert(proc);
    for (size_t line = 0; line < draw_n_lines; line++)
    {
        for (size_t index = 0; index < draw_line_length; index++)
            {
                if(proc -> RAM[line * draw_line_length + index] == 0) //////////double???
                    printf(".");
                else
                    printf("*");
            }
        printf("\n");
    }

    return 0;
}

int processor(processor_t* proc)
{
    assert(proc);

    while (true)
    {
        int cmd = proc -> cmd_array[proc -> ip] & CMD_MASK;
        //printf("cmd:%d\n", cmd);
        switch (cmd)
        {
            case PUSH: {stack_push(&proc -> data_stack, get_push_arg(proc));
                        break;}
            case POP:  {stack_pop(&proc -> data_stack, get_pop_arg(proc));
                        break;}
            case JMP:  {proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        break;}
            case JA:   {proc_data_t a = 0;
                        proc_data_t b = 0;
                        stack_pop(&proc -> data_stack, &a);
                        stack_pop(&proc -> data_stack, &b);
                        if (b > a)
                            proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        else
                            proc -> ip = proc -> ip + 2;
                        break;}
            case CALL: {stack_push(&proc -> call_stack, proc -> ip + 2);
                        proc -> ip = proc -> cmd_array[proc -> ip + 1];
                        break;}
            case RTN:  {stack_pop(&proc -> call_stack, &proc -> ip);
                        break;}

            case DRAW: {draw_RAM(proc);
                        proc ->ip++;
                        break;}

            case ELEM_IN: {int arg = 0;
                           fscanf(stdin, "%d", &arg);
                           stack_push(&proc -> data_stack, arg);
                           proc -> ip++;
                           break;}
            case ELEM_OUT:{int arg = 0;
                           stack_pop(&proc -> data_stack, &arg);
                           printf("%d\n", arg);
                           proc -> ip++;
                           break;}

            case ADD: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> data_stack, &a);
                       stack_pop(&proc -> data_stack, &b);
                       stack_push(&proc -> data_stack, b + a);
                       proc -> ip++;
                       break;}
            case SUB: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> data_stack, &a);
                       stack_pop(&proc -> data_stack, &b);
                       stack_push(&proc -> data_stack, b - a);
                       proc -> ip++;
                       break;}
            case MULT:{int a = 0;
                       int b = 0;
                       stack_pop(&proc -> data_stack, &a);
                       stack_pop(&proc -> data_stack, &b);
                       stack_push(&proc -> data_stack, b * a);
                       proc -> ip++;
                       break;}
            case DIV: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> data_stack, &a);
                       stack_pop(&proc -> data_stack, &b);
                       stack_push(&proc -> data_stack, b / a);
                       proc -> ip++;
                       break;}

            case SQRT:{int a = 0;
                       stack_pop(&proc -> data_stack, &a);
                       stack_push(&proc -> data_stack, sqrt(a));
                       proc -> ip++;
                       break;}
            case COS: {int a = 0;
                       stack_pop (&proc -> data_stack, &a);
                       stack_push(&proc -> data_stack, cos(a));
                       proc -> ip++;
                       break;}
            case SIN: {int a = 0;
                       stack_pop (&proc -> data_stack, &a);
                       stack_push(&proc -> data_stack, sin(a));
                       proc -> ip++;
                       break;}

            case DUMP:{proc_dump(proc);
                       proc -> ip++;
                       break;}

            case HLT:{return 0;}
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", cmd);
                     break;}
        }
        /*proc_dump(proc);
        int a = 0;
        scanf("%d", &a);*/
    }
}

proc_data_t get_push_arg(processor_t* proc)
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

proc_data_t* get_pop_arg(processor_t* proc)
{
    assert(proc);

    char arg_type = proc -> cmd_array[proc -> ip] & TYPE_MASK;
    proc -> ip++;
    int arg_value = 0;
    if (arg_type & MEM_ARG_MASK)
    {
        if (arg_type & NUM_ARG_MASK){ arg_value =  proc -> cmd_array[(proc -> ip)++];}
        if (arg_type & REG_ARG_MASK){ arg_value += proc -> registers[proc -> cmd_array[(proc -> ip)++]];}
        return &proc -> RAM[arg_value];
    }
    else
    {
        if (arg_type & REG_ARG_MASK) { return &proc -> registers[proc -> cmd_array[(proc -> ip)++]];}
        else
        {
            fprintf(stderr, "ERROR: INVALID ARGUMENT FOR get_pop_arg: %d\n", arg_type);
            return NULL;
        }
    }
}
