#ifndef _SOFT_PROCESSOR_UNIT_H__
#define _SOFT_PROCESSOR_UNIT_H__

#define CURRENT_VERSION 2
#define PROC_SIGNATURE 4341

#define AX 0
#define BX 1
#define CX 2
#define DX 3

#define CMD_MASK     0b00011111
#define NUM_ARG_MASK 0b00100000
#define REG_ARG_MASK 0b01000000
#define MEM_ARG_MASK 0b10000000

enum instructions {
    PUSH     = 0b00011011, //27
    POP      = 0b00011100, //28
    JMP      = 0b00011101, //29
    JA       = 0b00011110, //30
    ELEM_IN  = 0b00000000, //0
    ELEM_OUT = 0b00000001, //1
    ADD      = 0b00000010, //2
    SUB      = 0b00000011, //3
    MULT     = 0b00000100, //4
    DIV      = 0b00000101, //5
    SQRT     = 0b00000110, //6
    COS      = 0b00000111, //7
    SIN      = 0b00001000, //8
    DUMP     = 0b00001001, //9
    HLT      = 0b00011111  //31
};

//int assembler(FILE* stream_in, FILE* stream_out);
int* interpreter(FILE* stream);
int processor(int* cmd_array);
int run(FILE* stream);

#endif
