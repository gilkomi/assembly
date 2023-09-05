/*
 * This code includes functions for manipulating a symbol table implemented as a linked list.
 */

#include <malloc.h>
#include <stdio.h>
#include "symbol_table.h"

/*
 * Function: free_symbol_list
 * --------------------------
 * Frees the memory allocated for the symbol list.
 *
 * list: Pointer to the symbol list pointer.
 */
void free_symbol_list(struct symbol_list **list){
    struct symbol_list *current = *list;
    while (current != NULL){
        struct symbol_list *temp = current;
        current = current->next;
        free(temp);
    }
    *list = NULL;
}

/*
 * Function: marge_list
 * --------------------
 * Merges two symbol lists together.
 *
 * list1: Pointer to the first symbol list.
 * list2: Pointer to the second symbol list.
 * amount_of_coded_lines_in_list1: The amount of coded lines in the first list (used for updating labels' indexes).
 */
int marge_list(struct symbol_list *list1, struct symbol_list *list2, int amount_of_coded_lines_in_list1){
    struct symbol_list *q = list2;
    struct symbol_list *temp_symbol;
    int error_counter = 0;

    while (q != NULL){
        temp_symbol = (struct symbol_list *)malloc(sizeof (struct symbol_list));
        temp_symbol->symbol = malloc(sizeof (struct symbol));
        temp_symbol->next = NULL;

        temp_symbol->symbol->outsource_type = q->symbol->outsource_type;
        temp_symbol->symbol->appearance_type = q->symbol->appearance_type;
        temp_symbol->symbol->labels_index = q->symbol->labels_index + amount_of_coded_lines_in_list1;
        strcpy(temp_symbol->symbol->label, q->symbol->label);

        error_counter += add_to_symbol_list(list1, temp_symbol);
        q = q->next;

    }
    /*
    free(temp_symbol->symbol);
    free(temp_symbol);
     */

    return error_counter;
}

/*
 * Function: get_symbol
 * --------------------
 * Retrieves a symbol from the symbol list based on the label.
 *
 * first: The first symbol in the symbol list.
 * label: The label of the symbol to retrieve.
 *
 * returns: A pointer to the symbol list node containing the symbol, or NULL if not found.
 */
struct symbol_list* get_symbol(struct symbol_list *first, char *label){
    struct symbol_list *current = first;

    while (current != NULL && current->symbol != NULL){
        if(strcmp(current->symbol->label, label) == 0){
            return current;
        }
        current = current->next;
    }

    return NULL;
}

/*
 * Function: add_to_list
 * ---------------------
 * Adds a symbol to the symbol list.
 *
 * head: The head of the symbol list.
 * new_symbol: The symbol to be added.
 */
void add_to_list(struct symbol_list *head, struct symbol_list *new_symbol){
    if(head->symbol == NULL){
        head->symbol = new_symbol->symbol;
    }
    else{
        while(head->next != NULL)
            head= head->next;
        head->next = new_symbol;
    }
}

/*
 * Function: index_of_label
 * ------------------------
 * Retrieves the index of a label in the symbol list.
 *
 * list: The symbol list.
 * label: The label to find the index for.
 *
 * returns: The index of the label in the symbol list, or -1 if not found.
 */
int index_of_label(struct symbol_list *list, char *label){
    struct symbol_list *res = get_symbol(list, label);
    if(res != NULL &&
    (res->symbol->appearance_type == declaration || res->symbol->outsource_type == ext)){
        return res->symbol->labels_index;
    } else if(res != NULL){
        return index_of_label(res->next, label);
    }

    return -1;
}

/*
 * Function: update_as_external
 * ----------------------------
 * Updates symbols in the symbol list as external based on the external list.
 *
 * symbol_list: The symbol list to update.
 * external_list: The list of external symbols.
 */
