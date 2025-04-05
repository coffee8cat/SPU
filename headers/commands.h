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
DEF_CMD(JE,   4, true,
    proc_data_t a = 0;
    proc_data_t b = 0;
    DO_POP(&a);
    DO_POP(&b);
    if (b == a)
        IP = CMD_ARRAY[IP + 1];
    else
        IP = IP + 2;
    IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(JNE,   5, true,
    proc_data_t a = 0;
    proc_data_t b = 0;
    DO_POP(&a);
    DO_POP(&b);
    if (b != a)
        IP = CMD_ARRAY[IP + 1];
    else
        IP = IP + 2;
    IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(CALL, 6, true,
    DO_CALL_PUSH(IP + 2)
    IP = CMD_ARRAY[IP + 1];
    IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(RTN,  7, false,
    proc_data_t arg = 0;
    DO_CALL_POP(&arg)
    IP = (size_t)arg;
    IP--;)
/////////////////////////////////////////////////////////
DEF_CMD(ELEM_IN,   8, false,
    proc_data_t arg = 0;
    printf("ELEM IN: ");
    fscanf(stdin, "%lf", &arg);
    DO_PUSH(arg))
/////////////////////////////////////////////////////////
DEF_CMD(ELEM_OUT,  9, false,
    proc_data_t arg = 0;
    DO_POP(&arg)
    printf("ELEM OUT: %lg\n", arg);)
/////////////////////////////////////////////////////////
DEF_CMD(ADD,  10,  false, ARIFM(+))
DEF_CMD(SUB,  11,  false, ARIFM(-))
DEF_CMD(MUL,  12, false, ARIFM(*))
DEF_CMD(DIV,  13, false, ARIFM(/))

DEF_CMD(SQRT, 14, false, MATH(sqrt))
DEF_CMD(COS,  15, false, MATH(cos))
DEF_CMD(SIN,  16, false, MATH(sin))

DEF_CMD(POW,  17, false,
    proc_data_t a = 0;
    proc_data_t b = 0;
    stack_pop(&proc -> data_stack, &a);
    stack_pop(&proc -> data_stack, &b);
    stack_push(&proc -> data_stack, pow(b, a));  )

DEF_CMD(DUMP, 18, false, DO_DUMP)
DEF_CMD(DRAW, 19, false, DRAW_RAM)

DEF_CMD(MEOW, 20, false,
    proc_data_t arg = 0;
    DO_POP(&arg)
    for (size_t i = 0; i < arg; i++)
    {
        printf("MEOW\n");
    })

DEF_CMD(HLT,  21, false, return 0;)


#undef DO_PUSH
#undef DO_POP
#undef MATH
#undef ARIFM
#undef DRAW_RAM
#undef DO_DUMP
#undef CMD_ARRAY
#undef IP
