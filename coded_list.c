/*
 * This code is a collection of functions that manipulate a linked list of coded lines.
 * The list is represented by the struct coded_list, which contains a head pointer to the first node in the list
 * and a length variable that keeps track of the number of nodes in the list.
 */

#include "coded_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum operand_type {
    immediate = 1,
    direct = 3,
    direct_register = 5
};

/*
 * Function: add_str_to_list
 * -------------------------
 * Adds a new coded line to the end of the coded list.
 *
 * list: The coded list to add the line to.
 * code: The coded line to be added.
 */
static void add_str_to_list(struct coded_list *list, const char *code) {
    struct coded_node *next = (struct coded_node *) malloc(sizeof(struct coded_node));
    struct coded_node *current;

    strcpy(next->coded_line, code);
    next->next = NULL;

    current = list->head;
    if (current == NULL) {
        list->head = next;
        list->length++;
    } else {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = next;
        list->length++;
    }
}

/*
 * Function: get_register_number
 * -----------------------------
 * Returns the register number extracted from the register string.
 *
 * reg: The register string (e.g., "r1", "r2", etc.).
 *
 * returns: The register number.
 */
static int get_register_number(const char *reg) {
    int i = reg[2] - '0';
    return i;
}

/*
 * Function: parse_inst_op_code
 * ----------------------------
 * Parses the instruction's op code and determines the source and destination operand types.
 * It generates the binary code for the op code and operand types.
 *
 * dest: The destination string to store the generated binary code.
 * st: The syntax tree containing the instruction information.
 * source_type: Pointer to the variable to store the source operand type.
 * des_type: Pointer to the variable to store the destination operand type.
 */
void parse_inst_op_code(char *dest,const struct syntax_tree st, int *source_type, int *des_type) {
    char binary_code[14] = "";

    /* Code the source operand type - 9-11 bits */
    if (st.dir_or_inst.inst.opCode == op_code_mov ||
        st.dir_or_inst.inst.opCode == op_code_cmp ||
        st.dir_or_inst.inst.opCode == op_code_add ||
        st.dir_or_inst.inst.opCode == op_code_sub ||
        st.dir_or_inst.inst.opCode == op_code_lea) {
        if (is_valid_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter)) {
            *source_type = immediate;
            strcat(binary_code, "001");
        } else if (is_valid_var(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter)) {
            *source_type = direct;
            strcat(binary_code, "011");
        } else if (is_valid_register(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter)) {
            *source_type = direct_register;
            strcat(binary_code, "101");
        }
    } else {
        *source_type = 0;
        strcat(binary_code, "000");
    }

    /* Code the op code - 8-5 bits */
    strcat(binary_code, decimal_to_binary(st.dir_or_inst.inst.opCode - 1, 4));

    /* Code the destination operand type - 4-2 bits */
    if (st.dir_or_inst.inst.opCode == op_code_stop ||
        st.dir_or_inst.inst.opCode == op_code_rts) {
        *des_type = 0;
        strcat(binary_code, "000");
    } else {
        if (is_valid_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter)) {
            *des_type = immediate;
            strcat(binary_code, "001");
        } else if (is_valid_var(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter)) {
            *des_type = direct;
            strcat(binary_code, "011");
        } else if (is_valid_register(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter)) {
            *des_type = direct_register;
            strcat(binary_code, "101");
        }
    }
    /* A R E */
    strcat(binary_code, "00");

    strcpy(dest, binary_code);
}

/*
 * Function: parse_inst_des_parameter
 * ----------------------------------
 * Parses the instruction's destination parameter and generates the binary code for it.
 *
 * dest: The destination string to store the generated binary code.
 * st: The syntax tree containing the instruction information.
 * des_type: The destination operand type.
 * symbol_list: The list of symbols encountered in the code.
 * coded_list: The list of coded lines.
 */