int update_as_external(struct symbol_list *symbol_list, struct symbol_list *external_list){
    struct symbol_list *current_ext = external_list;
    struct symbol_list *current = symbol_list;

    int error_counter = 0;

    while (current_ext != NULL && current_ext->symbol != NULL){
        current = symbol_list;

        while (current != NULL && current_ext->symbol != NULL){
            if(current->symbol->outsource_type == ent &&
               strcmp(current->symbol->label, current_ext->symbol->label) == 0){
                printf("ERROR LABEL: %s IS BOTH EXTERNAL AND ENTRY\n", current->symbol->label);
                error_counter++;
            }
            else if(current->symbol->appearance_type == declaration &&
                    strcmp(current->symbol->label, current_ext->symbol->label) == 0){
                printf("ERROR LABEL: %s IS BOTH EXTERNAL AND DECLARED\n", current->symbol->label);
                error_counter++;
            }
            else if(strcmp(current_ext->symbol->label, current->symbol->label) == 0){
                current->symbol->outsource_type = ext;
            }

            current = current->next;
        }

        current_ext = current_ext->next;
    }

    return error_counter;
}

/*
 * Function: add_to_external_symbol_list
 * -------------------------------------
 * Adds a symbol to the list of external symbols.
 *
 * first: The first symbol in the external symbol list.
 * new_symbol: The symbol to be added.
 */
int add_to_external_symbol_list(struct symbol_list *first, struct symbol_list *new_symbol){
    struct symbol_list *current;
    int error_counter = 0;

    if(first == NULL || first->symbol == NULL){
        add_to_list(first, new_symbol);
        return error_counter;
    }

    current = get_symbol(first, new_symbol->symbol->label);

    if(current != NULL && current->symbol != NULL){
        printf("ERROR LABEL: %s DEFINE TWICE AS EXTERNAL\n", current->symbol->label);
        error_counter++;
    } else {
        current = first;
        while (current->next != NULL){
            current = current->next;
        }
        current->next = new_symbol;
    }

    return error_counter;
}

/*
 * Function: add_to_symbol_list
 * ----------------------------
 * Adds a symbol to the symbol list.
 *
 * first: The first symbol in the symbol list.
 * new_symbol: The symbol to be added.
 */
int add_to_symbol_list(struct symbol_list *first, struct symbol_list *new_symbol) {
    struct symbol_list *current;
    int error_counter = 0;

    if(first->symbol == NULL || new_symbol->symbol->appearance_type == usage){
        add_to_list(first, new_symbol);
    }
    else if(new_symbol->symbol->appearance_type == declaration){
        current = first;
        while (current != NULL){
            if(current->symbol->appearance_type == declaration &&
               strcmp(current->symbol->label, new_symbol->symbol->label) == 0){
                printf("ERROR LABEL: \"%s\" DECLARED TWICE\n", current->symbol->label);
                error_counter++;
                break;
            }

            if(strcmp(current->symbol->label, new_symbol->symbol->label) == 0 &&
               current->symbol->outsource_type == ent){
                current->symbol->appearance_type = declaration;
                current->symbol->labels_index = new_symbol->symbol->labels_index;
                break;
            }

            current = current->next;
        }
        if(current == NULL){
            add_to_list(first, new_symbol);
        }

    }
    else if(new_symbol->symbol->outsource_type == ent){
        current = first;
        while (current != NULL){
            if(strcmp(current->symbol->label, new_symbol->symbol->label) == 0 &&
               current->symbol->outsource_type == ent){
                printf("ERROR LABEL: %s DECLARED TWICE AS ENTRY\n", current->symbol->label);
                error_counter++;
                break;
            }

            if(strcmp(current->symbol->label, new_symbol->symbol->label) == 0 &&
               current->symbol->appearance_type == declaration){
                current->symbol->outsource_type = ent;
                break;
            }

            current = current->next;
        }
        if(current == NULL){
            add_to_list(first, new_symbol);
        }
    }

    return error_counter;
}
