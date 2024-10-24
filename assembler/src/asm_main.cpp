#include <TXLib.h>
#include <stdio.h>

#include "..\headers\assembler.h"
#include "..\headers\getoptions.h"
#include "onegin.h"

int main(int argc, char *const argv[])
{
/*
;  Main info
;  Arguments ax - number to calc fact
;  Return .../none
; Destrys*/
    char* input_file  = (char*)calloc(32, sizeof(char)); // memory
    char* output_file = (char*)calloc(32, sizeof(char));  // check
    if (input_file == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED (input_file)\n");
        return 0;
    }
    if (output_file == NULL)
    {
        fprintf(stderr, "ERROR: CALLOC FAILED (output_file)\n");
        return 0;
    }

    handle_flags(input_file, output_file, argc, argv);
    FILE* stream_in  = fopen(input_file, "rb");
    FILE* stream_out = fopen(output_file, "wb"); // check
    if (stream_in == NULL)
    {
        fprintf(stderr, "ERROR: stream_in was not opened\n");
        return 0;
    }
    if (stream_out == NULL)
    {
        fprintf(stderr, "ERROR: stream_out was not opened\n");
        return 0;
    }


    size_t num_of_cmds = 0;
    char** array = make_pointers_array(stream_in, &num_of_cmds);
    printf("Number of cmds: [%d]\n", num_of_cmds);

    int* asm_code = (int*)calloc(num_of_cmds * 3 + 3, sizeof(int));
    if (asm_code == NULL)
    {
        fprintf(stderr, "CALLOC ERROR\n");
        //
    }
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
