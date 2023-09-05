#include <stdlib.h>
#include "lexer.h"
#include "string.h"
#include "ctype.h"
#include "parser.h"
#include "utils.h"

void escape_white_chars(const char** str) {
    int i = 0;

    while((*str)[i] != '\0' && isspace((*str)[i])) {
        i++;
    }
    *str = *str + i;
}

/*
 * Function that verifies if the word in the string represents an integer number.
 */
static bool is_a_number(char *str){
    int i = 0;

    /*
     * Check if it starts with + or -
     */
    if(strlen(str) > 1){
        if( (str[0] != '+' && str[0] != '-' && !isdigit(str[0]))){
            return false;
        } else {
            /*
             * If it does, start the loop from index 2
             */
            i++;
        }
    }
    for (; i < strlen(str); ++i) {
        if(!isdigit(str[i]))
            return false;
    }

    return true;
}

/*
 * Extract the first word from a sentence (line) to the word.
 * A word is defined as "all the letters until the first space".
 */
static void get_first_word(const char *line, char *word){
    int i = 0;
    for (; !isspace(line[i]) && line[i] != '\0' ; i++) {
    }
    strncpy(word, line, i);
    word[i] = '\0';
}

/*
 * Extract the first parameter from a sentence (line) to the word.
 * A parameter is defined as "all the letters until the first comma, if there is no comma then the whole word".
 */
static bool get_first_parameter(struct syntax_tree *st, const char *line, char *word){
    int comma_index = char_at(line, ',');

    if(comma_index == -1){
        comma_index = strlen(line);
    }
    strncpy(word, line, comma_index);
    word[comma_index] = '\0';

    if(strlen(word) == 0){
        SET_ERROR("ERROR MISSING PARAMETER")
    }

    return true;
}

/*
 * Check if the word (input) is a valid label:
 * - Starts with a letter
 * - Does not equal a reserved word
 */
static bool is_label(struct syntax_tree *st, const char *input){
    if(!is_not_equal_to_reserved_word(input)){
        strcpy(st->error_message, "ERROR LABEL IS INVALID, RESERVED WORD USED");
        st->lineType = error;
    } else if(!(input[0] >= 'A' && input[0] <= 'z')){
        strcpy(st->error_message, "ERROR LABEL IS INVALID");
        st->lineType = error;
    }
    return st->lineType != error;
}

/*
 * Check if the word (input) is a declaration of a label
 * Example: "LABEL:" , "WORD:"
 */
static bool is_declaration_label(const char *input){
    return input[strlen(input) - 1] == ':';
}

/*
 * Function that removes unnecessary spaces from a string.
 * Unnecessary spaces are:
 * - Two spaces together, e.g., "word[SPACE][SPACE]word"
 * - Leading or trailing spaces, e.g., "[SPACE][SPACE]word" or "word[SPACE][SPACE]"
 */
static bool remove_white_char_from_str(struct syntax_tree *st, char *str){
    char* temp = malloc((strlen(str) + 1) * sizeof(char));
    int i = 0, j = 0, white_char_counter = 0, is_comma_exist = false;

    if(str == NULL || strlen(str) == 0){
        return true;
    }

    for (; i < strlen(str); ++i) {
        if(!isspace(str[i])){
            if(str[i] != ','){
                if(white_char_counter != 0 && !is_comma_exist){
                    SET_ERROR("ERROR MISSING COMMA")
                }
                white_char_counter = 0;
                is_comma_exist = false;
            } else {
                if(is_comma_exist){
                    SET_ERROR("ERROR TOO MANY COMMAS OR MISSING PARAMETER")
                }
                is_comma_exist = true;
            }

            temp[j] = str[i];
            j++;
        } else {
            white_char_counter++;
        }
    }

    temp[j] = '\0';
    if(temp[strlen(temp) - 1] == ','){
        SET_ERROR("ERROR MISSING PARAMETER OR TOO MANY COMMAS")
    }
    if(strcmp(str, temp) != 0){
        strcpy(str, temp);
    }

    free(temp);
    return true;
}

