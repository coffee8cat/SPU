#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "my_stack.h"
#include "spu.h"

const char *instructions_list[] {
    "push", "in", "out",
    "add", "sub", "mult", "div",
    "sqrt", "cos", "sin",
    "dump",
    "hlt"
};

int assembler(FILE* stream_in, FILE* stream_out)
{
    stack_t stk = {};
    stack_init(&stk, 8, 4);
    char asm_code[512] = "";
    size_t curr = 0;
    while (1)
    {
        char cmd[32] = "";
        fscanf(stream_in, "%s", cmd);

        if (strncmp(cmd, "push", strlen("push")) == 0)
        {
            int arg = 0;
            asm_code[curr++] = (char)PUSH;
            asm_code[curr++] = ' ';
            fscanf(stream_in, "%d", &arg);
            memcpy(asm_code + curr, &arg, sizeof(arg));
            asm_code[curr++] = '\n';
            fprintf(stream_out, "%d %d\n", PUSH, arg);
            continue;
        }
        if (strncmp(cmd, "in", strlen("in")) == 0)
        {
            int arg = 0;
            asm_code[curr++] = (char)ELEM_IN;
            asm_code[curr++] = ' ';
            fscanf(stdin, "%d", &arg);
            memcpy(asm_code + curr, &arg, sizeof(arg));
            asm_code[curr++] = '\n';
            continue;
        }
        if (strncmp(cmd, "hlt", strlen("hlt")) == 0)
        {
            asm_code[curr++] = (char)HLT;
            asm_code[curr++] = '\n';
            fprintf(stream_out, "%d\n", HLT);
            break;
        }
        for (size_t i = 0; i < (size_t)HLT; i++)
        {
            if (strncmp(cmd, instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                asm_code[curr++] = i;
                asm_code[curr++] = '\n';
                fprintf(stream_out, "%d\n", i);
                continue;
            }
        }
    }
    printf("%s\n", asm_code);
    //fwrite(asm_code, sizeof(char), curr, stream_out);
    return 0;
}

int* interpreter(FILE* stream)
{
    int* cmd_array = (int*)calloc(64, sizeof(int));
    size_t curr = 0;
    while (fscanf(stream, "%d", cmd_array + curr) != EOF)
    {
        curr++;
    }
    for (size_t i = 0; i <= curr; i++)
    {
        fprintf(stdout, "[%2d] = [%d]\n", i, cmd_array[i]);
    }
    return cmd_array;
}

int processor(int* cmd_array)
{
    assert(cmd_array);

    stack_t stk = {};
    stack_init(&stk, 8, 4);
    size_t ip = 0;
    while (cmd_array[ip] != HLT)
    {
        switch (cmd_array[ip])
        {
            case PUSH: {stack_push(&stk, cmd_array[ip + 1]);
                        ip++;
                        break;}
            case ELEM_IN: {int arg = 0;
                           fscanf(stdin, "%d", &arg);
                           stack_push(&stk, arg);
                           break;}
            case ELEM_OUT:{int arg = 0;
                           stack_pop(&stk, &arg);
                           printf("%d\n", arg);
                           break;}

            case ADD: {int a = 0;
                       int b = 0;
                       stack_pop(&stk, &a);
                       stack_pop(&stk, &b);
                       stack_push(&stk, b + a);
                       break;}
            case SUB: {int a = 0;
                       int b = 0;
                       stack_pop(&stk, &a);
                       stack_pop(&stk, &b);
                       stack_push(&stk, b - a);
                       break;}
            case MULT:{int a = 0;
                       int b = 0;
                       stack_pop(&stk, &a);
                       stack_pop(&stk, &b);
                       stack_push(&stk, b * a);
                       break;}
            case DIV: {int a = 0;
                       int b = 0;
                       stack_pop(&stk, &a);
                       stack_pop(&stk, &b);
                       stack_push(&stk, b / a);
                       break;}

            case SQRT:{int a = 0;
                       stack_pop(&stk, &a);
                       stack_push(&stk, sqrt(a));
                       break;}
            case COS: {int a = 0;
                       stack_pop(&stk, &a);
                       stack_push(&stk, cos(a));
                       break;}
            case SIN: {int a = 0;
                       stack_pop(&stk, &a);
                       stack_push(&stk, sin(a));
                       break;}

            case DUMP:{STACK_DUMP(&stk, __func__);
                       break;}
            default:{fprintf(stderr, "ERROR: UNKNOWN COMMAND [%d]\n", cmd_array[ip]);
                     break;}
        }
        ip++;
    }
}

int run(FILE* stream)
{
    stack_t stk = {};
    stack_init(&stk, 8, 4);
    while (1)
    {
        char cmd[32] = "";
        fscanf(stream, "%s", cmd);

        if (strncmp(cmd, "push", strlen("push")) == 0)
        {
            int arg = 0;
            fscanf(stream, "%d", &arg);
            stack_push(&stk, arg);
            continue;
        }
        if (strncmp(cmd, "in", strlen("in")) == 0)
        {
            int arg = 0;
            fscanf(stdin, "%d", &arg);
            stack_push(&stk, arg);
            continue;
        }
        if (strncmp(cmd, "out", strlen("out")) == 0)
        {
            int arg = 0;
            stack_pop(&stk, &arg);
            printf("%d\n", arg);
            continue;
        }
        if (strncmp(cmd, "dump", strlen("dump")) == 0)
        {
            STACK_DUMP(&stk, __func__);
            continue;
        }
        if (strncmp(cmd, "hld", strlen("hld")) == 0)
        {
            break;
        }

        int a = 0;
        stack_pop(&stk, &a);

        if (strncmp(cmd, "sqrt", strlen("sqrt")) == 0)
        {
            stack_push(&stk, sqrt(a));
            continue;
        }
        if (strncmp(cmd, "cos", strlen("cos")) == 0)
        {
            stack_push(&stk, cos(a));
            continue;
        }
        if (strncmp(cmd, "sin", strlen("sin")) == 0)
        {
            stack_push(&stk, sin(a));
            continue;
        }

        int b = 0;
        stack_pop(&stk, &b);

        if (strncmp(cmd, "add", strlen("add")) == 0)
        {
            stack_push(&stk, b + a);
            continue;
        }
        if (strncmp(cmd, "sub", strlen("sub")) == 0)
        {
            stack_push(&stk, b - a);
            continue;
        }
        if (strncmp(cmd, "mult", strlen("mult")) == 0)
        {
            stack_push(&stk, b * a);
            continue;
        }
        if (strncmp(cmd, "div", strlen("div")) == 0)
        {
            if (a == 0)
            {
                fprintf(stderr, "ERROR: DIVISION BY ZERO\n");
                break;
            }
            stack_push(&stk, b / a);
            continue;
        }
    }

    return 0;
}
