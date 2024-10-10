#include <TXLib.h>
#include <stdio.h>

#include "my_stack.h"
#include "spu.h"

int main()
{
    FILE* stream = fopen(".\\data\\programm1.txt", "r");
    FILE* fp = fopen(".\\data\\programm1_asm.txt", "w");
    assembler(stream, fp);
    fclose(fp);
    fp = fopen(".\\data\\programm1_asm.txt", "r");
    int* cmd_array = interpreter(fp);
    fclose(fp);
    processor(cmd_array);
    //run(stream);
    //fclose(stream);
    return 0;
}
