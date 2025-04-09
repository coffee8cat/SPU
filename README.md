# Software Processing Unit

##
Implementation of virtual machine for compiling and executing programms written on simpified assembly. A project consists of two modules: assembler interpreter, translating code into a sequence of 8-byte instructions for SPU, and a simulator of processor, consistantly executing instructions until HLT is met.

Before launching for SPU are allocated memory resources for: data stack, call stack, RAM. Stack structure is implemented from my earlier project and stores 8-byte values, so do RAM.

## Instruction set
cmd_array - array of instructions of running programm.
ip - instruction pointer, index of current instruction in an array of instructions of running programm.

Full set of instructions is described below:

**Transfer of control**:
| Instruction | Arguments | Argument Source | Desription |
|-------------|-----------|-----------------|------------|
| CALL        | label(ip)  | cmd_array | x86      |
| RTN         | ip     | call stack| ret(x86) |
|JMP          | ip     | cmd_array | See **Instructions desription**      |
|JE          | ip, a, b    | ip: cmd_array <br> a, b: data stack | See **Instructions desription**|
|JNE          | ip, a, b    | ip: cmd_array <br> a, b: data stack | See **Instructions desription**|
|JA          | ip, a, b    | ip: cmd_array <br> a, b: data stack | See **Instructions desription**|

**Stack interactions**:
| Instruction | Arguments | Argument Source | Desription |
|-------------|-----------|-----------------|------------|
|PUSH         | reg or a or [a]    | reg: register <br> a: cmd_array <br> [a]: RAM[a] | push to data stack|
|POP          | reg or [a]    | reg: register <br> [a]: RAM[a] | pop from data stack|

**Arithmetic operations**:
All arithmetic instructions take arguments from data stack (1 or 2 last pushed) by popping them and store result by pushing it to data stack.
List of implemented operations: ADD, SUB, MUL, DIV, SQRT, COS, SIN, POW.

**Input/Output**:
| Instruction | Arguments | Argument Source | Desription |
|-------------|-----------|-----------------|------------|
|ELEM_IN | None | None | Reads argument from console and stores in data stack|
| ELEM_OUT | num    | data stack | Print value, popped from data stack, to console |

**Extra possibilities**:
 - DRAW - rendering SPU's RAM content, representing zero values as <kbd>.</kbd> and non-zero as <kbd>*</kbd>.
 - DUMP - print to console information about current instruction, call stack, data stack and RAM contents.

**End of execution**:
 - HLT

### Instructions desription
For instruction set desription and modification view `/headers/commands.h` - all instructions described using DSL - view `/headers/DSL_description.h` for details.

## Test setup

- **CPU**: AMD Ryzen 5 4600H with Radeon Graphics 3.00 GHz
- **OS**: W10
- **Compiler**: g++ 13.3.0
