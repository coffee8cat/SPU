#include "onegin.h"

size_t count_chr(char* string, char ch)
{
    size_t count = 0;
    while(*string != '\0')
    {
        if (*string == ch)
            count++;
        string++;
    }
    return count;
}

char* readfile(FILE* fp, size_t* text_size, char* text)
{
    assert(fp);

    *text_size = get_file_size(fp);
    printf("%d\n", *text_size);

    text = (char*)calloc(*text_size + 1, sizeof(char));
    if (text == NULL)
    {
        fprintf(stderr, "ERROR: Cannot allocate memory " __FILE__ " %d \n", __LINE__);
        return NULL;
    }
    fread(text, sizeof(char), *text_size, fp);
    fclose(fp);
    *(text + *text_size - 1) = '\n';
    fprintf(stderr, "%s", text);
    return text;
}

size_t get_file_size(FILE* fp)
{
    assert(fp);

    struct stat st = {};
    fstat(fileno(fp), &st);
    return (size_t)st.st_size;
}

char** make_pointers_array(FILE* input_stream)
{
    assert(input_stream);

    size_t text_size = 0;
    char* text = readfile(input_stream, &text_size, text);
    fprintf(stderr, "[%s]\n", text);

    size_t n_lines = count_chr(text, '\n');
    fprintf(stderr, "number of lines: %d\n", n_lines);

    char** array = (char**)calloc(n_lines + 1, sizeof(char*));
    if (array == NULL)
    {
        fprintf(stderr, "ERROR: Cannot allocate memory in function [make_pointers_array] for [onegin -> original]\n");
        return NULL;
    }

    array[0] = text;
    size_t line_num = 1;
    size_t curr_len = 0;
    printf("text in : %p\n", text);
    printf("text end: %p\n", text + text_size - 1);
    for (char* curr_ptr = text; curr_ptr < text + text_size; curr_ptr++)
    {
        if(*curr_ptr == '\n')
        {
            if (curr_len != 0)
            {
                array[line_num] = curr_ptr + 1;
                line_num++;
            }
            else
            {
                array[line_num - 1] = curr_ptr + 1;
            }
            printf("line[%d] curr len: %d\n", line_num - 1, curr_len);
            curr_len = 0;
            *curr_ptr = '\0';
        }
        else
            curr_len++;
    }

    for (size_t i = 0; i < line_num - 2; i++)
    {
        printf("[%3d](%p): %s\n", i, &array[i], array[i]);
    }
    return array;
}
