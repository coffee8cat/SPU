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
    "push", "pop",
    "JMP", "JA", "CALL", "RTN",
    "in", "out",
    "add", "sub", "mult", "div",
    "sqrt", "cos", "sin",
    "dump", "draw",
    "hlt"
};

int translate_push_pop(char cmd, int* asm_code, size_t* asm_code_counter, char* curr)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    char* arg_ptr = curr + strlen(instructions_list[cmd]) + 1; // + 1 for space

    int reg_arg_value = -1;
    int num_arg_value = -1;
    if (*arg_ptr == '[')
    {
        cmd = cmd | MEM_ARG_MASK;
        arg_ptr++;
    }
    if (*arg_ptr - '0' > 9 || *arg_ptr - '0' < 0)
    {
        if (*arg_ptr - 'A' < 8 & *arg_ptr -'A' > 0)
        {
            reg_arg_value = *arg_ptr - 'A';
        }
        else
        {
            fprintf(stderr, "INVALID ARGUMENT FOR POP: %s\n", arg_ptr);
        }
        arg_ptr = arg_ptr + 2;
        if (*arg_ptr == '+')
            arg_ptr++;

        cmd = cmd | REG_ARG_MASK;
    }
    if ((*arg_ptr - '0' <= 9) && (*arg_ptr - '0' >= 0))
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

    if (cmd == POP && (cmd & NUM_ARG_MASK) && !(cmd & REG_ARG_MASK) && !(cmd & MEM_ARG_MASK))
    {
        fprintf(stderr, "ARGUMENT ERROR: Pop cannot be used with only number as an argument\n");
        //abort();
        return -1;
    }

    return 0;
}

int translate_label_func(char cmd, int* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    asm_code[(*asm_code_counter)++] = cmd | NUM_ARG_MASK;

    char* label_name = get_label(curr);
    printf("label: %s\n", label_name);

    add_label(fixup, label_name, *asm_code_counter);
    (*asm_code_counter)++;

    return 0;
}

int handle_labels(int* asm_code, size_t* asm_code_counter, char* curr, label* labels)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    printf("curr: %p\n", curr);
    char* label_name = get_label(curr);

    add_label(labels, label_name, *asm_code_counter - 3); // first three are for tech info

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
            labels[i].ip = index;
            break;
        }
    }
    dump_labels(labels);
    return 0;
}

char* get_label(char* curr)
{
    char* start = strchr(curr, ':');
    *start = '\0';
    printf("start: %p", start);
    while (start > curr && !isspace(*(start-1)))
        start--;

    printf("start: %p\n", start);
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
char* skip_space(char* curr, char* end)
{
    assert(curr);
    while ((curr < end) && isspace(*curr))
        curr++;

    return curr;
}

char* skip_comment(char* curr, char* end)
{
    assert(curr);
    while (curr < end && *curr != '\n')
        curr++;
    curr++;
    return curr;
}

size_t assembler(char* text, size_t text_size, int* asm_code)
{
    assert(text);
    assert(asm_code);

    label* labels = (label*)calloc(n_labels, sizeof(label));
    if (labels == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED\n");
        return 0;
    }
    label* fixup  = (label*)calloc(n_labels, sizeof(label)); //what about size? // chexk
    if (fixup == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED\n");
        return 0;
    }
    printf("asm_code [%p]\n"
           "text    [%p]\n", asm_code, text);

    asm_code[0] = CURRENT_VERSION;
    asm_code[1] = PROC_SIGNATURE;
    size_t asm_code_counter = 3;

    char* text_end = text + text_size;
    char* curr = text - 1;
    *curr = '\n';
    bool cmd_found = false;

    while (curr < text_end)
    {
        cmd_found = false;
        curr = strchr(curr, '\n');
        curr = skip_space(curr, text_end);

        //printf("curr [%p], (%d)\n"
        //    "end  [%p]\n", curr, *curr, text_end);
        if (curr >= text_end)
            break;

        if (*curr == ';')
        {
            curr = skip_comment(curr, text_end);
        }
        if (strncmp(curr, "push", strlen("push")) == 0)
        {
            translate_push_pop(PUSH, asm_code, &asm_code_counter, curr);
            continue;
        }
        if (strncmp(curr, "pop", strlen("pop")) == 0)
        {
            printf("pop\n");
            translate_push_pop(POP, asm_code, &asm_code_counter, curr);
            continue;
        }
        if (strncmp(curr, "JMP", strlen("JMP")) == 0)
        {
            translate_label_func(JMP, asm_code, &asm_code_counter, curr, labels, fixup);
            curr = strchr(curr, '\0') + 1;
            continue;
        }
        if (strncmp(curr, "JA", strlen("JA")) == 0)
        {
            translate_label_func(JA, asm_code, &asm_code_counter, curr, labels, fixup);
            curr = strchr(curr, '\0') + 1;
            continue;
        }
        if (strncmp(curr, "CALL", strlen("CALL")) == 0)
        {
            translate_label_func(CALL, asm_code, &asm_code_counter, curr, labels, fixup);
            curr = strchr(curr, '\0') + 1;
            continue;
        }
        for (size_t i = 0; i <= (size_t)HLT; i++)
        {
            if (strncmp(curr, instructions_list[i], strlen(instructions_list[i])) == 0)
            {
                fprintf(stdout, "%d\n", i);
                asm_code[asm_code_counter++] = i;
                cmd_found = true;
                break;
            }
        }
        if (cmd_found)
            continue;
        else
        {
            if (strchr(curr, ':') != NULL)
            {
                printf("LABEL FOUND\n");
                handle_labels(asm_code, &asm_code_counter, curr, labels);
                curr = strchr(curr, '\0') + 1;
                continue;
            }
            //fprintf(stderr, "ERROR: COMMAND CANNOT BE UNDERSTOOD\n");
            //abort();
        }
    }
    printf("---LABELS---\n");
    dump_labels(labels);
    printf("---FIXUP---\n");
    dump_labels(fixup);
    fix_code(fixup, labels, asm_code);

    asm_code[2] = asm_code_counter - 3;

    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%3d: %d\n", i, asm_code[i]);
    }

    txDump(asm_code);
    return asm_code_counter;
}


