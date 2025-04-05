#ifndef _GET_OPTIONS_FOR_PROCESSOR_H__
#define _GET_OPTIONS_FOR_PROCESSOR_H__

struct streams_data {
    FILE* stream_in;
    FILE* stream_out;
    char input_file[32];
    char output_file[32];
};

int prepare_streams(streams_data* streams_info, int argc, char *const argv[]);
bool check_flags(char* input_file, char* output_file, int argc, char *const argv[]);
#endif
