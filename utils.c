#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"
#include "lexer.h"

char reserved_words[AMOUNT_OF_RESERVED_WORDS][MAX_LABEL_SIZE] = {
        "mov","cmp","add",
        "sub","not","clr",
        "lea","inc","dec",
        "jmp","bne","red",
        "prn","jsr","rts",
        "stop",
        ".string", ".data", ".entry", ".extern",
        "@r1","@r2","@r3","@r4","@r5","@r6","@r7"
};


/*
 * Function: is_not_equal_to_reserved_word
 * ----------------------------
 *   Checks if a given string is not equal to any reserved word.
 *
 *   str: the string to check
 *
 *   returns: true if the string is not equal to any reserved word, false otherwise.
 */
bool is_not_equal_to_reserved_word(const char *str) {
    int i = 0;

    for(; i < AMOUNT_OF_RESERVED_WORDS; i++){
        if(strcmp(str, reserved_words[i]) == 0){
            return false;
        }
    }

    return true;
}


/*
 * Function: char_at
 * ----------------------------
 *   Finds the index of a specific character in a string.
 *
 *   str: the string to search in
 *   ch: the character to find
 *
 *   returns: the index of the character in the string, or -1 if not found.
 */
int char_at(const char *str, char ch){
    int i = 0;
    for (; i < strlen(str); ++i) {
        if(str[i] == ch){
            return i;
        }
    }

    return -1;
}


/*
 * Function: is_valid_number
 * ----------------------------
 *   Checks if a given string is a valid number.
 *
 *   str: the string to check
 *
 *   returns: true if the string is a valid number, false otherwise.
 */
bool is_valid_number(const char *str){
    int i = 0, num;

    /* Check if it is starting with + or - */
    if(strlen(str) > 1){
        if( (str[0] != '+' && str[0] != '-' && !isdigit(str[0]))){
            return false;
        } else {
            /* If it does, start the loop from index 2 */
            i++;
        }
    }
    for (; i < strlen(str); ++i) {
        if(!isdigit(str[i]))
            return false;
    }

    num = atoi(str);
    if(num > 1000 && num < -1000){
        return false;
    }

    return true;
}


/*
 * Function: is_valid_var
 * ----------------------------
 *   Checks if a given string is a valid variable name.
 *
 *   str: the string to check
 *
 *   returns: true if the string is a valid variable name, false otherwise.
 */
bool is_valid_var(const char *str){
    if(str == NULL || strlen(str) == 0){
        return false;
    }
    if(str[0] < 'A' || str[0] > 'z'){
        return false;
    }
    return true;
}


/*
 * Function: is_valid_register
 * ----------------------------
 *   Checks if a given string is a valid register name.
 *
 *   str: the string to check
 *
 *   returns: true if the string is a valid register name, false otherwise.
 */
bool is_valid_register(const char *str){
    int amount_of_operands = 8, i = 0;
    char reg[] = "@r ";

    if(str[0] != '@')
        return false;

    for (; i < amount_of_operands; ++i) {
        reg[2] = i + 48;
        if(strcmp(str, reg) == 0){
            return true;
        }
    }

    return false;
}


/*
 * Function: reverse_string
 * ----------------------------
 *   Reverses a given string in-place.
 *
 *   str: the string to reverse
 */
void reverse_string(char* str) {
    int length = strlen(str);
    int i, j;
    for (i = 0, j = length - 1; i < j; i++, j--) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}


/*
 * Function: decimal_to_binary
 * ----------------------------
 *   Converts a decimal number to a binary string representation.
 *
 *   decimal_number: the decimal number to convert
 *   n: the number of bits in the binary representation
 *
 *   returns: the binary string representation of the decimal number.
 */
char* decimal_to_binary(int decimal_number, int n) {
    char* binary_str = malloc((n + 1) * sizeof(char));
    int i;

    /* Check if decimal_number is negative */
    if (decimal_number < 0) {
        int positive_decimal = -decimal_number;
        int complement_decimal = ((1 << n) - positive_decimal);

        /* Convert complement_decimal to binary string manually */
        for (i = n - 1; i >= 0; i--) {
            binary_str[i] = (complement_decimal & 1) ? '1' : '0';
            complement_decimal >>= 1;
        }
    } else {
        /* Convert decimal_number to binary string manually */
        for (i = n - 1; i >= 0; i--) {
            binary_str[i] = (decimal_number & 1) ? '1' : '0';
            decimal_number >>= 1;
        }
    }

    binary_str[n] = '\0';  /* Null-terminate the string */
    return binary_str;
}


/*
 * Function: get_num_of_parameters_inst
 * ----------------------------
 *   Gets the number of parameters for a given instruction opcode.
 *
 *   op_code: the opcode of the instruction
 *
 *   returns: the number of parameters for the instruction.
 */
int get_num_of_parameters_inst(int op_code){
    if(op_code == op_code_mov ||
       op_code == op_code_cmp ||
       op_code == op_code_add ||
       op_code == op_code_sub ||
       op_code == op_code_lea){
        return 2;
    }
        /* Single parameter */
    else if(op_code == op_code_not ||
            op_code == op_code_clr ||
            op_code == op_code_inc ||
            op_code == op_code_dec ||
            op_code == op_code_jmp ||
            op_code == op_code_bne ||
            op_code == op_code_red ||
            op_code == op_code_prn ||
            op_code == op_code_jsr){
        return 1;
    }
        /* Non-parameter */
    else if(op_code == op_code_rts ||
            op_code == op_code_stop){
        return 0;
    }

    return -1;
}



