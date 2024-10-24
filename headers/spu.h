#ifndef _SOFT_PROCESSOR_UNIT_H__
#define _SOFT_PROCESSOR_UNIT_H__

#define CURRENT_VERSION 3
#define PROC_SIGNATURE 4341

typedef int proc_data_t;

#define CMD_MASK     0b00011111
#define TYPE_MASK    0b11100000
#define NUM_ARG_MASK 0b00100000
#define REG_ARG_MASK 0b01000000
#define MEM_ARG_MASK 0b10000000

enum instructions {
    PUSH     = 0b00000, //0
    POP      = 0b00001, //1
    JMP      = 0b00010, //2
    JA       = 0b00011, //3
    CALL     = 0b00100, //4
    RTN      = 0b00101, //5
    ELEM_IN  = 0b00110, //6
    ELEM_OUT = 0b00111, //7
    ADD      = 0b01000, //8
    SUB      = 0b01001, //9
    MULT     = 0b01010, //10
    DIV      = 0b01011, //11
    SQRT     = 0b01100, //12
    COS      = 0b01101, //13
    SIN      = 0b01110, //14
    DUMP     = 0b01111, //15
    DRAW     = 0b10000, //16
    HLT      = 0b10001  //17
};

#endif
