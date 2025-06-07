#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "macro.h"
#include "memorylist.h"
#include "symboltable.h"
#include "firstpass.h"
#include "secondpass.h"
#include "binarytohexa.h"

extern macro *top_macro;
extern memory_word *top_word;
extern memory_word *bottom_word;
extern symbol *top_symbol;

int main(int argc, char *argv[]);
void close_file(FILE **file);
void cleanup(FILE *source_file, FILE *macro_file, FILE *hexa_file, macro **top_macro, memory_word **top_word, memory_word **bottom_word, symbol **top_symbol);

#endif