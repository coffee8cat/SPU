#include <TXLib.h>
#include <stdio.h>

#include "..\headers\assembler.h"
#include "..\headers\getoptions.h"
#include "onegin.h"

int main(int argc, char *const argv[])
{
    char* input_file  = (char*)calloc(32, sizeof(char)); // memory
    char* output_file = (char*)calloc(32, sizeof(char));

    handle_flags(input_file, output_file, argc, argv);
    FILE* stream_in  = fopen(input_file, "rb");
    FILE* stream_out = fopen(output_file, "wb"); // check

    size_t num_of_cmds = 0;
    char** array = make_pointers_array(stream_in, &num_of_cmds);
    printf("Number of cmds: [%d]\n", num_of_cmds);

    int* asm_code = (int*)calloc(num_of_cmds * 3 + 3, sizeof(int));
    asm_code[2] = num_of_cmds - 2;

    size_t code_size = 0;
    if (check_compatibility(stream_in, asm_code) == 0)
    {
        code_size = assembler(array, asm_code, num_of_cmds);
    }
    fwrite(asm_code, sizeof(int), code_size, stream_out);
    fclose(stream_in);
    fclose(stream_out);

    return 0;
}
