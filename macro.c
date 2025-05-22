#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"

#define FALSE 0
#define TRUE 1
#define ERROR -1

/*this function gets two files and a pointer to a linked list. it goes line by line on the first file. when it detect a macro definition it saves it in the linked list. when it detect a macro declaration it replaces it with the macro data from the linked list in the second file. if it detect an error it returns 0 to the main function. if successful the second file will be updated with the macros data.*/
int macro_search(FILE *source_file, FILE *macro_file, macro **top_macro)
{
	int code_line = 0;				/*saves the line we are checking (for printing errors with the problematic line)*/
	int error_flag = 0;
	int macro_result;
	char current_line[MAX_LINE];			/*a string to store the line we're checking*/
	while (fgets(current_line, MAX_LINE, source_file) != NULL)  	/*we'll go through every line until end of file*/
	{
		code_line++;
		if (!macro_check(*top_macro, current_line, macro_file)) 				/*a function to check if the line beging with a macro name*/
		{	
			macro_result = macro_push(top_macro, current_line, source_file, &code_line);	 /*a function to check if the line begins with a macro definition. if it is we will copy the macro data to the file. if the function returns -1 the function discovered an error.*/
			if (macro_result == ERROR)	
				error_flag = 1;
			if (!macro_result)
				fputs(current_line, macro_file);  				/*if it's not macro related we'll copy the line as it is*/
				fflush(macro_file);
		}
	}	
	if (error_flag == 1)
		return FALSE;
	return TRUE;
}
	

/*a function that checks if we have a macro declaration and updates the file according to it*/
int macro_check(macro *top_macro, char current_line[], FILE *macro_file)
{
	char first_word[MAX_WORD];
	sscanf(current_line, "%s" , first_word); 			/*save the first word of the line*/
	while (top_macro != NULL)		 			/*we'll go through every object in the linked list*/
	{
		if (strcmp(first_word, top_macro->name) == 0) 		/*first word is a saved mcro name from the linked list*/
		{
			fputs(top_macro->data, macro_file);		/*we'll copy the data of the macro to the file*/
			fflush(macro_file);
			return TRUE;				
		}
		top_macro = top_macro->next;				/*move to the next object in the linked list*/
	}
	return FALSE;							/*the first word wasnt a saved macro name, return false*/
}
	

/*a function that checks if we have a macro setting, and saving it in a linked list*/	
int macro_push(macro **top_macro, char current_line[], FILE * source_file, int *code_line) 
{
	char first_word[MAX_WORD], second_word[MAX_WORD];
	sscanf(current_line, "%s %s" , first_word, second_word); 	/*save the first and second word of the line*/
	if (strcmp(first_word, "mcro") == 0)  /*if the first word is mcro and the second word is not a command or declaration we'll update the linked list*/
	{
		if (iscommand(second_word) || isdeclaration(second_word))
		{
			fprintf(stderr, "Error: macro name is a command/declaration in line %i in .as file.\n", *code_line);
			return ERROR;
		}
		else
		{
			macro *new_macro = (macro *)malloc(sizeof(macro)); 			/*new space for the macro object*/
			if (new_macro == NULL) 							/*checking if memory allocation was succesful*/
			{
				fprintf(stderr, "Error: Memory allocation failed.\n");
				return ERROR;
		    	}
			strcpy(new_macro->name, second_word);	/*we'll update the object's name with the second word which is the macro name*/
			new_macro->data[0] = '\0'; 				/*setting the object's data and next field to null*/
	 	        new_macro->next = NULL;
			while (fgets(current_line, MAX_LINE, source_file) != NULL) 	 /*scan line by line until the end of the macro setting*/
			{
				code_line++;   			
				sscanf(current_line, "%s", first_word);
		   		if (strcmp(first_word, "mcroend") == 0)		/*if the first word of a line is mcroend we've reached the end of the macro*/
		        		break;
		    		strcat(new_macro->data, current_line);		/*adding the line to the object's data field*/
			}
		new_macro->next = *top_macro;		/*updating the object's next field with a pointer to the macro object top_macro (the previous macro object we made or a null one if this was the first)*/
		*top_macro = new_macro;			/*updating the top_macro pointer to point to the new macro object we made*/
		return TRUE;			
		}	
	}
	return FALSE;				/*not a macro definition, return false*/
}
	
		


void free_macro_stack(macro **top_macro)   /*a function to free the memory allocation of the linked list*/
{
	macro *temp;
	while (*top_macro != NULL) 
	{
        	temp = *top_macro;
        	*top_macro = (*top_macro)->next;
        	free(temp);
	}
	*top_macro = NULL;
}
	