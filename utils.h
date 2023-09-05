#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#define AMOUNT_OF_RESERVED_WORDS 27
#define MAX_MEMORY_SIZE 1024
#define MAX_LABEL_SIZE 31
#define MAX_ERROR_MSG_SIZE 50
#define MAX_LINE_SIZE 80
#define MAX_VARS 20
#define bool int
#define true 1
#define false 0

int char_at(const char *str, char ch);
bool is_not_equal_to_reserved_word(const char *str);
bool is_valid_number(const char *str);
bool is_valid_var(const char *str);
bool is_valid_register(const char *str);
void reverse_string(char* str);
char *decimal_to_binary(int decimalNumber, int n);
int get_num_of_parameters_inst(int op_code);


#endif
