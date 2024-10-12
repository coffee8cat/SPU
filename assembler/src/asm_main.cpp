#include <TXLib.h>
#include <stdio.h>

#include "..\headers\assembler.h"
#include "..\headers\getoptions.h"

int main(int argc, char *const argv[])
{
    char* input_file  = (char*)calloc(32, sizeof(char));
    char* output_file = (char*)calloc(32, sizeof(char));
    check_flags(input_file, output_file, argc, argv);
    FILE* stream_in  = fopen(input_file, "r");
    FILE* stream_out = fopen(output_file, "w");

    if (check_compatibility(stream_in, stream_out) == 0)
    {
        assembler(stream_in, stream_out);
    }
    fclose(stream_in);
    fclose(stream_out);

    return 0;
}
