#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

#include "string.h"
#include "utils.h"

enum outsource_type{
    ent = 1,
    ext
};

enum appearance_type{
    declaration = 1, usage
};

struct symbol{
    char label[MAX_LABEL_SIZE];
    int labels_index;
    int outsource_type;
    int appearance_type;
};

struct symbol_list{
    struct symbol *symbol;
    struct symbol_list *next;
};

int add_to_symbol_list(struct symbol_list *first, struct symbol_list *new_symbol);
void free_symbol_list(struct symbol_list **list);
int index_of_label(struct symbol_list *list, char *label);
int add_to_external_symbol_list(struct symbol_list *first, struct symbol_list *new_symbol);
int update_as_external(struct symbol_list *symbol_list, struct symbol_list *external_list);
int marge_list(struct symbol_list *list1, struct symbol_list *list2, int amount_of_coded_lines_in_list1);


#endif
