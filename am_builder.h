#include "stdio.h"
#include "utils.h"

#ifndef ASSEMBLER_AM_BUILDER_H
#define ASSEMBLER_AM_BUILDER_H

struct file_struct{
    FILE *file;
    char name[MAX_LINE_SIZE];
};

struct mcro {
    char mcro_name[MAX_LABEL_SIZE];
    char* code_lines[MAX_LINE_SIZE];
    int code_lines_count;
};

struct mcro_list {
    struct mcro data;
    struct mcro_list* next;
};

void am_builder(struct file_struct *as_file);


#endif