/*
 * Extract a string from a sentence.
 * A string is enclosed in apostrophes.
 * Example: "string "word""
 */
static bool extract_string(struct syntax_tree *st, const char *line){
    char temp[MAX_LINE_SIZE];
    int i;

    escape_white_chars(&line);

    for (i = strlen(line) - 1; i >= 0 ; i--) {
        if(!isspace(line[i])) {
            break;
        }
    }
    strncpy(temp, line, i + 1);
    temp[i + 1] = '\0';

    if(strlen(temp) == 0){
        SET_ERROR("ERROR MISSING STRING")
    }
    if(temp[0] != '\"'){
        SET_ERROR("ERROR STRING NOT OPEN")
    }
    if(temp[strlen(temp) - 1] != '\"'){

        SET_ERROR("ERROR STRING NOT CLOSED")
    }

    /*
     * Remove the quotes from the string
     */
    strcpy(st->dir_or_inst.dir.dir_info.str, temp + 1);
    st->dir_or_inst.dir.dir_info.str[strlen(st->dir_or_inst.dir.dir_info.str) - 1] = '\0';

    return true;
}

/*
 * Extract a list of integers to an array.
 * The result is stored in st->dir_or_inst->dir->dir_info->num_arr->int_arr.
 * Example of a list of integers: "1,2,3,4,5,6,7"
 */
static int extract_ints(struct syntax_tree *st, const char *line){
    char str_num[MAX_LABEL_SIZE + 1];
    int i = 0, num;
    escape_white_chars(&line);

    if(strlen(line) == 0){
        SET_ERROR("ERROR NOT ENOUGH VARIABLES")
    }

    for (; i < MAX_VARS && strlen(line) > 0; ++i) {
        get_first_parameter(st, line, str_num);

        if((strlen(str_num) == 0 && strlen(line) != 0 ) || !is_a_number(str_num)){
            SET_ERROR("ERROR INPUT ISN'T A NUMBER OR INPUT IS EMPTY")
        } else {
            /*
             * No more parameters to read
             */
            if(strlen(str_num) == 0)
                break;

            num = atoi(str_num);

            if(num < -2048 || num > 2047) {
                st->lineType = error;
                strcpy(st->error_message, "ERROR NUMBER OUT OF RANGE");
                num = -1;
            }

            st->dir_or_inst.dir.dir_info.num_arr.int_arr[i] = num;
            st->dir_or_inst.dir.dir_info.num_arr.arr_length = i + 1;
            /*
             * Escape the comma
             */
            if(strlen(line) != strlen(str_num)){
                line += 1;
            }
            line += strlen(str_num);;
        }
    }

    return -1;
}

/*
 * Extract the source and destination parameters from line to st->dir_or_inst->inst->one_or_two_parameters->two_parameters.
 * Example of two parameters: "src,des"
 */
static bool extract_two_parameters(struct syntax_tree *st, const char *line) {
    char parameter[MAX_LABEL_SIZE + 1];
    int comma_index;

    /*
     * First parameter
     */
    get_first_parameter(st, line, parameter);
    line += strlen(parameter) + 1;

    if (strlen(parameter) == 0) {
        SET_ERROR("ERROR MISSING PARAMETER")
    } else {
        strcpy(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.src_parameter, parameter);

        /*
         * Second parameter
         */
        comma_index = char_at(line, ',');
        if (comma_index != -1) {
            SET_ERROR("ERROR TOO MANY VARIABLES")
        } else {
            escape_white_chars(&line);

            get_first_parameter(st, line, parameter);
            strcpy(st->dir_or_inst.inst.one_or_two_parameters.two_parameters.des_parameter, parameter);
        }

    }

    return true;
}

/*
 * Extract the destination parameter from line to st->dir_or_inst->inst->one_or_two_parameters->one_parameter->parameter.
 * Example of parameter: "des"
 */
