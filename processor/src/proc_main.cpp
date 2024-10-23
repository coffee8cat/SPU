#include <TXLib.h>
#include <stdio.h>

#include "processor.h"
#include "getoptions.h"

int main(int argc, char *const argv[])
{
    FILE* stream_in  = NULL;
    FILE* stream_out = NULL;
    char* input_file  = (char*)calloc(32, sizeof(char)); // check
    char* output_file = (char*)calloc(32, sizeof(char));
    check_flags(input_file, output_file, argc, argv);

    if (strlen(input_file) == 0)
        stream_in = stdin;
    else
        stream_in  = fopen(input_file, "r");

    if (strlen(output_file) == 0)
        stream_out = fopen(output_file, "w");
    else
        stream_out = stdout;


    if (check_compatibility(stream_in) == 0)
    {
        size_t code_size = 0;
        fread(&code_size, 1, sizeof(int), stream_in);
        printf("code size: %d\n", code_size);
        processor_t proc = proc_ctor(code_size); // constructor
        printf("got here\n");
        make_cmd_array(&proc, stream_in);
        for (size_t i = 0; i < code_size; i++)
        {
            printf("%d: %d\n", i, proc.cmd_array[i]);
        }
        printf("sadasdasdasd\n");
        processor(&proc);
    }
    // destructor

    return 0;
}
