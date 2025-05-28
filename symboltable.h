#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#define MAX_LINE 1000


typedef struct symbol_table			/*creating a linked list to store the symbols*/
	{
		char symbol_name[MAX_LINE];			/*symbol's name*/
		char symbol_type[MAX_LINE];			/*symbol's type*/
		int entry;					/*if the symbol is an entry the field will be 1 (TRUE)*/
		int symbol_address;				/*address of the symbol*/
		struct symbol_table *next;	
	}symbol;	

int add_symbol_to_table(char name[], int address_num, int code_line, char type[], symbol **top_symbol);
void free_symbol_table(symbol **top_symbol);

#endif