static int extract_single_parameter(struct syntax_tree *st, const char *line){
    if(char_at(line, ',') != -1){
        SET_ERROR("ERROR TOO MANY VARIABLES")
    } else if(strlen(line) == 0) {
        SET_ERROR("ERROR MISSING VARIABLE")
    }else {
        strcpy(st->dir_or_inst.inst.one_or_two_parameters.one_parameter.parameter, line);
    }
    return true;
}

/*
 * Extract a label from line to st->dir_or_inst->dir->dir_info->label.
 * Example of parameter: "label"
 */
static bool extract_single_label(struct syntax_tree *st, const char *line){
    if(char_at(line, ',') != -1){
        SET_ERROR("ERROR TOO MANY PARAMETERS")
    }
    if(is_label(st, line)){
        strcpy(st->dir_or_inst.dir.dir_info.label, line);
    } else if(strlen(line) == 0){
        SET_ERROR("ERROR MISSING LABEL")
    } else {
        SET_ERROR("ERROR NOT A VALID LABEL")
    }

    return true;
}

/*
 * The function navigates the syntax tree and calls the correct extracting function based on the type of input.
 */
static bool get_parameters_by_type(struct syntax_tree *st, const char *line) {
    if(st->lineType == directive){
        if(st->dir_or_inst.dir.dirType == string){
            extract_string(st, line);
        } else if(st->dir_or_inst.dir.dirType == data){
            extract_ints(st, line);
        } else if(st->dir_or_inst.dir.dirType == entry ||
                  st->dir_or_inst.dir.dirType == external){
            extract_single_label(st, line);
        }
    } else if (st->lineType == instruction){
        /*
         * Double parameter
         */
        if(st->dir_or_inst.inst.opCode == op_code_mov ||
           st->dir_or_inst.inst.opCode == op_code_cmp ||
           st->dir_or_inst.inst.opCode == op_code_add ||
           st->dir_or_inst.inst.opCode == op_code_sub ||
           st->dir_or_inst.inst.opCode == op_code_lea){
            extract_two_parameters(st, line);
        }
            /*
             * Single parameter
             */
        else if(st->dir_or_inst.inst.opCode == op_code_not ||
                st->dir_or_inst.inst.opCode == op_code_clr ||
                st->dir_or_inst.inst.opCode == op_code_inc ||
                st->dir_or_inst.inst.opCode == op_code_dec ||
                st->dir_or_inst.inst.opCode == op_code_jmp ||
                st->dir_or_inst.inst.opCode == op_code_bne ||
                st->dir_or_inst.inst.opCode == op_code_red ||
                st->dir_or_inst.inst.opCode == op_code_prn ||
                st->dir_or_inst.inst.opCode == op_code_jsr){
            extract_single_parameter(st, line);
        }
            /*
             * Non-parameter
             */
        else if(st->dir_or_inst.inst.opCode == op_code_rts ||
                st->dir_or_inst.inst.opCode == op_code_stop){
            if(line != NULL && strlen(line) != 0){
                SET_ERROR("ERROR TOO MANY PARAMETERS")
            }
        }
    }

    return true;
}

/*
 * Classify the syntax tree line type:
 * - directive: starts with a dot, optional inputs - ".string", ".data", ".entry", ".extern"
 * - instruction: "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
 * - comment: starts with ';', e.g., ";comment line"
 */
