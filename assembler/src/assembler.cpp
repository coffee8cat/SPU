#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <TXLib.h>

#include "my_stack.h"
#include "assembler.h"
#include "spu.h"

const char *instructions_list[] {
    //"push"
    "in", "out",
    "add", "sub", "mult", "div",
    "sqrt", "cos", "sin",
    "dump", "rtn"
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
/*
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
*/
int translate_pop(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char cmd = POP;
    char* arg_ptr = array[curr_cmd] + strlen("pop ");

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
            default: fprintf(stderr, "Incorrect argument for POP: [%s]\n", arg_ptr);
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
    if ((cmd & NUM_ARG_MASK) & (!(cmd & REG_ARG_MASK) | !(cmd & MEM_ARG_MASK)))
    {
        fprintf(stderr, "SYNTAX ERROR: Invalid argument for pop: %d", num_arg_value);
    }
    if (cmd & REG_ARG_MASK) { asm_code[(*asm_code_counter)++] = reg_arg_value; printf("%d ", reg_arg_value);}
    if (cmd & NUM_ARG_MASK) { asm_code[(*asm_code_counter)++] = num_arg_value; printf("%d ", num_arg_value);}
    printf("\n");

    return 0;
}

int translate_JMP(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels, label* fixup)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    asm_code[(*asm_code_counter)++] = JMP | NUM_ARG_MASK;

    char* label_name = get_label(array, curr_cmd);
    printf("label: %s\n", label_name);

    for (size_t i = 0; i < n_labels; i++)
    {
        if (labels[i].name == NULL)
        {
            add_label(labels, label_name, -1);
            add_label(fixup, label_name, *asm_code_counter);
            (*asm_code_counter)++;
            printf("fixup added: (%s)[%d]\n", label_name, *asm_code_counter - 1);
            dump_labels(fixup);
            break;
        }
        else if (strncmp(labels[i].name, label_name, strlen(label_name)) != 0)
            continue;

        asm_code[(*asm_code_counter)++] = labels[i].ip;
        fprintf(stdout, "%d %d\n", JMP | NUM_ARG_MASK, labels[i].ip);
        break;
    }
    return 0;
}

int translate_JA(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels, label* fixup)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    asm_code[(*asm_code_counter)++] = JA | NUM_ARG_MASK;

    char* label_name = get_label(array, curr_cmd);
    printf("label: %s\n", label_name);

    for (size_t i = 0; i < n_labels; i++)
    {
        if (labels[i].name == NULL)
        {
            add_label(labels, label_name, -1);
            add_label(fixup, label_name, *asm_code_counter);
            (*asm_code_counter)++;
            printf("fixup added: (%s)[%d]\n", label_name, *asm_code_counter - 1);
            dump_labels(fixup);
            break;
        }
        else if (strncmp(labels[i].name, label_name, strlen(label_name)) != 0)
            continue;

        asm_code[(*asm_code_counter)++] = labels[i].ip;
        fprintf(stdout, "%d %d\n", JA | NUM_ARG_MASK, labels[i].ip);
        break;
    }
    return 0;
}

int translate_CALL(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels, label* fixup)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    asm_code[(*asm_code_counter)++] = CALL | NUM_ARG_MASK;

    char* label_name = get_label(array, curr_cmd);
    printf("label: %s\n", label_name);

    for (size_t i = 0; i < n_labels; i++)
    {
        if (labels[i].name == NULL)
        {
            add_label(labels, label_name, -1);
            add_label(fixup, label_name, *asm_code_counter);
            (*asm_code_counter)++;
            printf("fixup added: (%s)[%d]\n", label_name, *asm_code_counter - 1);
            dump_labels(fixup);
            break;
        }
        else if (strncmp(labels[i].name, label_name, strlen(label_name)) != 0)
            continue;

        asm_code[(*asm_code_counter)++] = labels[i].ip;
        fprintf(stdout, "%d %d\n", CALL | NUM_ARG_MASK, labels[i].ip);
        break;
    }
    return 0;
}

int handle_labels(int* asm_code, size_t* asm_code_counter, char** array, size_t curr_cmd, label* labels)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(array);

    char* label_name = get_label(array, curr_cmd);
    printf("label: %s\n", label_name);

    add_label(labels, label_name, *asm_code_counter - 3); // first three are for tech info
    dump_labels(labels);

    return 0;
}

