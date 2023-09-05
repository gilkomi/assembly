#include <malloc.h>
#include "first_pass.h"

/*
 * The print_symbols function iterates over the symbol list and prints the index, label,
 * outsource type, and appearance type of each symbol. It serves debugging purposes,
 * enabling a clear and formatted visualization of the symbol list content.
 */
void print_symbols(struct symbol_list *list){
    int i = 0;

    while (list != NULL && list->symbol != NULL){
        printf("%d: %s - %d, %d\n",list->symbol->labels_index , list->symbol->label, list->symbol->outsource_type, list->symbol->appearance_type);
        i++;

        list = list->next;
    }
}

/*
 * The print_codes function iterates over the coded list and prints the index and
 * binary code of each node. It is useful for debugging purposes, to visually
 * inspect the content of the coded list in a readable and formatted manner.
 */
void print_codes(struct coded_list *list){
    int i = 100;
    struct coded_node *node = list->head;

    while (node != NULL){
        printf("%d: %s\n",i , node->coded_line);
        i++;

        node = node->next;
    }
}

/*
 * The verify_symbols function checks if the labels used in the symbol_list are declared either in
 * the symbol_list itself or in the ext_symbol_list. If a label is used but not declared, the function
 * prints an error message and increments the error_counter. This function serves as a semantic
 * validation step, ensuring that every symbol referenced in the code has a corresponding declaration.
 *
 * @param: struct symbol_list symbol_list - The list of symbols to be verified.
 * @param: struct symbol_list ext_symbol_list - The list of external symbols to be cross-referenced.
 * @return: int error_counter - The number of errors found during the verification process.
 */
int verify_symbols(struct symbol_list symbol_list, struct symbol_list ext_symbol_list){
    struct symbol_list *first = &symbol_list;
    int error_counter = 0;

    while (first != NULL){
        if(index_of_label(&symbol_list, first->symbol->label) == -1 &&
                index_of_label(&ext_symbol_list, first->symbol->label) == -1){
            error_counter++;
            printf("ERROR LABEL: \"%s\" USED BUT NEVER DECLARED\n", first->symbol->label);
        }

        first = first->next;
    }

    return error_counter;
}

/*
 * The first_pass function performs the first pass analysis of the assembly file. This includes building
 * the syntax tree for each line, classifying the line type (instruction, directive, or error), and
 * adding the codes to the appropriate list (inst_coded_list or dir_coded_list). The function also
 * manages symbols, updates external symbols, and verifies symbols. The function increments the error_counter
 * for each error encountered during these processes.
 *
 * @param: FILE *am_file - The assembly file to be analyzed.
 * @param: struct symbol_list *symbols - The list of symbols.
 * @param: struct symbol_list *ext_symbols - The list of external symbols.
 * @param: struct coded_list *inst_coded_list - The list of instruction codes.
 * @param: struct coded_list *dir_coded_list - The list of directive codes.
 * @param: int *errors_counter - Pointer to the counter of the errors found during the first pass.
 */
void first_pass(FILE *am_file, struct symbol_list *symbols, struct symbol_list *ext_symbols,
        struct coded_list *inst_coded_list, struct coded_list *dir_coded_list,
                int *errors_counter){
    char line[MAX_LINE_SIZE+1];
    struct syntax_tree *st = (struct syntax_tree *) malloc(sizeof (struct syntax_tree));
    struct symbol_list *inst_symbols = (struct symbol_list *) malloc(sizeof (struct symbol_list)),
            *dir_symbols= (struct symbol_list *) malloc(sizeof (struct symbol_list));
    int i = 1;

    inst_symbols->symbol = NULL;
    inst_symbols->next = NULL;

    dir_symbols->symbol = NULL;
    dir_symbols->next = NULL;


    /*
     * This loop reads the assembly file line by line until it reaches the end.
     */
    while (fgets(line, sizeof(line), am_file) != NULL){
        /*
        * This code checks if a line from the file exceeds a defined length. If so, it increments 'errors_counter', prints
        * an error message, discards the rest of the line, and proceeds to the next line.
        */
        if(line[strlen(line) - 1] != '\n' && strlen(line) == MAX_LINE_SIZE){
            (*errors_counter)++;
            printf("%d: ERROR INPUT LENGTH IS TOO LONG\n", i);
            do{
                fgets(line, sizeof(line), am_file);
            } while (line[strlen(line) - 1] != '\n');

            i++;
            continue;
        }

        /*
         * Build syntax tree from line
         */
        build_syntax_tree_from_line(st, line);

        /*
         * If lineType is error, print error message and increment error counter
         */
        if(st->lineType == error) {
            printf("%d: %s - \"%s\"\n", i, st->error_message, line);
            *errors_counter = *errors_counter + 1;
        }
        /*
         * If lineType is instruction, add code to instruction list and update error counter if needed
         */
         else if(st->lineType == instruction){
            *errors_counter = *errors_counter + add_code_to_coded_list(inst_coded_list, *st, inst_symbols, ext_symbols);
        }
         /*
          * If lineType is directive, add code to directive list and update error counter if needed
          */
          else if(st->lineType == directive){
            *errors_counter = *errors_counter + add_code_to_coded_list(dir_coded_list, *st, dir_symbols, ext_symbols);
        }

        i++;
    }

    *errors_counter = *errors_counter + marge_list(inst_symbols, dir_symbols, inst_coded_list->length);
    *errors_counter = *errors_counter + marge_list(symbols, inst_symbols, 100);
    *errors_counter = *errors_counter + update_as_external(symbols, ext_symbols);

    *errors_counter = *errors_counter + verify_symbols(*symbols, *ext_symbols);

    /*
    * Check if the total length of the instruction and directive coded lists exceeds the maximum memory size.
    * If it does, increment the errors counter and print an error message indicating memory overflow.
    */
    if(inst_coded_list->length + dir_coded_list->length > MAX_MEMORY_SIZE){
        (*errors_counter)++;
        printf("ERROR: Memory Overflow");
    }

/*
    printf("---------\n");
    print_symbols(symbols);
    printf("---------\n");
    print_symbols(ext_symbols);
    printf("---------\n");

    print_codes(inst_coded_list);
*/


    free(st);
    free_symbol_list(&inst_symbols);
    free_symbol_list(&dir_symbols);
}

