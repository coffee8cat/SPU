#include "onegin.h"

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
