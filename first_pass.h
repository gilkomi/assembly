#ifndef ASSEMBLER_FIRST_PASS_H
#define ASSEMBLER_FIRST_PASS_H

#include <stdio.h>
#include "symbol_table.h"
#include "coded_list.h"
#include "utils.h"

void first_pass(FILE *am_file, struct symbol_list *symbols, struct symbol_list *ext_symbols, struct coded_list *inst_coded_list, struct coded_list *dir_coded_list, int *errors_counter);

#endif
