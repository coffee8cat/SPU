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
    printf("here\n");
    if (strlen(input_file) == 0)
        stream_in = stdin;
    else
        stream_in  = fopen(input_file, "r");

    if (strlen(output_file) == 0)
        stream_out = fopen(output_file, "w");
    else
        stream_out = stdout;

    processor_data* proc = proc_ctor(); // constructor
    if (check_compatibility(stream_in) == 0)
    {
        make_cmd_array(proc, stream_in);
        processor(proc);
    }
    // destructor

    return 0;
}
