#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "my_stack.h"
#include "assembler.h"
#include "spu.h"

const char *instructions_list[] {
    //"push"
    "in", "out",
    "add", "sub", "mult", "div",
    "sqrt", "cos", "sin",
    "dump",
    //"hlt"
};


int check_compatibility(FILE* stream_in, FILE* stream_out)
{
    int version = 0;
    int signature = 0;
    fscanf(stream_in, "%d", &version);
    fscanf(stream_in, "%d", &signature);
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

    fprintf(stream_out, "%d\n%d\n", version, signature);
    return 0;
}

int assembler(FILE* stream_in, FILE* stream_out)
{
    assert(stream_in);
    assert(stream_out);

    stack_t stk = {};
    stack_init(&stk, 8, 4);

    size_t num_of_cmds = 0;
    fscanf(stream_in, "%d", &num_of_cmds);
    fprintf(stream_out, "%d\n", num_of_cmds);
    int* asm_code = (int*)calloc(num_of_cmds * 2 + 3, sizeof(int));
    size_t curr = 0;
    size_t cmds_counter = 0;
    while (cmds_counter < num_of_cmds)
    {
        char cmd[32] = "";
        fscanf(stream_in, "%s", cmd);

        if (strncmp(cmd, "push", strlen("push")) == 0)
        {
            int arg = -1;
            char reg[2] = {};
            if(fscanf(stream_in, "%d", &arg) != 0)
            {
                asm_code[curr++] = PUSH | NUM_ARG_MASK;
                fprintf(stream_out, "%d ", PUSH | NUM_ARG_MASK);
            }
            else
            {
                fscanf(stream_in, " %s", &reg);
                switch(reg[0] + reg[1])
                {
                    case 'A' + 'X': arg = AX; break;
                    case 'B' + 'X': arg = BX; break;
                    case 'C' + 'X': arg = CX; break;
                    case 'D' + 'X': arg = DX; break;
                    default: fprintf(stderr, "Incorrect argument for POP: [%s]\n", reg);
                }
                asm_code[curr++] = PUSH | REG_ARG_MASK;
                fprintf(stream_out, "%d ", PUSH | REG_ARG_MASK);
            }
            asm_code[curr++] = arg;
            //memcpy(asm_code + curr, &arg, sizeof(arg));
            fprintf(stream_out, "%d\n", arg);
            cmds_counter++;
            continue;
        }
        if (strncmp(cmd, "in", strlen("in")) == 0)
        {
            int arg = 0;
            asm_code[curr++] = ELEM_IN;
            fprintf(stream_out, "%d\n", ELEM_IN);
            fscanf(stdin, "%d", &arg);
            asm_code[curr++] = arg;
            //memcpy(asm_code + curr, &arg, sizeof(arg));
            //asm_code[curr++] = '\n';
            cmds_counter++;
            continue;
        }
        if (strncmp(cmd, "pop", strlen("pop")) == 0)
        {
            char reg[2] = {};
            int arg = -1;
            fscanf(stream_in, " %s", &reg);

            switch(reg[0] + reg[1])
            {
                case 'A' + 'X': arg = AX; break;
                case 'B' + 'X': arg = BX; break;
                case 'C' + 'X': arg = CX; break;
                case 'D' + 'X': arg = DX; break;
                default: fprintf(stderr, "Incorrect argument for POP: [%s]\n", reg);
            }
            asm_code[curr++] = POP | REG_ARG_MASK;
            asm_code[curr++] = arg;
            fprintf(stream_out, "%d %d\n", POP | REG_ARG_MASK, arg);
            continue;
        }
        if (strncmp(cmd, "JMP", strlen("JMP")) == 0)
        {
            asm_code[curr++] = JMP | NUM_ARG_MASK;
            int arg = 0;
            fscanf(stream_in, "%d", &arg);
            asm_code[curr++] = arg;
            fprintf(stream_out, "%d %d\n", JMP | NUM_ARG_MASK, arg);
        }
        if (strncmp(cmd, "JA", strlen("JA")) == 0)
        {
            asm_code[curr++] = JA | NUM_ARG_MASK;
            int arg = 0;
            fscanf(stream_in, "%d", &arg);
            asm_code[curr++] = arg;
            fprintf(stream_out, "%d %d\n", JA | NUM_ARG_MASK, arg);
        }
        if (strncmp(cmd, "hlt", strlen("hlt")) == 0)
        {
            asm_code[curr++] = HLT;
            fprintf(stream_out, "%d\n", HLT);
            cmds_counter++;
            break;
        }
        for (size_t i = 0; i <= (size_t)DUMP; i++)
        {
            if (strncmp(cmd, instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                fprintf(stream_out, "%d\n", i);
                asm_code[curr++] = i;
                cmds_counter++;
                continue;
            }
        }
    }
    for (size_t i = 0; i < curr; i++)
    {
        printf("%d ", asm_code[i]);
    }
    //fwrite(asm_code, sizeof(char), curr, stream_out);
    return 0;
}
