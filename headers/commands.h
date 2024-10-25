#include "DSL_description.h"

DEF_CMD(PUSH, 0, true,
DO_PUSH(get_push_arg(proc)))

DEF_CMD(POP,  1, true,
DO_POP(get_pop_arg(proc)))

DEF_CMD(JMP,  2, true,
IP = CMD_ARRAY[IP + 1];)

DEF_CMD(JA,   3, true,
proc_data_t a = 0;
proc_data_t b = 0;
DO_POP(a);
DO_POP(b);
if (b > a)
    IP = CMD_ARRAY[IP + 1];
else
    IP = IP + 2;)

DEF_CMD(CALL, 4, true,
DO_CALL_PUSH(IP + 2)
stack_push(&proc -> call_stack, IP + 2);
IP = CMD_ARRAY[IP + 1];)

DEF_CMD(RTN,  5, false,
DO_CALL_POP(IP))                 ///////

DEF_CMD(ELEM_IN,   6, false,
int arg = 0;
fscanf(stdin, "%d", &arg);
DO_PUSH(arg)
proc -> ip++;)

DEF_CMD(ELEM_OUT,  7, false,
int arg = 0;
DO_POP(arg)
printf("%d\n", arg);
proc -> ip++;)

DEF_CMD(ADD,  8, false,
ARIFM(+)
proc -> ip++;)

DEF_CMD(SUB,  9, false,
ARIFM(-)
proc -> ip++;)

DEF_CMD(MULT, 10, false,
ARIFM(*)
proc -> ip++;)

DEF_CMD(DIV,  11, false,
ARIFM(/)
proc -> ip++;)

DEF_CMD(SQRT, 12, false,
MATH(sqrt)
proc -> ip++;)

DEF_CMD(COS,  13, false,
MATH(cos)
proc -> ip++;)

DEF_CMD(SIN,  14, false,
MATH(sin)
proc -> ip++;)

DEF_CMD(DUMP, 15, false,
DUMP
proc -> ip++;)

DEF_CMD(DRAW, 16, false,
DRAW_RAM
proc ->ip++;)

DEF_CMD(HLT,  17, false,
return 0;)

#undef DO_PUSH
#undef DO_POP
#undef MATH
#undef ARIFM
#undef DRAW_RAM
#undef DUMP
#undef CMD_ARRAY
#undef IP
