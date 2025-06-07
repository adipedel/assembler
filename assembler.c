#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

#define EXTENSION_AS ".as"
#define EXTENSION_AM ".am"
#define EXTENSION_OP ".op"
#define SUCCESS 1
#define MAX_NAME 100

/*student name: Adi Pedel
assembler project:
the main function opens the '.as' file with the prefix in argv[i] and creates '.am' and '.op' files with the same prefix. It sends the '.as' file to the function macro_search, a function that search for macro definitions and declarations in the file and updates them in the '.am' file.If successful, it will send the '.am' file to the function firstpass. This function is in charge of translating every word of information to binary code according to the assembler rules and based on the command or declaration and the operands with supporting functions.The function also detects errors and would not continue to the next step if they were found. If successful main will call secondpass, a function which updates the binary code of the labels' words of information. If succeessfull main will call the function binary_to_hexa with the '.op' file. The function will write each word of information address in decimal and code in hexa.*/
int main(int argc, char *argv[])
{
	int i;
	char file_name_as[MAX_NAME], file_name_am[MAX_NAME], file_name_op[MAX_NAME];
	macro *top_macro = NULL;					/*a pointer to the top macro list*/
	memory_word *top_word = NULL;					/*a pointer to the top memory list*/
	memory_word *bottom_word = NULL;				/*a pointer to the botoom of the memory list*/
	symbol *top_symbol = NULL;					/* pointers to the top and bottom symbol list*/
	FILE *source_file = NULL, *macro_file = NULL, *hexa_file = NULL;

	for (i = 1; i < argc; i++)
	{
		strcpy(file_name_as, argv[i]);  				/*copy the file name from agrv[i]*/
		strcat(file_name_as, EXTENSION_AS);  				/*add to the string the finish: ".as"*/
		source_file = fopen(file_name_as, "r");				/*opening the file*/
		if (!source_file) 
		{
			perror("Error opening source file");
			continue;
		}
		strcpy(file_name_am, argv[i]);  				/*copy the file name from agrv[i]*/
		strcat(file_name_am, EXTENSION_AM);  				/*add to the string the finish: ".am"*/
		macro_file = fopen(file_name_am, "w+");				/*create a new file for the macro changes*/

            	if (!macro_file) 
		{
			perror("Error creating macro file");
			cleanup(source_file, NULL, NULL, &top_macro, &top_word, &bottom_word, &top_symbol);
			continue;
		}
		if (!(macro_search(source_file, macro_file, &top_macro))) 			/*sending the files to the macro function*/
		{
			cleanup(source_file, macro_file, NULL, &top_macro, &top_word, &bottom_word, &top_symbol);
			continue;
		}
		fseek(macro_file, 0, SEEK_SET);
		if (!(firstpass(macro_file, &top_macro, &top_symbol, &top_word, &bottom_word)))		/*sending the file to firstpass funnction*/
		{
			cleanup(source_file, macro_file, NULL, &top_macro, &top_word, &bottom_word, &top_symbol);
			continue;
		}
		if (!(secondpass(&top_word, &top_symbol, argv[i])))				/*calling the function secondpass*/
		{
			cleanup(source_file, macro_file, NULL, &top_macro, &top_word, &bottom_word, &top_symbol);
			continue;
		}
		strcpy(file_name_op, argv[i]);  				/*copy the file name from agrv[i]*/
		strcat(file_name_op, EXTENSION_OP);  				/*add to the string the finish: ".op"*/
		hexa_file = fopen(file_name_op, "w");			/*create a new file for the macro changes*/
            	if (!hexa_file) 
		{
			perror("Error creating hexa file");
			cleanup(source_file, macro_file, NULL, &top_macro, &top_word, &bottom_word, &top_symbol);
			continue;
		}
		binary_to_hexa(file_name_op, top_word);				/*sending the file name to binary_to_hexa function*/
		cleanup(source_file, macro_file, hexa_file, &top_macro, &top_word, &bottom_word, &top_symbol);
	}
	return SUCCESS;
}


void cleanup(FILE *source_file, FILE *macro_file, FILE *hexa_file, macro **top_macro, memory_word **top_word, memory_word **bottom_word, symbol **top_symbol) 
{
	if (source_file) 
		fclose(source_file);
	if (macro_file) 
		fclose(macro_file);
	if (hexa_file) 
		fclose(hexa_file);
	if (top_macro) 
		free_macro_stack(top_macro);
	if (top_word && bottom_word) 
		free_memory_stack(top_word, bottom_word);
	if (top_symbol) 
		free_symbol_table(top_symbol);
}




void close_file(FILE **file) 
{
	if (*file) 
	{
		fclose(*file);
		*file = NULL;
	}
}