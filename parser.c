#include "parser.h"

/*
 * Syntax validation of the syntax tree.
 */
bool syntax_validation(struct syntax_tree *st){
    /* Source operand
     * 1,3,5 || 3 || non
     */
    if (st->lineType != instruction)
        return false;

    /*
     * 1,3,5
     */
    if (st->dir_or_inst.inst.opCode == op_code_mov ||
        st->dir_or_inst.inst.opCode == op_code_cmp ||
        st->dir_or_inst.inst.opCode == op_code_add ||
        st->dir_or_inst.inst.opCode == op_code_sub) {
        if (!is_valid_number(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter) &&
            !is_valid_register(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter) &&
            !is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter)) {
            SET_ERROR("ERROR INVALID SOURCE OPERAND")
        }
    }

        /*
         * 3
         */
    else if (st->dir_or_inst.inst.opCode == op_code_lea) {
        if (!is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter)) {
            SET_ERROR("ERROR INVALID SOURCE OPERAND")
        }
    }

        /*
         * non
         */
    else if (st->dir_or_inst.inst.opCode == op_code_not ||
             st->dir_or_inst.inst.opCode == op_code_clr ||
             st->dir_or_inst.inst.opCode == op_code_inc ||
             st->dir_or_inst.inst.opCode == op_code_dec ||
             st->dir_or_inst.inst.opCode == op_code_jmp ||
             st->dir_or_inst.inst.opCode == op_code_bne ||
             st->dir_or_inst.inst.opCode == op_code_red ||
             st->dir_or_inst.inst.opCode == op_code_prn ||
             st->dir_or_inst.inst.opCode == op_code_jsr ||
             st->dir_or_inst.inst.opCode == op_code_rts ||
             st->dir_or_inst.inst.opCode == op_code_stop) {
        /* No source operand */
    }

    /* Destination operand
     * 1,3,5 || 3,5 || non
     * 1,3,5
     */
    if (st->dir_or_inst.inst.opCode == op_code_cmp) {
        if (!is_valid_number(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter) &&
            !is_valid_register(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter) &&
            !is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter)) {
            SET_ERROR("ERROR INVALID DESTINATION OPERAND")
        }
    } else if (st->dir_or_inst.inst.opCode == op_code_prn) {
        if (!is_valid_number(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter) &&
            !is_valid_register(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter) &&
            !is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter)) {
            SET_ERROR("ERROR INVALID DESTINATION OPERAND")
        }
    }
        /* 3,5 */
    else if (st->dir_or_inst.inst.opCode == op_code_mov ||
             st->dir_or_inst.inst.opCode == op_code_add ||
             st->dir_or_inst.inst.opCode == op_code_sub ||
             st->dir_or_inst.inst.opCode == op_code_lea) {
        if (!is_valid_register(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter) &&
            !is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter)) {
            SET_ERROR("ERROR INVALID DESTINATION OPERAND")
        }
    } else if (st->dir_or_inst.inst.opCode == op_code_not ||
               st->dir_or_inst.inst.opCode == op_code_clr ||
               st->dir_or_inst.inst.opCode == op_code_inc ||
               st->dir_or_inst.inst.opCode == op_code_dec ||
               st->dir_or_inst.inst.opCode == op_code_jmp ||
               st->dir_or_inst.inst.opCode == op_code_bne ||
               st->dir_or_inst.inst.opCode == op_code_red ||
               st->dir_or_inst.inst.opCode == op_code_jsr) {
        if (!is_valid_register(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter) &&
            !is_valid_var(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter)) {
            SET_ERROR("ERROR INVALID DESTINATION OPERAND")
        }
    }
        /* non */
    else if (st->dir_or_inst.inst.opCode == op_code_rts ||
             st->dir_or_inst.inst.opCode == op_code_stop) {
        /* No destination operand */
    }

    return true;
}
