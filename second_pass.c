#include <stdlib.h>
#include "second_pass.h"

/*
 * This function takes a string of binary digits and converts them to base 64.
 * It's working on a block of 12 binary digits at a time (half of the full 24-bit
 * block of Base64 encoding), producing two Base64 characters.
 */
char* binaryToBase64Half(const char* binaryStr) {
    static const char* b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char* base64str = (char*)malloc(3);

    unsigned int val = 0;
    int i;

    if (base64str == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL;
    }

    for (i = 0; i < 6; i++) {
        val = (val << 1) | (binaryStr[i] - '0');
    }
    base64str[0] = b64chars[val];

    val = 0;
    for (; i < 12; i++) {
        val = (val << 1) | (binaryStr[i] - '0');
    }
    base64str[1] = b64chars[val];
    base64str[2] = '\0';

    return base64str;
}

/*
* This function creates an object file (.obj) for assembly language.
* The object file will contain the 64base representation of the machine code,
* as well as addresses and other necessary details.
*
* Parameters:
* - is_there_ext_symbols: boolean flag to check if there are external symbols in the code.
* - is_there_ent_symbols: boolean flag to check if there are entry symbols in the code.
* - symbols: linked list of symbol nodes.
* - inst_coded_list: coded instructions linked list.
* - dir_coded_list: coded directives linked list.
* - file_name: name of the file to be created.
*/
void obj_file_creator(bool *is_there_ext_symbols, bool *is_there_ent_symbols, struct symbol_list *symbols,
                      struct coded_list inst_coded_list, struct coded_list dir_coded_list,
                      char file_name[]) {
    FILE *obj_file = NULL;
    struct symbol_list *symbol_first = symbols;
    struct coded_node *p = NULL;
    char obj_file_name[31] = "";
    int ic = inst_coded_list.length;
    int dc = dir_coded_list.length;
    int i = 100;


    strcat(obj_file_name, file_name);
    strcat(obj_file_name, ".obj");
    obj_file = fopen(obj_file_name, "w+");
    if (obj_file == NULL) {
        printf("There Was Problem With Open The File %s\n", obj_file_name);
        exit(-1);
    }

    /*
    * Writing the instruction count and directive count to the file.
    */
    fprintf(obj_file, "%d %d\n", ic, dc);

    /*
    * Processing the coded instructions list.
    */
    p = inst_coded_list.head;
    while (p != NULL){
        if(strcmp(p->coded_line, symbol_first->symbol->label) == 0){
            if(symbol_first->symbol->outsource_type == ext){
                /*
                * If the symbol is external, flag it and modify the coded line accordingly.
                */
                *is_there_ext_symbols = true;
                strcpy(p->coded_line,"0000000000");
                strcat(p->coded_line, "01");
            }
            else {
                /*
                * If the symbol is not external, change the coded line to the binary representation of the symbol index.
                */
                strcpy(p->coded_line, decimal_to_binary(index_of_label(symbols, p->coded_line), 10));
                strcat(p->coded_line, "10");
            }

            /*
            * Process the symbol list to check if there are entry symbols and to skip declarationType symbols.
            */
            do {
                symbol_first = symbol_first->next;
                if(symbol_first->symbol->outsource_type == ent)
                    *is_there_ent_symbols = true;
            }while (symbol_first->next != NULL && symbol_first->symbol->appearance_type == declaration);
        }

        fprintf(obj_file, "%s\n", binaryToBase64Half(p->coded_line));
        p = p->next;
        i++;
    }

    /*
    * Processing the coded directives list.
    */
    p = dir_coded_list.head;
    while (p != NULL){
        fprintf(obj_file, "%s\n", binaryToBase64Half(p->coded_line));
        p = p->next;
        i++;
    }

    fclose(obj_file);
}


/*
 * This function creates .ent and .ext files based on the symbols list.
 * It iterates over the list and writes the label and index of each symbol
 * to the corresponding file (ext or ent), depending on the type of the symbol.
 */
void ext_ent_file_creator(bool is_there_ent_symbols, bool is_there_ext_symbols,
                          char file_name[], struct symbol_list *symbols) {
    FILE *ent_file;
    FILE *ext_file;

    char ent_file_name[50] = "";
    char ext_file_name[50] = "";
    char temp[31+4+1];

    if(is_there_ent_symbols){
        strcpy(ent_file_name, file_name);
        strcat(ent_file_name, ".ent");
        ent_file = fopen(ent_file_name, "w+");
        if(ent_file == NULL){
            printf("There Was Problem With Open The File %s\n", ent_file_name);
            exit(-1);
        }

        if (ent_file == NULL) {
            printf("Error: could not open file %s\n", ent_file_name);
            exit(-1);
        }
    }
    if(is_there_ext_symbols){
        strcpy(ext_file_name, file_name);
        strcat(ext_file_name, ".ext");
        ext_file = fopen(ext_file_name, "w+");
        if(ext_file == NULL){
            printf("There Was Problem With Open The File %s\n", ext_file_name);
            exit(-1);
        }

        if (ext_file == NULL) {
            printf("Error: could not open file %s\n", ent_file_name);
            exit(-1);
        }
    }

    while (symbols != NULL &&
           is_there_ent_symbols &&
           is_there_ext_symbols){
        strcpy(temp,"");
        sprintf(temp, "%s %d\n", symbols->symbol->label, symbols->symbol->labels_index);

        if(symbols->symbol->outsource_type == ent){
            fprintf(ent_file, "%s", temp);
        }
        else if(symbols->symbol->outsource_type == ext){
            fprintf(ext_file, "%s", temp);
        }

        symbols = symbols->next;
    }

    if(is_there_ent_symbols)
        fclose(ent_file);
    if(is_there_ext_symbols){
        fclose(ext_file);
    }
}

/*
 * The 'second_pass' function is the central routine of the second pass in the assembler process.
 * This function handles symbol resolution, binary conversion, and file creation.
 *
 * It begins by creating an object file using the 'obj_file_creator' function.
 * This function also checks and keeps track of any symbols that are either external or entry type.
 *
 * After object file creation, if any symbols were found to be of external or entry type,
 * it invokes 'ext_ent_file_creator' to create respective .ext and .ent files.
 *
 * Parameters:
 * - symbols: a linked list of symbol nodes.
 * - inst_coded_list: a linked list of coded instructions.
 * - dir_coded_list: a linked list of coded directives.
 * - file_name: the name of the input file.
 */
void second_pass(struct symbol_list *symbols, struct coded_list *inst_coded_list,
                 struct coded_list *dir_coded_list, char *file_name){
    /* Boolean flags to track if there are any entry or external symbols */
    bool is_there_ent_symbols = false, is_there_ext_symbols = false;

    /*
     * Call the 'obj_file_creator' function to create an object file (.obj),
     */
    obj_file_creator(&is_there_ext_symbols, &is_there_ent_symbols,
                     symbols, *inst_coded_list, *dir_coded_list, file_name);

    /*
     * If there are any symbols marked as external or entry symbols,
     * call the 'ext_ent_file_creator' function to create .ext and .ent
     * files with the relevant information.
     */
    ext_ent_file_creator(is_there_ent_symbols, is_there_ext_symbols,
                         file_name, symbols);
}




