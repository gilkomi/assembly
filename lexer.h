#ifndef ASSEMBLER_LEXER_H
#define ASSEMBLER_LEXER_H

#include "utils.h"

#define SET_ERROR(error_msg) { st->lineType = error; strcpy(st->error_message, error_msg); return false;}


enum line_type{
    error = -1,
    empty,
    comment,
    directive,
    instruction
};

enum dir_type{
    non,
    entry,
    external,
    string,
    data
};

enum op_code {
    op_code_mov = 1,
    op_code_cmp,
    op_code_add,
    op_code_sub,
    op_code_not,
    op_code_clr,
    op_code_lea,
    op_code_inc,
    op_code_dec,
    op_code_jmp,
    op_code_bne,
    op_code_red,
    op_code_prn,
    op_code_jsr,
    op_code_rts,
    op_code_stop
};


struct syntax_tree{
    char label[MAX_LABEL_SIZE + 1];
    char error_message[MAX_ERROR_MSG_SIZE + 1];

    enum line_type lineType;

    union {
        struct {
            enum dir_type dirType;
            union {
                char label[MAX_LABEL_SIZE + 1];
                char str[MAX_LINE_SIZE + 1];
                struct {
                    int int_arr[MAX_VARS];
                    int arr_length;
                }num_arr;
            }dir_info;

        }dir;

        struct {
            enum op_code opCode;

            union {
                struct {
                    char parameter[MAX_LABEL_SIZE + 1];
                }one_parameter;

                struct {
                    char des_parameter[MAX_LABEL_SIZE + 1];
                    char src_parameter[MAX_LABEL_SIZE + 1];
                }two_parameters;
            }one_or_two_parameters;

        }inst;


    }dir_or_inst;


};

void build_syntax_tree_from_line(struct syntax_tree *st, char *line);
void clear_syntax_tree(struct syntax_tree *st);

#endif
