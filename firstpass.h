#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include "macro.h"
#include "symboltable.h"
#include "memorylist.h"
#include "typecheck.h"



int firstpass(FILE *current_file, macro **top_macro, symbol **top_symbol, memory_word **top_word, memory_word **bottom_word);

#endif