#include <TXLib.h>
#include <stdio.h>

#include "my_stack.h"
#include "spu.h"

int main()
{
    FILE* stream = stdin; //fopen(".\\data\\programm1.txt", "r");
    assembler(stdin, stdout);
    //run(stream);
    //fclose(stream);
    return 0;
}
