#define DO_PUSH(arg) stack_push(&proc -> data_stack, arg);
#define DO_CALL_PUSH(arg) stack_push(&proc -> call_stack, arg);

#define DO_POP(arg)  stack_pop(&proc -> data_stack, arg);
#define DO_CALL_POP(arg) stack_pop(&proc -> call_stack, arg);

#define MATH(func) \
proc_data_t a = 0; \
stack_pop (&proc -> data_stack, &a); \
stack_push(&proc -> data_stack, func(a)); \

#define ARIFM(oper) \
proc_data_t a = 0; \
proc_data_t b = 0; \
stack_pop(&proc -> data_stack, &a); \
stack_pop(&proc -> data_stack, &b); \
stack_push(&proc -> data_stack, b oper a); \

#define DRAW_RAM draw_RAM(proc);
#define DO_DUMP proc_dump(proc);

#define CMD_ARRAY proc -> cmd_array
#define IP proc->ip

