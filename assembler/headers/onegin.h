#ifndef _ONEGIN_THE_STRING_HANDLER_H__
#define _ONEGIN_THE_STRING_HANDLER_H__

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* readfile(FILE* fp, size_t* text_size, char* text);
size_t get_file_size(FILE* fp);

#endif
