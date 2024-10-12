#ifndef _SOFT_PROCESSOR_UNIT_H__
#define _SOFT_PROCESSOR_UNIT_H__

enum instructions {
    PUSH, ELEM_IN, ELEM_OUT,
    ADD,  SUB, MULT, DIV,
    SQRT, COS, SIN,
    DUMP,
    HLT
};

//int assembler(FILE* stream_in, FILE* stream_out);
int* interpreter(FILE* stream);
int processor(int* cmd_array);
int run(FILE* stream);

#endif
