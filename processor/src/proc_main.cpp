#include <TXLib.h>
#include <stdio.h>

#include "processor.h"
#include "getoptions.h"

int main(int argc, char *const argv[])
{
    FILE* stream_in  = NULL;
    FILE* stream_out = NULL;
    char* input_file  = (char*)calloc(32, sizeof(char)); // check
    if (input_file == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED (input_file)\n");
        return 0;
    }
    char* output_file = (char*)calloc(32, sizeof(char));
    if (output_file == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED (output_file)\n");
        return 0;
    }
    check_flags(input_file, output_file, argc, argv);

    if (strlen(input_file) == 0)
        stream_in = stdin;
    else
        stream_in  = fopen(input_file, "rb");

    if (strlen(output_file) == 0)
        stream_out = stdout;
    else
        stream_out = fopen(output_file, "wb");


    if (stream_in == NULL)
    {
        fprintf(stderr, "ERROR: stream is not valid (stream_in)");
    }
    if (stream_out == NULL)
    {
        fprintf(stderr, "ERROR: stream is not valid (stream_out)");
    }
    if (check_compatibility(stream_in) == 0)
    {
        size_t code_size = 0;
        fread(&code_size, 1, sizeof(int), stream_in);
        processor_t proc = proc_ctor(code_size); // constructor
        make_cmd_array(&proc, stream_in);
        processor(&proc);
        proc_dtor(&proc);
    }

    return 0;
}
