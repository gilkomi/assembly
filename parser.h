#ifndef ASSEMBLER_PARSER_H
#define ASSEMBLER_PARSER_H

#include "lexer.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>
#include "math.h"
#include "utils.h"
#include <stdlib.h>

bool syntax_validation(struct syntax_tree *st);

#endif