void parse_inst_des_parameter(char *dest, struct syntax_tree st, int des_type, struct symbol_list *symbol_list,
                              struct coded_list coded_list) {
    char binary_code[14] = "";
    struct symbol_list *next;

    if (des_type == immediate) {
        int source = atoi(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter);
        strcat(binary_code, decimal_to_binary(source, 10));
        strcat(binary_code, "00");
    } else if (des_type == direct_register) {
        strcat(binary_code, "00000");
        strcat(binary_code, decimal_to_binary(
                get_register_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter),
                5));
        strcat(binary_code, "00");
    } else if (des_type == direct) {
        strcpy(binary_code, st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter);

        next = (struct symbol_list *) malloc(sizeof(struct symbol_list));
        next->symbol = (struct symbol *) malloc(sizeof (struct symbol));

        next->next = NULL;
        strcpy(next->symbol->label, st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter);
        next->symbol->outsource_type = non;
        next->symbol->appearance_type = usage;
        next->symbol->labels_index = coded_list.length;

         add_to_symbol_list(symbol_list, next);
    }

    strcpy(dest, binary_code);
}

/*
 * Function: parse_inst_src_parameter
 * ----------------------------------
 * Parses the instruction's source parameter and generates the binary code for it.
 *
 * dest: The destination string to store the generated binary code.
 * st: The syntax tree containing the instruction information.
 * source_type: The source operand type.
 * symbol_list: The list of symbols encountered in the code.
 * coded_list: The list of coded lines.
 */
void parse_inst_src_parameter(char *dest, struct syntax_tree st, int source_type, struct symbol_list *symbol_list,
                              struct coded_list coded_list) {
    char binary_code[14] = "";
    struct symbol_list *next;

    if (source_type == immediate) {
        int source = atoi(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter);
        strcpy(binary_code, decimal_to_binary(source, 10));
        strcat(binary_code, "00");
    } else if (source_type == direct_register) {
        strcat(binary_code, decimal_to_binary(
                get_register_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter),
                5));
        strcat(binary_code, "00000");
        strcat(binary_code, "00");
    } else if (source_type == direct) {
        strcpy(binary_code, st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter);

        next = (struct symbol_list *) malloc(sizeof(struct symbol_list));
        next->symbol = (struct symbol *) malloc(sizeof (struct symbol));

        next->next = NULL;
        strcpy(next->symbol->label, st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter);
        next->symbol->outsource_type = non;
        next->symbol->appearance_type = usage;
        next->symbol->labels_index = coded_list.length;

         add_to_symbol_list(symbol_list, next);
    }

    strcpy(dest, binary_code);
}

/*
 * Function: parse_instruction_to_code
 * -----------------------------------
 * Parses an instruction syntax tree node and generates the binary code for it.
 * It calls the respective functions to parse the op code, source parameter, and destination parameter.
 * The generated binary code is then added to the coded list.
 *
 * coded_list: The coded list to add the binary code to.
 * st: The syntax tree containing the instruction information.
 * symbol_list: The list of symbols encountered in the code.
 */
void parse_instruction_to_code(struct coded_list *coded_list, struct syntax_tree st, struct symbol_list *symbol_list) {
    char binary_code[14] = "";
    int source_type, des_type;

    parse_inst_op_code(binary_code, st, &source_type, &des_type);

    add_str_to_list(coded_list, binary_code);
    strcpy(binary_code, "");

    if (source_type == direct_register && des_type == direct_register) {
        strcat(binary_code, decimal_to_binary(
                get_register_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter), 5));
        strcat(binary_code, decimal_to_binary(
                get_register_number(st.dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter), 5));
        /* A R E */
        strcat(binary_code, "00");

        add_str_to_list(coded_list, binary_code);
    } else {
        if(get_num_of_parameters_inst(st.dir_or_inst.inst.opCode) == 2){
            parse_inst_src_parameter(binary_code, st, source_type, symbol_list, *coded_list);
            add_str_to_list(coded_list, binary_code);
        }

        if(get_num_of_parameters_inst(st.dir_or_inst.inst.opCode) > 0){
            parse_inst_des_parameter(binary_code, st, des_type, symbol_list, *coded_list);
            add_str_to_list(coded_list, binary_code);

            strcpy(binary_code, "");
        }
    }
}

/*
 * Function: parse_directive_to_code
 * ---------------------------------
 * Parses a directive syntax tree node and generates the binary code for it.
 * The generated binary code is then added to the coded list.
 *
 * list: The coded list to add the binary code to.
 * st: The syntax tree containing the directive information.
 * symbol_list: The list of symbols encountered in the code.
 * extern_symbols: The list of external symbols encountered in the code.
 */
