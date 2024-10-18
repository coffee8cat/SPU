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

int check_compatibility(FILE* stream, int* asm_code)
{
    int version   = 0;
    int signature = 0;

    fseek(stream, 0, SEEK_SET);
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

    asm_code[0] = version;
    asm_code[1] = signature;
    return 0;
}

int translate_push(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char cmd = PUSH;
    char* arg_ptr = array[curr_cmd] + strlen("push ");
    //printf("start: [%p] [%c]\n"
    //       "arg:   [%p] [%c]\n"
    //       "counter[%d]\n", array[curr_cmd], *array[curr_cmd], arg, *arg, *asm_code_counter);

    int reg_arg_value = -1;
    int num_arg_value = -1;
    if (*arg_ptr == '[')
    {
        cmd = cmd | MEM_ARG_MASK;
        arg_ptr++;
        if (*(array[curr_cmd] + strlen(array[curr_cmd]) - 1) != ']')
            fprintf(stderr, "Square bracket not closed in line %d: %s\n", curr_cmd, array[curr_cmd]);
    }
    if (*arg_ptr - '0' > 9 | *arg_ptr - '0' < 0)
    {
        switch(*arg_ptr + *(arg_ptr + 1))
        {
            case 'A' + 'X': reg_arg_value = AX; break;
            case 'B' + 'X': reg_arg_value = BX; break;
            case 'C' + 'X': reg_arg_value = CX; break;
            case 'D' + 'X': reg_arg_value = DX; break;
            default: fprintf(stderr, "Incorrect argument for PUSH: [%s]\n", arg_ptr);
        }
        arg_ptr = arg_ptr + 2;
        if (*arg_ptr == '+')
            arg_ptr++;

        cmd = cmd | REG_ARG_MASK;
    }
    if (*arg_ptr - '0' <= 9 & *arg_ptr - '0' >= 0)
    {
        num_arg_value = atoi(arg_ptr);
        cmd = cmd | NUM_ARG_MASK;
    }

    asm_code[(*asm_code_counter)++] = cmd;
    printf("%d ", cmd);
    if (cmd & REG_ARG_MASK) { asm_code[(*asm_code_counter)++] = reg_arg_value; printf("%d ", reg_arg_value);}
    if (cmd & NUM_ARG_MASK) { asm_code[(*asm_code_counter)++] = num_arg_value; printf("%d ", num_arg_value);}
    printf("\n");

    return 0;
}

int translate_in(int* asm_code, size_t* asm_code_counter, char** array)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    int arg = 0;
    asm_code[(*asm_code_counter)++] = ELEM_IN;
    fprintf(stdout, "%d\n", ELEM_IN);
    fscanf(stdin, "%d", &arg);
    asm_code[(*asm_code_counter)++] = arg;

    return 0;
}

int translate_pop(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char* arg = array[curr_cmd] + strlen("pop ");
    printf("start: [%p] [%c]\n"
           "arg:   [%p] [%c]\n", array[curr_cmd], *array[curr_cmd], arg, *arg);
    int arg_value = -1;
    switch(*arg + *(arg + 1))
    {
        case 'A' + 'X': arg_value = AX; break;
        case 'B' + 'X': arg_value = BX; break;
        case 'C' + 'X': arg_value = CX; break;
        case 'D' + 'X': arg_value = DX; break;
        default: fprintf(stderr, "Incorrect argument for POP: [%s]\n", arg);
    }
    asm_code[(*asm_code_counter)++] = POP | REG_ARG_MASK;
    asm_code[(*asm_code_counter)++] = arg_value;
    fprintf(stdout, "%d %d\n", POP | REG_ARG_MASK, arg_value);

    return 0;
}

int translate_JMP(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char* arg = array[curr_cmd] + strlen("JMP ");
    asm_code[(*asm_code_counter)++] = JMP | NUM_ARG_MASK;
    asm_code[(*asm_code_counter)++] = atoi(arg);

    fprintf(stdout, "%d %d\n", JMP | NUM_ARG_MASK, arg);
    return 0;
}

int translate_JA(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char* arg = array[curr_cmd] + strlen("JA ");
    asm_code[(*asm_code_counter)++] = JA | NUM_ARG_MASK;
    asm_code[(*asm_code_counter)++] = atoi(arg);

    fprintf(stdout, "%d %d\n", JA | NUM_ARG_MASK, arg);
    return 0;
}


int handle_labels(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{

}

int assembler(char** array, int* asm_code, size_t num_of_cmds)
{
    assert(array);
    assert(asm_code);

    label* labels = {};

    printf("asm_code [%p]\n"
           "array    [%p]\n", asm_code, array);

    size_t asm_code_counter = 3;
    for (size_t curr_cmd = 2; curr_cmd < num_of_cmds; curr_cmd++)
    {
        printf("current cmd: %d [%s]\n", curr_cmd, array[curr_cmd]);
        printf("counter: %d\n", asm_code_counter);

        if (strchr(array[curr_cmd], ':') != NULL)
        {
            handle_labels(asm_code, &asm_code_counter, array, curr_cmd);
            continue;
        }
        if (strncmp(array[curr_cmd], "push", strlen("push")) == 0)
        {
            translate_push(asm_code, &asm_code_counter, array, curr_cmd);
            printf("counter after push [%d]\n", asm_code_counter);
            continue;
        }
        if (strncmp(array[curr_cmd], "in", strlen("in")) == 0)
        {
            translate_in(asm_code, &asm_code_counter, array);
            continue;
        }
        if (strncmp(array[curr_cmd], "pop", strlen("pop")) == 0)
        {
            translate_pop(asm_code, &asm_code_counter, array, curr_cmd);
            continue;
        }
        if (strncmp(array[curr_cmd], "JMP", strlen("JMP")) == 0)
        {
            translate_JMP(asm_code, &asm_code_counter, array, curr_cmd);
            continue;
        }
        if (strncmp(array[curr_cmd], "JA", strlen("JA")) == 0)
        {
            translate_JA(asm_code, &asm_code_counter, array, curr_cmd);
            continue;
        }
        if (strncmp(array[curr_cmd], "hlt", strlen("hlt")) == 0)
        {
            asm_code[asm_code_counter++] = HLT;
            printf("%d\n", HLT);
            continue;
        }
        for (size_t i = 0; i <= (size_t)DUMP; i++)
        {
            if (strncmp(array[curr_cmd], instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                fprintf(stdout, "%d\n", i);
                asm_code[asm_code_counter++] = i;
                continue;
            }
        }
    }
    /*
    while (cmds_counter < num_of_cmds)
    {
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
    */
    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%3d ", i);
    }
    printf("\n");
    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%3d ", asm_code[i]);
    }
    //fwrite(asm_code, sizeof(char), curr, stream_out);
    return 0;
}


