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

    proc -> cmd_array = (int*)calloc(64, sizeof(int));
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

    stack_init(&proc -> stack, 8, 4);
    proc -> ip = 0;
    while (proc -> cmd_array[proc -> ip] != HLT)
    {
        switch (proc -> cmd_array[proc -> ip])
        {
            case PUSH: {stack_push(&proc -> stack, proc -> cmd_array[proc -> ip + 1]);
                        proc -> ip++;
                        break;}
            case ELEM_IN: {int arg = 0;
                           fscanf(stdin, "%d", &arg);
                           stack_push(&proc -> stack, arg);
                           break;}
            case ELEM_OUT:{int arg = 0;
                           stack_pop(&proc -> stack, &arg);
                           printf("%d\n", arg);
                           break;}

            case ADD: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b + a);
                       break;}
            case SUB: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b - a);
                       break;}
            case MULT:{int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b * a);
                       break;}
            case DIV: {int a = 0;
                       int b = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_pop(&proc -> stack, &b);
                       stack_push(&proc -> stack, b / a);
                       break;}

            case SQRT:{int a = 0;
                       stack_pop(&proc -> stack, &a);
                       stack_push(&proc -> stack, sqrt(a));
                       break;}
            case COS: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, cos(a));
                       break;}
            case SIN: {int a = 0;
                       stack_pop (&proc -> stack, &a);
                       stack_push(&proc -> stack, sin(a));
                       break;}

            case DUMP:{STACK_DUMP(&proc -> stack, __func__);
                       break;}
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", proc -> cmd_array[proc -> ip]);
                     break;}
        }
        proc -> ip++;
    }
}
