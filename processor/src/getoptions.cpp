#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "getoptions.h"

int prepare_streams(streams_data* streams_info, int argc, char *const argv[])
{
    assert(streams_info);
    assert(argv);

    check_flags(streams_info -> input_file, streams_info -> output_file, argc, argv);

    if (strlen(streams_info -> input_file) == 0)
        streams_info -> stream_in = stdin;
    else
        streams_info -> stream_in  = fopen(streams_info -> input_file, "rb");

    if (strlen(streams_info -> output_file) == 0)
        streams_info -> stream_out = stdout;
    else
        streams_info -> stream_out = fopen(streams_info -> output_file, "wb");


    if (streams_info -> stream_in == NULL)
    {
        fprintf(stderr, "ERROR: stream is not valid (stream_in)");
        return 1;
    }
    if (streams_info -> stream_out == NULL)
    {
        fprintf(stderr, "ERROR: stream is not valid (stream_out)");
        fclose(streams_info -> stream_in);
        return 2;
    }
    return 0;
}

bool check_flags(char* input_file, char* output_file, int argc, char *const argv[])
{
    assert(input_file);
    assert(output_file);
    assert(argv != NULL);

    int opt = 0;
    const char optstring[] = "hf:o:";

    option longoptions[] ={
        {"help", 0, 0, 'h'},
        {"file_input", 1, 0, 'f'},
        {"output", 1, 0, 'o'}
    };

    int optidx = 0;

    while ((opt = getopt_long(argc, argv, optstring, longoptions, &optidx)) != -1)
    {
        switch (opt)
        {
            case 'h':
                printf("no help yet\n");
                break;

            case 'f':
                strncpy(input_file, optarg, strlen(optarg));
                break;

            case 'o':
                strncpy(output_file, optarg, strlen(optarg));
                break;

            case ':':
                printf("option -%c needs a value\n", optopt);
                break;

            case '?':
                printf("Unknown option: %c\n", optopt);
                break;

            default:
                fprintf(stderr, "option read error\n");

                return false;
        }
    }
    return true;
}