int parse_directive_to_code(struct coded_list *list, struct syntax_tree st, struct symbol_list *symbol_list, struct symbol_list *extern_symbols) {
    char binary_code[14] = "";
    int i, error_counter = 0;
    struct symbol_list *next = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    next->symbol = (struct  symbol*) malloc(sizeof (struct symbol));

    switch (st.dir_or_inst.dir.dirType) {
        case string:
            for (i = 0; i < strlen(st.dir_or_inst.dir.dir_info.str); ++i) {
                strcpy(binary_code, "");
                strcpy(binary_code, decimal_to_binary((int) st.dir_or_inst.dir.dir_info.str[i], 12));
                add_str_to_list(list, binary_code);
            }
            strcpy(binary_code, "000000000000");
            add_str_to_list(list, binary_code);
            break;

        case data:
            for (i = 0; i < st.dir_or_inst.dir.dir_info.num_arr.arr_length; ++i) {
                strcpy(binary_code, "");
                strcpy(binary_code, decimal_to_binary(st.dir_or_inst.dir.dir_info.num_arr.int_arr[i], 12));
                add_str_to_list(list, binary_code);
            }
            break;

        case entry:
            next->next = NULL;
            strcpy(next->symbol->label, st.dir_or_inst.dir.dir_info.label);
            next->symbol->outsource_type = ent;
            next->symbol->labels_index = -2;

            error_counter += add_to_symbol_list(symbol_list, next);
            break;

        case external:
            next->next = NULL;
            strcpy(next->symbol->label, st.dir_or_inst.dir.dir_info.label);
            next->symbol->outsource_type = ext;
            next->symbol->appearance_type = non;

            error_counter += add_to_external_symbol_list(extern_symbols, next);
            break;
        case non:
            break;
    }

    return error_counter;
}

/*
 * Function: parse_syntax_tree_to_code
 * -----------------------------------
 * Parses a syntax tree node and generates the binary code for it.
 * It checks whether the node represents an instruction or a directive and calls the respective parsing function.
 *
 * coded_list: The coded list to add the binary code to.
 * st: The syntax tree node to be parsed.
 * symbol_list: The list of symbols encountered in the code.
 * extern_symbols: The list of external symbols encountered in the code.
 */
int parse_syntax_tree_to_code(struct coded_list *coded_list, struct syntax_tree st, struct symbol_list *symbol_list,
                               struct symbol_list *extern_symbols) {
    int error_counter = 0;

    if(strlen(st.label) != 0){
        struct symbol_list *next = (struct symbol_list *) malloc(sizeof(struct symbol_list));
        next->symbol = (struct symbol *) malloc(sizeof (struct symbol));

        next->next = NULL;
        strcpy(next->symbol->label, st.label);
        next->symbol->outsource_type = non;
        next->symbol->appearance_type = declaration;
        next->symbol->labels_index = coded_list->length;

        error_counter += add_to_symbol_list(symbol_list, next);
    }

    if (st.lineType == directive) {
        error_counter += parse_directive_to_code(coded_list, st, symbol_list, extern_symbols);
    } else if (st.lineType == instruction) {
        parse_instruction_to_code(coded_list, st, symbol_list);
    }

    return error_counter;
}

/*
 * Function: add_code_to_coded_list
 * --------------------------------
 * Adds the binary code generated from a syntax tree node to the coded list.
 * It calls the respective parsing function based on whether the node represents an instruction or a directive.
 *
 * list: The coded list to add the binary code to.
 * st: The syntax tree node containing the code to be added.
 * symbols: The list of symbols encountered in the code.
 * extern_symbols: The list of external symbols encountered in the code.
 */
int add_code_to_coded_list(struct coded_list *list, struct syntax_tree st, struct symbol_list *symbols,
                            struct symbol_list *extern_symbols) {
    int error_counter = 0;

    error_counter += parse_syntax_tree_to_code(list, st, symbols, extern_symbols);

    if (error_counter > 0) {
        printf("%d\n", error_counter);
    }
    return error_counter;
}
