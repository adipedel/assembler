#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>

#define MAX_WORD 100
#define MAX_LINE 1000
#define MAX_DATA 10000


typedef struct macro_stack	/*Creating a linked list to store the macros*/
	{
		char name[MAX_LINE];		/*macro's name*/
		char data[MAX_DATA];		/*macro's settings*/
		struct macro_stack *next;	
	} macro;

int macro_search(FILE *source_file, FILE *macro_file, macro **top_macro);
int macro_check(macro *top_macro, char current_line[], FILE *macro_file);
int macro_push(macro **top_macro, char current_line[], FILE * source_file, int *code_line);
void free_macro_stack(macro **top_macro);

#endif