int add_label(label* labels, char* label_name, int index)
{
    assert(labels);
    assert(label_name);

    for (size_t i = 0; i < n_labels; i++)
    {
        if (labels[i].name == NULL)
        {
            labels[i].name = label_name;        //возможно нужно вычесть 2
        }
        else if (strncmp(labels[i].name, label_name, strlen(label_name)) != 0)
            continue;

        labels[i].ip = index;
        break;
    }
    return 0;
}

char* get_label(char** array, size_t curr_cmd)
{
    char* start = strchr(array[curr_cmd], ':');
    while (start > array[curr_cmd] & *(start - 1) != ' ')
        start--;

    return start;
}

int dump_labels(label* labels)
{
    assert(labels);

    printf("---LABELS DUMP---\n");
    for (size_t i = 0; i < n_labels; i++)
        printf("%2d: ip: [%d]; name: [%s]\n", i, labels[i].ip, labels[i].name);

    return 0;
}

int fix_code(label* fixup, label* labels, int* asm_code)
{
    assert(fixup);
    assert(labels);
    assert(asm_code);

    for (size_t fix_ind = 0; fix_ind < n_labels; fix_ind++)
    {
        if (fixup[fix_ind].name == NULL)
            break;
        else
        {
            for (size_t label_ind = 0; label_ind < n_labels; label_ind++)
            {
                if (labels[label_ind].name == NULL)
                {
                    fprintf(stderr, "FIXUP ERROR: label for fixup was not found (%s)\n", fixup[fix_ind].name);
                    break;
                }
                else if (strncmp(labels[label_ind].name, fixup[fix_ind].name, strlen(fixup[fix_ind].name)) != 0)
                    continue;

                asm_code[fixup[fix_ind].ip] = labels[label_ind].ip;
                break;
            }
        }

    }
    return 0;
}

size_t assembler(char** array, int* asm_code, size_t num_of_cmds)
{
    assert(array);
    assert(asm_code);

    label* labels = (label*)calloc(n_labels, sizeof(label));
    label* fixup  = (label*)calloc(n_labels, sizeof(label)); //what about size?

    printf("asm_code [%p]\n"
           "array    [%p]\n", asm_code, array);

    size_t asm_code_counter = 3;
    for (size_t curr_cmd = 2; curr_cmd <= num_of_cmds; curr_cmd++)
    {
        printf("current cmd: %d [%s]\n", curr_cmd, array[curr_cmd]);
        printf("counter: %d\n", asm_code_counter);

        if (strncmp(array[curr_cmd], "push", strlen("push")) == 0)
        {
            translate_push(asm_code, &asm_code_counter, array, curr_cmd);
            printf("counter after push [%d]\n", asm_code_counter);
            continue;
        }
        if (strncmp(array[curr_cmd], "pop", strlen("pop")) == 0)
        {
            translate_pop(asm_code, &asm_code_counter, array, curr_cmd);
            continue;
        }
        if (strncmp(array[curr_cmd], "hlt", strlen("hlt")) == 0)
        {
            asm_code[asm_code_counter++] = HLT;
            printf("%d\n", HLT);
            continue;
        }
        if (strncmp(array[curr_cmd], "JMP", strlen("JMP")) == 0)
        {
            translate_JMP(asm_code, &asm_code_counter, array, curr_cmd, labels, fixup);
            continue;
        }
        if (strncmp(array[curr_cmd], "JA", strlen("JA")) == 0)
        {
            translate_JA(asm_code, &asm_code_counter, array, curr_cmd, labels, fixup);
            continue;
        }
        if (strncmp(array[curr_cmd], "CALL", strlen("CALL")) == 0)
        {
            translate_CALL(asm_code, &asm_code_counter, array, curr_cmd, labels, fixup);
            continue;
        }
        if (strchr(array[curr_cmd], ':') != NULL)
        {
            handle_labels(asm_code, &asm_code_counter, array, curr_cmd, labels);
            continue;
        }
        for (size_t i = 0; i <= (size_t)RTN; i++)
        {
            if (strncmp(array[curr_cmd], instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                fprintf(stdout, "%d\n", i);
                asm_code[asm_code_counter++] = i;
                continue;
            }
        }
        fprintf(stderr, "SYNTAX ERROR: UNKNOWN COMMAND: %s\n", array[curr_cmd]);
    }
    dump_labels(fixup);
    fix_code(fixup, labels, asm_code);

    asm_code[2] = asm_code_counter - 3;
    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%4d ", i);
    }
    printf("\n");
    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%4d ", asm_code[i]);
    }

    txDump(asm_code);
    return asm_code_counter;
}


