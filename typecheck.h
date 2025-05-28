#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "macro.h"
#include "symboltable.h"

int iscommand(const char *name);
int isdeclaration(const char *name);
int islabel(char label_name[], char second_word[], int address_num, int code_line, macro **top_macro, symbol **top_symbol);

#endif