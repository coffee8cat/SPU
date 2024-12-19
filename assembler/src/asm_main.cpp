#include <TXLib.h>
#include <stdio.h>

#include "assembler.h"
#include "getoptions.h"
#include "onegin.h"

int main(int argc, char *const argv[])
{
    streams_data streams_info = {};

    if (prepare_streams(&streams_info, argc, argv) != 0)
        return -1;

    asm_data data = {};
    if (prepare_to_translate(&data, &streams_info) != 0)
        return -1;

    if (fclose(streams_info.stream_in) != 0)
    {
        fprintf(stderr, "ERROR: Unable to close input stream\n");
        return -1;
    }

    size_t code_size = 0;
    code_size = assembler(data.text, data.text_size, data.asm_code);

    fwrite(data.asm_code, sizeof(asm_data_t), code_size, streams_info.stream_out);

    if(fclose(streams_info.stream_out) != 0)
    {
        fprintf(stderr, "ERROR: Unable to close output stream\n");
        return -1;
    }

    return 0;
}
