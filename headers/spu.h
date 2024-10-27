#ifndef _SOFT_PROCESSOR_UNIT_H__
#define _SOFT_PROCESSOR_UNIT_H__

#define CURRENT_VERSION 3
#define PROC_SIGNATURE 4341

typedef double asm_data_t;
typedef double proc_data_t;
const double ACCURACY = 10000;

#define CMD_MASK     0b00011111
#define TYPE_MASK    0b11100000
#define NUM_ARG_MASK 0b00100000
#define REG_ARG_MASK 0b01000000
#define MEM_ARG_MASK 0b10000000

#define DEF_CMD(cmd, num, ...) CMD_ ##cmd = num,
enum instructions {
    #include "commands.h"
};

#undef DEF_CMD

#endif
