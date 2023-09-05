#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "string.h"
#include "lexer.h"
#include "am_builder.h"
#include "symbol_table.h"
#include "coded_list.h"
#include "first_pass.h"
#include "second_pass.h"

void print_differences(struct coded_list *cl, char *file_comp){
    struct coded_node *current = cl->head;
    FILE *tester = fopen(file_comp, "r");
    char line[16];
    int i = 1;

    while (fgets(line, sizeof(line), tester) != NULL && current != NULL){
        if(strncmp(line, current->coded_line, 12) != 0){
            printf("%d: %s - %s\n", i, current->coded_line, line);
        }

        i++;
        current = current->next;
    }

    fclose(tester);

}

void print_differences_files(char *file_src, char *file_comp){
    FILE *src = fopen(file_src, "r");
    FILE *comp = fopen(file_comp, "r");
    char line_src[16], line_comp[16];
    int i = 1;

    while (fgets(line_src, sizeof(line_src), src) != NULL && fgets(line_comp, sizeof(line_comp), comp) != NULL){
        if(strncmp(line_src, line_comp, 12) != 0){
            printf("%d: %s - %s\n", i, line_src, line_comp);
        }

        i++;
    }

    fclose(src);
    fclose(comp);

}

void assembler(char file_name[]){
    struct file_struct *input = (struct file_struct *) malloc(sizeof (struct file_struct));

    struct symbol_list *symbols = malloc(sizeof(struct symbol_list));
    struct symbol_list ext_symbols;
    struct coded_list inst_coded_list;
    struct coded_list dir_coded_list;

    int *errors_counter = (int *)malloc(sizeof (int)), i;

    symbols->symbol = NULL;
    symbols->next = NULL;


    *errors_counter = 0;

    strcpy(input->name, file_name);
    strcat(input->name, ".as");
    input->file = fopen(input->name, "r");
    if(input->file == NULL){
        printf("***************\n");
        printf("File %s Doesn't Found\n", input->name);
        printf("***************\n");

        free(input);
        free(symbols);
        free(errors_counter);

        return;
    }

    am_builder(input);
    fclose(input->file);

    input->name[strlen(input->name) - 1] = 'm';
    input->file = fopen(input->name, "r");
    if(input->file == NULL){
        printf("There Was Problem With Open The File %s\n", input->name);
        exit(-1);
    }


    symbols->symbol = NULL;
    symbols->next = NULL;

    ext_symbols.symbol = NULL;
    ext_symbols.next = NULL;
    inst_coded_list.head = NULL;
    inst_coded_list.length = 0;
    dir_coded_list.head = NULL;
    dir_coded_list.length = 0;

    if(input == NULL){
        printf("fail");
        return ;
    }

    printf("%s\n", file_name);
    for (i = 0; i < strlen(file_name); ++i) {
        printf("-");
    }
    printf("\n");


    first_pass(input->file, symbols, &ext_symbols, &inst_coded_list, &dir_coded_list, errors_counter);

    if(*errors_counter != 0){
        printf("There Are %d Errors\n", *errors_counter);
    } else {
        second_pass(symbols, &inst_coded_list, &dir_coded_list, file_name);
        printf("Files Created Successfully :)\n");
    }

    for (i = 0; i < strlen(file_name); ++i) {
        printf("*");
    }
    printf("\n");
}

int main(int argc, char **argv) {
    int i;

    for (i = 1; i < argc; ++i) {
        assembler(argv[i]);
    }

    return 0;
}
