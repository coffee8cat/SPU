#include <TXLib.h>
#include <stdio.h>

#include "processor.h"
#include "getoptions.h"

int main(int argc, char *const argv[])
{
    streams_data streams_info = {};

    if (prepare_streams(&streams_info, argc, argv) != 0)
        return -1;

    if (check_compatibility(streams_info.stream_in) == 0)
    {
        size_t code_size = 0;
        fread(&code_size, 1, sizeof(int), streams_info.stream_in);
        processor_t proc = proc_ctor(code_size);
        make_cmd_array(&proc, streams_info.stream_in);

        execute_cmds(&proc);
        proc_dtor(&proc);
    }

    return 0;
}
