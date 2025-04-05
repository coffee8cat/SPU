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
    assert(stream);

    asm_data_t version = 0;
    asm_data_t signature = 0;
    fread(&version,   1, sizeof(asm_data_t), stream);
    fread(&signature, 1, sizeof(asm_data_t), stream);
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

asm_data_t* make_cmd_array(processor_t* proc, FILE* stream)
{
    assert(proc);
    assert(stream);

    //printf("code size: %d\n",  proc -> code_size);

    fread(proc -> cmd_array, (size_t)proc -> code_size, sizeof(asm_data_t), stream);
    //for (size_t i = 0; i < proc -> code_size; i++)
    //    printf("%lf\n", proc -> cmd_array[i]);
    return proc -> cmd_array;
}

processor_t proc_ctor(size_t code_size)
{
    processor_t proc = {};
    proc.cmd_array = (asm_data_t*)calloc(code_size, sizeof(asm_data_t));
    if (proc.cmd_array == NULL)
    {
        fprintf(stderr, "CALLOC ERROR\n");
        assert(proc.cmd_array);
        return proc;
    }
    proc.ip = 0;
    proc.code_size = code_size;
    stack_init(&proc.data_stack, 8, sizeof(proc_data_t));
    stack_init(&proc.call_stack, 8, sizeof(asm_data_t));
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
    printf("code size: %d\n", proc -> code_size);
    printf("CMD ARRAY:\n");
    for (size_t i = 0; i < proc->code_size; i++)
    {
        printf("%3d: %lf\n", i, proc -> cmd_array[i]);
    }
    printf("\nREGISTERS\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("%3d: %lf\n", i, proc->registers[i]);
    }

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 10; j++)
        {
            printf("%3lg ", proc -> RAM[i*10+j]);
        }
        printf("\n");
    }
    return 0;
}

int draw_RAM(processor_t* proc)
{
    assert(proc);
    for (size_t line = 0; line < draw_n_lines; line++)
    {
        for (size_t index = 0; index < draw_line_length; index++)
            {
                if(proc -> RAM[line * draw_line_length + index] == 0)
                    printf(".");
                else
                    printf("*");
            }
        printf("\n");
    }

    return 0;
}

int execute_cmds(processor_t* proc)
{
    assert(proc);

    #define DEF_CMD(cmd, num, arg, ...) \
    case CMD_ ##cmd:{ __VA_ARGS__ break;} \

    while (true)
    {
        int cmd = (int)proc -> cmd_array[proc -> ip] & CMD_MASK;
        //printf("cmd:%d\n", cmd);
        switch (cmd)
        {
            #include "commands.h"
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", cmd);
                     break;}
        }
        (proc -> ip)++;
        //proc_dump(proc);
        //int a = 0;
        //scanf("%d", &a);
    }

    #undef DEF_CMD
    return 0;
}

proc_data_t get_push_arg(processor_t* proc)
{
    assert(proc);

    int arg_type = (int)proc -> cmd_array[proc -> ip] & TYPE_MASK;
    (proc -> ip)++;
    proc_data_t arg_value = 0;
    if (arg_type & REG_ARG_MASK) { arg_value +=  proc -> registers[(size_t)proc -> cmd_array[(proc -> ip)++]];}
    if (arg_type & NUM_ARG_MASK) { arg_value +=  proc -> cmd_array[(proc -> ip)++];}
    if (arg_type & MEM_ARG_MASK) { arg_value  =  proc -> RAM[(size_t)arg_value];}

    return arg_value;
}

proc_data_t* get_pop_arg(processor_t* proc)
{
    assert(proc);

    int arg_type = (int)proc -> cmd_array[proc -> ip] & TYPE_MASK;
    proc -> ip++;
    asm_data_t arg_value = 0;
    if (arg_type & MEM_ARG_MASK)
    {
        if (arg_type & REG_ARG_MASK){ arg_value += (asm_data_t)proc -> registers[(size_t)proc -> cmd_array[(proc -> ip)++]];}
        if (arg_type & NUM_ARG_MASK){ arg_value +=  proc -> cmd_array[(proc -> ip)++];}
        return &proc -> RAM[(size_t)arg_value];
    }
    else
    {
        if (arg_type & REG_ARG_MASK) { return &proc -> registers[(size_t)proc -> cmd_array[(proc -> ip)++]];}
        else
        {
            fprintf(stderr, "ERROR: INVALID ARGUMENT FOR get_pop_arg: %d on IP = %d\n", arg_type, proc -> ip);
            return NULL;
        }
    }
}
