#include <TXLib.h>
#include <stdio.h>

#include "..\headers\assembler.h"
#include "..\headers\getoptions.h"

int main(int argc, char *const argv[])
{
    char* input_file  = (char*)calloc(32, sizeof(char));
    char* output_file = (char*)calloc(32, sizeof(char));
    check_flags(input_file, output_file, argc, argv);
    FILE* stream = fopen(input_file, "r");
    FILE* fp     = fopen(output_file, "w");
    assembler(stream, fp);
    fclose(fp);

    return 0;
}
