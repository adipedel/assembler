#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

#define TRUE 1
#define ERROR -1
#define MAX_LINE 1000

/*a function that recieves a symbol name, address number, code line (for error prints) and a ointers to the symbol linked list. the function adds the symbol to the symbols linked list if its not already there. the function returns -1 if it detects an error. */
int add_symbol_to_table(char name[], int address_num, int code_line, char type[], symbol **top_symbol)
{
	symbol *temp, *new_symbol;
	new_symbol = (symbol *)malloc(sizeof(symbol));				/*new space for the memory_stack object*/
	if (new_symbol == NULL) 							/*checking if memory allocation was succesful*/
	{
        	fprintf(stderr, "Error: Memory allocation failed.\n");
		return ERROR;
    	}
	new_symbol->next = NULL;
	if (*top_symbol == NULL) 
    		*top_symbol = new_symbol;  						/*if its the first symbol, the symbol pointer will point to it*/
	else
	{
		temp = *top_symbol;					
		while (temp != NULL)						/*going through the symbols linked list  until we get to the bottom*/
		{
			if (temp->entry == 1 && strcmp(temp->symbol_name, name) == 0)	/*the entry label exists, we will update the address field*/
			{
				temp->symbol_address = address_num;				/*updating the symbol address in the new symbol object*/
				return TRUE;
			}
			if (strcmp(type, ".entry") == 0 && strcmp(temp->symbol_name, name) == 0) /*the symbol is was added as a label, we will update it as entry*/
			{
				temp->entry = 1;
				return TRUE;
			}
			if (temp->entry != 1 && !(strcmp(type, ".entry") == 0) && strcmp(temp->symbol_name, name) == 0)	/*we cant have two labels with the same name unless it's an entry (entry is written twice in the code  - declaration as entry and the label itself)*/
			{
				fprintf(stderr, "Error: existing label name in line %d\n", code_line);		
				return ERROR;
			}
			if (temp->next != NULL)
				temp = temp->next;						/*if exist we will continue to check the next label*/
			else
				break;
		}
		temp->next = new_symbol;

	}
	
	strcpy(new_symbol->symbol_name, name);						/*updating the symbol name in the new symbol object*/
	if (strcmp(type, "label") == 0)
	{
		strcpy(new_symbol->symbol_type, "code");				/*updating the symbol type in the new symbol object*/
		new_symbol->symbol_address = address_num;				/*updating the symbol address in the new symbol object*/
	}
	else if (strcmp(type, ".extern") == 0)
	{
		strcpy(new_symbol->symbol_type, "external");				/*updating the symbol type in the new symbol object*/
		new_symbol->symbol_address = 0;						/*updating the symbol address in the new symbol object*/
	}
	else if (strcmp(type, ".entry") == 0)				/*symbol type may change to data when the entry is declared in the code.*/
	{
		strcpy(new_symbol->symbol_type, "code");				/*updating the symbol type in the new symbol object*/
		new_symbol->entry = 1;
	}	
	new_symbol->next = NULL;
	return TRUE; 								/*new symbol was added to the symbol table*/
}



void free_symbol_table(symbol **top_symbol) 			/*a function to free the memory allocation of the linked list*/
{	
	symbol *temp;
	while (*top_symbol != NULL) 
	{
        	temp = *top_symbol;
        	*top_symbol = (*top_symbol)->next;
        	free(temp);
	}
	*top_symbol = NULL;

}