static int set_type_of_input(struct syntax_tree *st, char *input){
    typedef struct {
        char input[MAX_LABEL_SIZE + 1];
        int i;
    }type_command;
    type_command dc[] = {
            {".string", string},
            {".data", data},
            {".entry", entry},
            {".extern", external}
    };
    type_command ic[] = {
            {"mov", op_code_mov},
            {"cmp", op_code_cmp},
            {"add", op_code_add},
            {"sub", op_code_sub},
            {"not", op_code_not},
            {"clr", op_code_clr},
            {"lea", op_code_lea},
            {"inc", op_code_inc},
            {"dec", op_code_dec},
            {"jmp", op_code_jmp},
            {"bne", op_code_bne},
            {"red", op_code_red},
            {"prn", op_code_prn},
            {"jsr", op_code_jsr},
            {"rts", op_code_rts},
            {"stop", op_code_stop}
    };
    int i;

    if(input == NULL || input[0] == '\n'){

    }else if(input[0] == '.'){
        st->lineType = directive;
        st->dir_or_inst.dir.dirType = -1;

        i = 0;
        for (; i < sizeof(dc)/ sizeof(type_command); i++) {
            if(strcmp(dc[i].input, input) == 0){
                st->dir_or_inst.dir.dirType = dc[i].i;
                break;
            }
        }
        if(st->dir_or_inst.dir.dirType == -1){
            SET_ERROR("ERROR INVALID DECLARATION")
        }

    } else if(input[0] == ';') {
        st->lineType = comment;
    } else{
        st->lineType = instruction;


        i = 0;

        for (; i < sizeof(ic) / sizeof(type_command); ++i) {
            if(strcmp(ic[i].input, input) == 0){
                st->dir_or_inst.inst.opCode = ic[i].i;
                break;
            }
        }

        if(i == sizeof(ic) / sizeof(type_command)){
            SET_ERROR("ERROR UNRECOGNIZED COMMAND")
        }
    }

    return -1;
}

/*
 * Initialize all the fields of the syntax tree based on the input (line).
 * Set the declaration label, type of the command line (directive or instruction), and the variables of the current command.
 */
void build_syntax_tree_from_line(struct syntax_tree *st, char *line){
    char word[MAX_LABEL_SIZE + 1];
    clear_syntax_tree(st);

    strcpy(st->label, "");
    st->lineType = 0;
    if(line[strlen(line) - 1] == '\n'){
        line[strlen(line) - 1] = '\0';
    }

    escape_white_chars((const char**)&line);

    /*
     * Check if the sentence starts with a label
     */
    get_first_word(line, word);
    line += strlen(word);
    escape_white_chars((const char**)&line);

    if(is_declaration_label(word)){
        word[strlen(word) - 1] = '\0';
        if(!is_label(st, word)){
            return;
        }

        if(strcmp(line, "") == 0) {
            st->lineType = error;
            strcpy(st->error_message, "ERROR ONLY LABEL DECLARED");
            return;
        }

        strcpy(st->label, word);
        get_first_word(line, word);

        line += strlen(word);
        escape_white_chars((const char**)&line);
    }

    /*
     * Set if it's a directive or instruction
     */
    set_type_of_input(st, word);

    /*
     * Remove white characters and handle errors such as missing comma or missing parameter
     */
    if(!(st->lineType == directive && st->dir_or_inst.dir.dirType == string))
        remove_white_char_from_str(st, line);

    get_parameters_by_type(st, line);

    syntax_validation(st);
}

void reset_syntax_tree(struct syntax_tree *st) {
    st->label[0] = '\0'; /* Reset the label field */
    st->error_message[0] = '\0'; /* Reset the error_message field */
    st->lineType = empty; /* Reset the lineType field */
    st->dir_or_inst.dir.dirType = non;
    st->dir_or_inst.inst.opCode = non;

    /*
     * Reset the additional fields in the sub structs
     */
    if (st->lineType == directive) {
        st->dir_or_inst.dir.dirType = entry;
        st->dir_or_inst.dir.dir_info.label[0] = '\0';
        st->dir_or_inst.dir.dir_info.str[0] = '\0';
        /*
         * Reset the additional fields in the directive
         */
    } else if (st->lineType == instruction) {
        st->dir_or_inst.inst.opCode = op_code_mov;
        /*
         * Reset the additional fields in the instruction
         */
    }
}

void clear_syntax_tree(struct syntax_tree *st){
    if(st != NULL){
        reset_syntax_tree(st);
    }
}
