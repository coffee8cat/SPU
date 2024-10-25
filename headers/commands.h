DEF_CMD(PUSH, 0, true,
stack_push(&proc -> data_stack, get_push_arg(proc));)

DEF_CMD(POP,  1, true,
stack_pop(&proc -> data_stack, get_pop_arg(proc));)

DEF_CMD(JMP,  2, true,
proc -> ip = proc -> cmd_array[proc -> ip + 1];)

DEF_CMD(JA,   3, true,
proc_data_t a = 0;
proc_data_t b = 0;
stack_pop(&proc -> data_stack, &a);
stack_pop(&proc -> data_stack, &b);
if (b > a)
    proc -> ip = proc -> cmd_array[proc -> ip + 1];
else
    proc -> ip = proc -> ip + 2;)

DEF_CMD(CALL, 4, true,
stack_push(&proc -> call_stack, proc -> ip + 2);
proc -> ip = proc -> cmd_array[proc -> ip + 1];)

DEF_CMD(RTN,  5, false,
stack_pop(&proc -> call_stack, &proc -> ip);)

DEF_CMD(ELEM_IN,   6, false,
int arg = 0;
fscanf(stdin, "%d", &arg);
stack_push(&proc -> data_stack, arg);
proc -> ip++;)

DEF_CMD(ELEM_OUT,  7, false,
int arg = 0;
stack_pop(&proc -> data_stack, &arg);
printf("%d\n", arg);
proc -> ip++;)

DEF_CMD(ADD,  8, false,
int a = 0;
int b = 0;
stack_pop(&proc -> data_stack, &a);
stack_pop(&proc -> data_stack, &b);
stack_push(&proc -> data_stack, b + a);
proc -> ip++;)

DEF_CMD(SUB,  9, false,
int a = 0;
int b = 0;
stack_pop(&proc -> data_stack, &a);
stack_pop(&proc -> data_stack, &b);
stack_push(&proc -> data_stack, b - a);
proc -> ip++;)

DEF_CMD(MULT, 10, false,
int a = 0;
int b = 0;
stack_pop(&proc -> data_stack, &a);
stack_pop(&proc -> data_stack, &b);
stack_push(&proc -> data_stack, b * a);
proc -> ip++;)

DEF_CMD(DIV,  11, false,
int a = 0;
int b = 0;
stack_pop(&proc -> data_stack, &a);
stack_pop(&proc -> data_stack, &b);
stack_push(&proc -> data_stack, b / a);
proc -> ip++;)

DEF_CMD(SQRT, 12, false,
int a = 0;
stack_pop(&proc -> data_stack, &a);
stack_push(&proc -> data_stack, sqrt(a));
proc -> ip++;)

DEF_CMD(COS,  13, false,
int a = 0;
stack_pop (&proc -> data_stack, &a);
stack_push(&proc -> data_stack, cos(a));
proc -> ip++;)

DEF_CMD(SIN,  14, false,
int a = 0;
stack_pop (&proc -> data_stack, &a);
stack_push(&proc -> data_stack, sin(a));
proc -> ip++;)

DEF_CMD(DUMP, 15, false,
proc_dump(proc);
proc -> ip++;)

DEF_CMD(DRAW, 16, false,
draw_RAM(proc);
proc ->ip++;)

DEF_CMD(HLT,  17, false,
return 0;)
