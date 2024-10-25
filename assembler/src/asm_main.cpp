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

    fclose(streams_info.stream_in);

    size_t code_size = 0;
    code_size = assembler(data.text, data.text_size, data.asm_code);

    fwrite(data.asm_code, sizeof(int), code_size, streams_info.stream_out);
    fclose(streams_info.stream_out);

    return 0;
}
