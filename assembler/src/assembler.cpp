
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <TXLib.h>

#include "my_stack.h"
#include "assembler.h"
#include "getoptions.h"
#include "onegin.h"
#include "spu.h"

#define DEF_CMD(cmd, ...) #cmd,

const char *instructions_list[] {
    #include "commands.h"
    "end"
};

#undef DEF_CMD

int prepare_to_translate(asm_data* data, streams_data* streams_info)
{
    assert(data);
    assert(streams_info);

    data -> text = readfile(streams_info -> stream_in, &data -> text_size, data -> text);
    if (data -> text == NULL)
        return -1;

    data -> asm_code = (asm_data_t*)calloc(data -> text_size / 2, sizeof(int));
    if (data -> asm_code == NULL)
    {
        fclose(streams_info -> stream_in);
        fclose(streams_info -> stream_out);
        fprintf(stderr, "CALLOC ERROR\n");
        return -1;
    }
    return 0;
}

char* translate_push_pop(int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    char* arg_ptr = curr + strlen(instructions_list[cmd]) + 1; // + 1 for space

    int reg_arg_value = -1;
    proc_data_t num_arg_value = -1;
    if (*arg_ptr == '[')
    {
        cmd = cmd | MEM_ARG_MASK;
        arg_ptr++;
    }
    if ((*arg_ptr - '0' > 9) || (*arg_ptr - '0' < 0) && (*arg_ptr != '-'))
    {
        if ((*arg_ptr - 'A' < 8) & (*arg_ptr -'A' >= 0))
        {
            reg_arg_value = *arg_ptr - 'A';
        }
        else
        {
            fprintf(stderr, "INVALID ARGUMENT FOR PUSH/POP: %s\n", arg_ptr);
            return NULL;
        }
        arg_ptr = arg_ptr + 2;
        if (*arg_ptr == '+')
            arg_ptr++;

        cmd = cmd | REG_ARG_MASK;
    }
    if ((*arg_ptr - '0' <= 9) && (*arg_ptr - '0' >= 0) || (*arg_ptr == '-'))
    {
        num_arg_value = atof(arg_ptr);
        cmd = cmd | NUM_ARG_MASK;
    }

    asm_code[(*asm_code_counter)++] = cmd;

    if (cmd & REG_ARG_MASK) { asm_code[(*asm_code_counter)++] = reg_arg_value;}
    if (cmd & NUM_ARG_MASK) { asm_code[(*asm_code_counter)++] = num_arg_value;}

    if (cmd == CMD_POP && (cmd & NUM_ARG_MASK) && !(cmd & REG_ARG_MASK) && !(cmd & MEM_ARG_MASK))
    {
        fprintf(stderr, "ARGUMENT ERROR: Pop cannot be used with only number as an argument\n");
        //abort();
        return NULL;
    }

    curr = skip_until_space(arg_ptr);
    return curr;
}

char* translate_label_func(int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    asm_code[(*asm_code_counter)++] = cmd | NUM_ARG_MASK;

    char* label_name = get_label(curr);

    add_label(fixup, label_name, *asm_code_counter);
    (*asm_code_counter)++;

    curr = strchr(curr, '\0') + 1;
    return curr;
}

char* handle_labels(asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels)
{
    assert(asm_code);
    assert(asm_code_counter);
    assert(curr);

    printf("curr: %p\n", curr);
    char* label_name = get_label(curr);

    add_label(labels, label_name, *asm_code_counter - 3); // first three are for tech info

    curr = strchr(curr, '\0') + 1;
    return curr;
}

int add_label(label* labels, char* label_name, int index)
{
    assert(labels);
    assert(label_name);

    for (size_t i = 0; i < n_labels; i++)
    {
        if (labels[i].name == NULL)
        {
            labels[i].name = label_name;
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
    while (start > curr && !isspace(*(start-1)))
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

int fix_code(label* fixup, label* labels, asm_data_t* asm_code)
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

char* skip_until_space(char* curr)
{
    assert(curr);
    while (!isspace(*curr))
        curr++;

    return curr;
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

int prepare_to_assemble(label** labels, label** fixup, asm_data_t* asm_code, size_t* asm_code_counter)
{
    *labels = (label*)calloc(n_labels, sizeof(label));
    if (labels == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED\n");
        return 0;
    }
    *fixup  = (label*)calloc(n_labels, sizeof(label)); //what about size? // chexk
    if (fixup == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED\n");
        return 0;
    }

    asm_code[0] = CURRENT_VERSION;
    asm_code[1] = PROC_SIGNATURE;
    *asm_code_counter = 3;

    return 0;
}

char* Compile_with_arg(int cmd, asm_data_t* asm_code, size_t* asm_code_counter, char* curr, label* labels, label* fixup)
{
    if (cmd == CMD_PUSH || cmd == CMD_POP)
        curr = translate_push_pop(cmd, asm_code, asm_code_counter, curr);
    else
        curr = translate_label_func(cmd, asm_code, asm_code_counter, curr, labels, fixup);

    return curr;
}

size_t assembler(char* text, size_t text_size, asm_data_t* asm_code)
{
    assert(text);
    assert(asm_code);

    label* labels = NULL;
    label* fixup  = NULL;
    size_t asm_code_counter = 0;

    prepare_to_assemble(&labels, &fixup, asm_code, &asm_code_counter);

    #define DEF_CMD(cmd, num, arg, ...) \
    printf("cmd: %s\n", #cmd);\
    if (strncmp(curr, #cmd, strlen(#cmd)) == 0) \
    { \
        if (arg) {curr = Compile_with_arg(num, asm_code, &asm_code_counter, curr, labels, fixup);} \
        else \
        { \
            asm_code[asm_code_counter++] = num; \
            curr = skip_until_space(curr); \
        } \
        continue; \
    } \

    char* text_end = text + text_size;
    char* curr = text;

    while (curr < text_end)
    {
        curr = skip_space(curr, text_end);

        printf("curr [%p], (%d)\n"
               "end  [%p]\n", curr, *curr, text_end);
        if (curr >= text_end)
            break;

        if (*curr == ';')
        {
            curr = skip_comment(curr, text_end);
            continue;
        }

        #include "commands.h"

        if (strchr(curr, ':') != NULL && (strchr(curr, ':') < strchr(curr, '\n')))
        {
            curr = handle_labels(asm_code, &asm_code_counter, curr, labels);
            continue;
        }
        fprintf(stderr, "ERROR: COMMAND CANNOT BE UNDERSTOOD\n");
        char* end = strchr(curr, '\n');
        *end = '\0';
        printf("%s\n", curr);
        return 0;
    }

    #undef DEF_CMD

    printf("---LABELS---\n");
    dump_labels(labels);
    printf("---FIXUP---\n");
    dump_labels(fixup);

    fix_code(fixup, labels, asm_code);
    asm_code[2] = asm_code_counter - 3;

    for (size_t i = 0; i < asm_code_counter; i++)
    {
        printf("%3d: %lf\n", i, asm_code[i]);
    }

    txDump(asm_code);
    return asm_code_counter;
}
