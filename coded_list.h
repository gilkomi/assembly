#ifndef ASSEMBLER_CODED_LIST_H
#define ASSEMBLER_CODED_LIST_H

#include "lexer.h"
#include "utils.h"
#include "string.h"
#include "symbol_table.h"

 struct coded_node {
    char coded_line[MAX_LABEL_SIZE];
    struct coded_node *next;
};

 struct coded_list {
    struct coded_node *head;
    int length;
};

int add_code_to_coded_list(struct coded_list *list, struct syntax_tree st, struct symbol_list *symbols,
                            struct symbol_list *extern_symbols);

#endif
