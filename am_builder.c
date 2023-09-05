/*
 * This code includes functions that manipulate macro lists and build an assembly language file (.am) based on a given assembly file (.as).
 * The code handles macro definitions, macro calls, and error checking.
 */

#include <malloc.h>
#include "am_builder.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Function: is_empty_row
 * ----------------------
 * Checks if a given string represents an empty row.
 *
 * str: The string to check.
 *
 * returns: True if the string is empty or contains only whitespace characters, False otherwise.
 */
bool is_empty_row(char str[]) {
    int i = 0;
    for (; i < strlen(str); ++i) {
        if (!isspace(str[i]))
            return false;
    }
    return true;
}

/*
 * Function: write_to_am_file
 * --------------------------
 * Writes a line to the .am file, either as it is or by expanding a macro.
 *
 * as_file: The .as file being processed.
 * am_file: The .am file to write to.
 * mcro_list: The list of macros.
 * line: The line to write.
 */
void write_to_am_file(FILE *am_file, struct mcro_list *mcro_list, char line[]) {
    struct mcro_list *current = NULL;
    struct mcro mcro;
    bool is_mcro = false;
    int i;

    if (strlen(line) == 0 || line[0] == ';' || is_empty_row(line)) {
        /* Ignore empty lines or comments */
    } else {
        current = mcro_list;
        while (current != NULL) {
            i = 0;
            if (strcmp(line, current->data.mcro_name) == 0) {
                mcro = current->data;
                is_mcro = true;
                for (; i < mcro.code_lines_count; ++i) {
                    fprintf(am_file, "%s", mcro.code_lines[i]);
                }
                break;
            }
            current = current->next;
        }

        if (!is_mcro) {
            fprintf(am_file, "%s", line);
        }
    }
}

/*
 * Function: make_mcro_list_and_am_file
 * ------------------------------------
 * Creates a list of macros and builds the .am file based on the .as file.
 *
 * as_file: The .as file.
 * am_file: The .am file to create.
 * mcro_list: Pointer to the list of macros.
 */
void make_mcro_list_and_am_file(FILE *as_file, FILE *am_file, struct mcro_list** mcro_list) {
    char line[MAX_LINE_SIZE] = "aa";
    struct mcro_list* current_mcro = NULL;
    int row_index = 0;
    char temp[MAX_LABEL_SIZE];

    while (fgets(line, sizeof(line), as_file) != NULL) {
        /* Check if line starts with "mcro" */
        if (strncmp(line, "mcro", 4) == 0) {
            /* Create a new mcro node */
            struct mcro_list* new_mcro = (struct mcro_list*)malloc(sizeof(struct mcro_list));
            strcpy(new_mcro->data.mcro_name, line + 5);
            new_mcro->data.code_lines_count = 0;
            new_mcro->next = NULL;

            strncpy(temp, new_mcro->data.mcro_name, strlen(new_mcro->data.mcro_name) - 1);
            if (!is_not_equal_to_reserved_word(temp)) {
                printf("as - %d: ERROR MACRO NAME IS ILLEGAL\n", row_index);
            }

            /* Add mcro to the mcro_list */
            if (*mcro_list == NULL) {
                *mcro_list = new_mcro;
                current_mcro = *mcro_list;
            } else {
                current_mcro->next = new_mcro;
                current_mcro = current_mcro->next;
            }

            /* Read code lines until "endmcro" is reached */
            while (fgets(line, sizeof(line), as_file) != NULL) {
                if (strncmp(line, "endmcro", 6) == 0) {
                    break;
                }

                /* Add code line to mcro */
                strcpy(current_mcro->data.code_lines[current_mcro->data.code_lines_count], line);
                current_mcro->data.code_lines_count++;
            }
        } else {
            /* Check if the line is a macro call */
            write_to_am_file(am_file, *mcro_list, line);
        }
        row_index++;
    }
}

/*
 * Function: is_mcro_error
 * -----------------------
 * Checks if there is a macro call before its declaration in the .am file.
 *
 * am_file: The .am file.
 * mcro_list: The list of macros.
 */
void is_mcro_error(FILE *am_file, struct mcro_list *mcro_list) {
    char line[MAX_LINE_SIZE];
    int row_index = 0;

    while (fgets(line, sizeof(line), am_file) != NULL) {
        struct mcro_list *current = mcro_list;
        while (current != NULL){
            if(strcmp(line, current->data.mcro_name) == 0){
                printf("am - %d: ERROR MACRO CALLING BEFORE DECLARATION\n", row_index);
                break;
            }
            current = current->next;
        }
        row_index++;
    }
}

/*
 * Function: am_builder
 * --------------------
 * Builds the .am file based on the given .as file.
 *
 * as_file: The .as file.
 */
void am_builder(struct file_struct *as_file) {
    struct mcro_list* mcro_list = NULL;
    struct mcro_list* current_mcro = NULL;
    struct mcro_list* temp = NULL;
    int i;
    struct file_struct *am_file = malloc(sizeof(struct file_struct));

    strcpy(am_file->name, as_file->name);
    am_file->name[strlen(am_file->name) - 1] = 'm';
    am_file->file = fopen(am_file->name, "w+");
    if (am_file->file == NULL) {
        printf("fail\n");
        exit(-1);
    }

    make_mcro_list_and_am_file(as_file->file, am_file->file, &mcro_list);

    fclose(am_file->file);
    am_file->file = fopen(am_file->name, "r");
    if (am_file->file == NULL) {
        printf("fail\n");
    }

    is_mcro_error(am_file->file, mcro_list);

    current_mcro = mcro_list;
    while (current_mcro != NULL) {
        for (i = 0; i < current_mcro->data.code_lines_count; i++) {
            free(current_mcro->data.code_lines[i]);
        }
        temp = current_mcro;
        current_mcro = current_mcro->next;
        free(temp);
    }
    fclose(am_file->file);
    free(am_file);
}
