#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "getoptions.h"

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
