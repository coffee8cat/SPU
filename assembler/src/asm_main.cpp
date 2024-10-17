#include <TXLib.h>
#include <stdio.h>

#include "..\headers\assembler.h"
#include "..\headers\getoptions.h"
#include "onegin.h"

int main(int argc, char *const argv[])
{
    char* input_file  = (char*)calloc(32, sizeof(char));
    char* output_file = (char*)calloc(32, sizeof(char));

    handle_flags(input_file, output_file, argc, argv);
    FILE* stream_in  = fopen(input_file, "r");
    FILE* stream_out = fopen(output_file, "w");

    size_t num_of_cmds = 0;
    char** array = make_pointers_array(stream_in, &num_of_cmds);
    int* asm_code = (int*)calloc(num_of_cmds * 2 + 3, sizeof(int));
    asm_code[2] = num_of_cmds;

    if (check_compatibility(stream_in, asm_code) == 0)
    {
        assembler(array, asm_code, num_of_cmds);
    }
    fclose(stream_in);
    fclose(stream_out);

    return 0;
}
