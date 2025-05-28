#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "typecheck.h"

#define FALSE 0
#define TRUE 1
#define ERROR -1

/*a function that recieves a name, and checks if it is a command name. returns TRUE if it is and FALSE if it isn't*/
int iscommand(const char *name)
{
	int i;
	const char *commands[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"}; /*array of chars of commands names*/
	int array_lenght = sizeof(commands) / sizeof(commands[0]);  	/*calculates array lenght*/
	for (i=0; i<array_lenght; i++) 				
	{
		if (strcmp(name, commands[i]) == 0)	/*for every char in the array, we'll check if its eqaual to the string the function got*/
		{
			return TRUE;
		}
	}
	return FALSE; 
}


/*a function that recieves a name and checks if it is a saved word of declaration type. returns TRUE if it is and FALSE if it isn't*/
int isdeclaration(const char *name)
{
	int i;
	const char *declarations[] = {".data", ".string", ".entry", ".extern"};
	int array_lenght = sizeof(declarations) / sizeof(declarations[0]);  	/*calculates array lenght*/
	for (i=0; i<array_lenght; i++) 				
	{
		if (strcmp(name, declarations[i]) == 0)		/*for every char in the array, we'll check if its equal to the string the function got*/
			return 1;
	}
	return 0; 
}


/*a function that recieves a label name, the following word, address number, code line (for error prints) and pointers to the macro and symbol linked lists.
the function checks if its a valid label. returns TRUE if it is and FALSE if it isn't and ERROR if one was discovered*/
int islabel(char label_name[], char second_word[], int address_num, int code_line, macro **top_macro, symbol **top_symbol)
{
	int i;
	int len = strlen(label_name);
	macro *temp_macro;
	if (label_name[len-1] == ':' && !(strcmp(second_word, ".entry") == 0) && !(strcmp(second_word, ".extern") == 0)) /*(ignoring a label before entry/extern)*/
	{
		label_name[strlen(label_name) - 1] = '\0';  	/*removing the char ':' from the label name*/
		if (len > 32 || !(isalpha(label_name[0])) || iscommand(label_name) || strcmp(label_name, "string") == 0 || strcmp(label_name, "data") == 0 || strcmp(label_name, "extern") == 0 || strcmp(label_name, "entry") == 0)
		{
				fprintf(stderr, "Error: existing label name in line %d\n", code_line);		/*we cant have two labels with the same name*/
				return ERROR;
		}
		temp_macro = *top_macro;
		while (temp_macro  != NULL)		 		/*going through the macro linked list*/
		{
			if (strcmp(label_name, temp_macro ->name) == 0) 		/*checking to see if the label name is a macro name*/
			{
				fprintf(stderr, "Error: invalid label name in line %d  in .am file.\n", code_line);	
				return ERROR;
			}
			temp_macro = temp_macro->next;		/*move to the next object in the linked list*/
		}
		for (i = 0; i < len-2; i++)
		{
			if (!(isdigit(label_name[i]) || isalpha(label_name[i])))			/*label name contains a char that is not a letter or a number*/
			{
				fprintf(stderr, "Error: invalid label name in line %d  in .am file.\n", code_line);
				return ERROR;
			}
		}
		add_symbol_to_table(label_name, address_num, code_line, "label", top_symbol);	/*sending the label to the add_symbol_to_table function*/

		return TRUE;
	}
	return FALSE;	
}