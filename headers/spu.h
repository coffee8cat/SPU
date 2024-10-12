#ifndef _SOFT_PROCESSOR_UNIT_H__
#define _SOFT_PROCESSOR_UNIT_H__

#define CURRENT_VERSION 2
#define PROC_SIGNATURE 4341

#define AX 0
#define BX 1
#define CX 2
#define DX 3

enum instructions {
    PUSH_NUM = 0b00100001, //33
    PUSH_REG = 0b01000001, //65
    POP      = 0b01000010, //66
    JMP      = 0b00100010, //34
    JA       = 0b00100011, //35
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
