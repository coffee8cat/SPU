#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
            printf("%d %d\n", PUSH, arg);
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
            printf("%d\n", HLT);
            break;
        }
        for (size_t i = 0; i < (size_t)HLT; i++)
        {
            if (strncmp(cmd, instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                asm_code[curr++] = i;
                asm_code[curr++] = '\n';
                printf("%d\n", i);
                continue;
            }
        }
        /*
        if (strncmp(cmd, "dump", strlen("dump")) == 0)
        {
            asm_code[curr++] = (char)DUMP;
            asm_code[curr++] = '\n';
            printf("%d\n", DUMP);
            continue;
        }
        if (strncmp(cmd, "hld", strlen("hld")) == 0)
        {
            asm_code[curr++] = (char)HLT;
            asm_code[curr++] = '\n';
            printf("%d\n", HLT);
            break;
        }
        if (strncmp(cmd, "sqrt", strlen("sqrt")) == 0)
        {
            asm_code[curr++] = (char)SQRT;
            asm_code[curr++] = '\n';
            printf("%d\n", SQRT);
            continue;
        }
        if (strncmp(cmd, "cos", strlen("cos")) == 0)
        {
            asm_code[curr++] = (char)COS;
            asm_code[curr++] = '\n';
            printf("%d\n", COS);
            continue;
        }
        if (strncmp(cmd, "sin", strlen("sin")) == 0)
        {
            asm_code[curr++] = (char)SIN;
            asm_code[curr++] = '\n';
            printf("%d\n", SIN);
            continue;
        }
        if (strncmp(cmd, "add", strlen("add")) == 0)
        {
            asm_code[curr++] = (char)ADD;
            asm_code[curr++] = '\n';
            printf("%d\n", ADD);
            continue;
        }
        if (strncmp(cmd, "sub", strlen("sub")) == 0)
        {
            asm_code[curr++] = (char)SUB;
            asm_code[curr++] = '\n';
            printf("%d\n", SUB);
            continue;
        }
        if (strncmp(cmd, "mult", strlen("mult")) == 0)
        {
            asm_code[curr++] = (char)MULT;
            asm_code[curr++] = '\n';
            printf("%d\n", MULT);
            continue;
        }
        if (strncmp(cmd, "div", strlen("div")) == 0)
        {
            asm_code[curr++] = (char)DIV;
            asm_code[curr++] = '\n';
            printf("%d\n", DIV);
            continue;
        }
        */
    }
    printf("%s\n", asm_code);
    fwrite(asm_code, sizeof(char), curr, stream_out);
    return 0;
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
