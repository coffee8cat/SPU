#include "DSL_description.h"

//IP++ after proceeding every cmd by default
DEF_CMD(PUSH, 0, true, DO_PUSH(get_push_arg(proc)) IP--;)
DEF_CMD(POP,  1, true, DO_POP(get_pop_arg(proc))   IP--;)

DEF_CMD(JMP,  2, true, IP = CMD_ARRAY[IP + 1]; IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(JA,   3, true,
    proc_data_t a = 0;
    proc_data_t b = 0;
    DO_POP(&a);
    DO_POP(&b);
    if (b > a)
        IP = CMD_ARRAY[IP + 1];
    else
        IP = IP + 2;
    IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(CALL, 4, true,
DO_CALL_PUSH(IP + 2)
IP = CMD_ARRAY[IP + 1];
IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(RTN,  5, false,
proc_data_t arg = 0;
DO_CALL_POP(&arg)
IP = (size_t)arg;
IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(ELEM_IN,   6, false,
proc_data_t arg = 0;
fscanf(stdin, "%lf", &arg);
DO_PUSH(arg))
/////////////////////////////////////////////////////////
DEF_CMD(ELEM_OUT,  7, false,
proc_data_t arg = 0;
DO_POP(&arg)
printf("%lf\n", arg);)
/////////////////////////////////////////////////////////
DEF_CMD(ADD,  8,  false, ARIFM(+))
DEF_CMD(SUB,  9,  false, ARIFM(-))
DEF_CMD(MULT, 10, false, ARIFM(*))
DEF_CMD(DIV,  11, false, ARIFM(/))

DEF_CMD(SQRT, 12, false, MATH(sqrt))
DEF_CMD(COS,  13, false, MATH(cos))
DEF_CMD(SIN,  14, false, MATH(sin))

DEF_CMD(DUMP, 15, false, DO_DUMP)
DEF_CMD(DRAW, 16, false, DRAW_RAM)
DEF_CMD(HLT,  17, false, return 0;)

#undef DO_PUSH
#undef DO_POP
#undef MATH
#undef ARIFM
#undef DRAW_RAM
#undef DO_DUMP
#undef CMD_ARRAY
#undef IP
