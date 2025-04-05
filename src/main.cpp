#include <TXLib.h>
#include <stdio.h>

#include "my_stack.h"
#include "spu.h"
#include "getoptions.h"

int main(int argc, char *const argv[])
{
    char* input_file  = (char*)calloc(32, sizeof(char));
    char* output_file = (char*)calloc(32, sizeof(char));
    check_flags(input_file, output_file, argc, argv);
    FILE* stream = fopen(input_file, "r");
    FILE* fp     = fopen(output_file, "w");
    assembler(stream, fp);
    fclose(fp);
    fp = fopen(output_file, "r");
    int* cmd_array = interpreter(fp);
    fclose(fp);
    processor(cmd_array);

    return 0;
}
