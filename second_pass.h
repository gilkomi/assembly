#ifndef ASSEMBLER_SECOND_PASS_H
#define ASSEMBLER_SECOND_PASS_H

#include <stdio.h>
#include "symbol_table.h"
#include "coded_list.h"
#include "stdio.h"
#include "utils.h"


void second_pass(struct symbol_list *symbols, struct coded_list *inst_coded_list,
                 struct coded_list *dir_coded_list, char *file_name);

#endif
