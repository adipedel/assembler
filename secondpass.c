#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "secondpass.h"

/*a function that recieves pointers to the memory linked list, and the symbols linked list. the function goes through the memory linked list and if it finds a word with no binary code it needs to update it based on the label the word is for. the function goes through the symbols linked list and when it finds a symbol name that matches the label name of the word it can update the binary code based on the label address. the function returns TRUE if it succeeded and FALSE if it found an error*/
int secondpass(memory_word **top_word, symbol **top_symbol, char *file_name)
{
	memory_word *current_word;
	symbol *current_label;
	int address_distance, address;
	int error_flag = 0;
	char file_name_ext[MAX_NAME],file_name_ent[MAX_NAME];
	FILE *extern_file = NULL, *entry_file = NULL;

	strcpy(file_name_ext, file_name);  				/*copy the file name prefix*/
	strcat(file_name_ext, EXTENSION_EXT);  				/*add to the string the finish: ".ext"*/
	strcpy(file_name_ent, file_name);  				/*copy the file name prefix*/
	strcat(file_name_ent, EXTENSION_ENT);  				/*add to the string the finish: ".ent"*/
	current_label = *top_symbol;		


	while (current_label != NULL) 		/*searching for the label in the symbol table.*/
	{
		if (current_label->entry == 1) /*if the symbol entry flag is on we'll add it to the entry file*/
		{
			entry_file = fopen(file_name_ent, "a+");				/*create /append the entry file*/
			if (!entry_file) 
			{
				perror("Error opening entry_file");
				return FALSE;
			}
			fprintf(entry_file, "%s %07d\n", current_label->symbol_name ,current_label->symbol_address); 	/*writing the entry label name and its address*/
		}
		current_label = current_label->next;	
	}

	current_word = *top_word;
	while (current_word != NULL) 				/*going through the words in the memory linked list*/
	{
		if (current_word->binary_code == 0 && !(strcmp(current_word->source, ".string") == 0))		/*if binary_code is null (and it's not the null char of a string), it's a word of informtion of a label and we need to fill it in */
		{
			current_label = *top_symbol;		
			while (current_label != NULL) 		/*searching for the label in the symbol table.*/
			{
				if ((strcmp(current_label->symbol_name, current_word->source) == 0) || (current_word->source[0] == '&' && strcmp(current_label->symbol_name, current_word->source +1) == 0))		/*found the label*/		
				{	
					if (strcmp(current_label->symbol_type, "external") == 0) /*if the symbol type is external we'll add it to the external file*/
					{
						extern_file = fopen(file_name_ext, "a+");				/*create /append the external file*/
						if (!extern_file) 
						{
							perror("Error opening entry_file");
							return FALSE;
						}
						current_word->binary_code = 1;				 /*updating the label address(1 for external label)*/
						fprintf(extern_file, "%s %07d\n", current_word->source ,current_word->memory_address); 	/*writing the external label name and the address it's being used as an operand to the file*/
					}
					else if (current_word->source[0] == '&')	/*relative addressing*/
					{
						address_distance = (current_label->symbol_address - (current_word->memory_address - 1)) & ADDRESS_MASK;
						/*the distance between the command with the relative addressing address and the label address in binary (in 21 bits)*/
						address_distance <<= SHIFT_AMOUNT; 	
						address_distance |= ABSOLUTE_BIT;		/*shift left and updating the 'A' bit*/
						current_word->binary_code = address_distance;	/*the adress of the label in binary (in 24 bits)*/
					}
					else
					{
						address = current_label->symbol_address & ADDRESS_MASK; 	/*the label address in binary in 21 bits*/
						address <<= SHIFT_AMOUNT; 	
						if (strcmp(current_label->symbol_type, ".external") == 0)
							address |= EXTERNAL_BIT;				/*updating the 'E' bit*/
						else
							address |= RELOCATABLE_BIT;				/*updating the 'R' bit*/
						current_word->binary_code = address;			/*the adress of the label in binary (in 24 bits)*/
		
					}
					break;  		/*label found, exiting loop */	
				}
				current_label = current_label->next;
			}
			if (current_label == NULL)
			{
				fprintf(stderr, "Error: the label %s wasn't defined\n", current_word->source);		/*the label wasnt found*/
				error_flag = 1;
				current_word = current_word->next;  			/*move to the next memory word entry*/
				continue;
			}
		}
		current_word = current_word->next;  			/*move to the next memory word entry*/
	}
	if (error_flag == 1)
		return FALSE;
	return TRUE;
}