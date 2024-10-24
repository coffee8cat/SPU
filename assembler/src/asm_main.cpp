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


    size_t text_size = 0;
    char* text = NULL;
    text = readfile(stream_in, &text_size, text);

    int* asm_code = (int*)calloc(text_size / 2, sizeof(int));
    if (asm_code == NULL)
    {
        fprintf(stderr, "CALLOC ERROR\n");
        return 0;
    }

    size_t code_size = 0;
    code_size = assembler(text, text_size, asm_code);
    fwrite(asm_code, sizeof(int), code_size, stream_out);

    fclose(stream_in);
    fclose(stream_out);

    return 0;
}
