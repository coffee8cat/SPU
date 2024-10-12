#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "my_stack.h"
#include "assembler.h"
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
