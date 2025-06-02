#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "firstpass.h"

#define ERROR -1
#define MIN_REGISTER '0'
#define MAX_REGISTER '7'
#define ABSOLUTE_BIT 0x4

/*This function recieves a file and pointers to linked lists. the function goes line by line in the file, detect errors and sends the line to the suited function according to its type. if a function returns -1 it detected an error and we will return 0 to alert the main function.*/
int firstpass(FILE *current_file, macro **top_macro, symbol **top_symbol, memory_word **top_word, memory_word **bottom_word)
{
	char *first_part, *second_part, *remaining, *token, *end_char;					/*strings to store parts of the line*/
	char current_line[MAX_LINE], first_word[MAX_WORD], second_word[MAX_WORD], temp_second_part[MAX_LINE];
	char first_operand[MAX_WORD] = "\0", second_operand[MAX_WORD] = "\0";			
	int i, error_flag = 0, line_len, code_line = 0, label_flag = 0;
	int words_of_information = 0;		/*stores how many words of information came back from a function (if value is -1 the function discovered an error)*/
	int address = 100;						/*keeps count of the current line memory address*/
	int operands_count = 0;						/*operands count to check if the number of operands is correct*/
	int comma_count;						/*commas count to check if the number of commas is correct*/

	while (fgets(current_line, MAX_LINE, current_file) != NULL) 	 /*we'll go through every line until end of file*/
	{
		line_len = strlen(current_line);
		if (line_len > 0 && current_line[line_len - 1] == '\n') 	/*turning the next line char into null char to avoid errors*/
		{
			current_line[line_len - 1] = '\0';
			line_len--;
		}
		code_line++;						/*the line in the code we are reading*/
		comma_count = 0;					/*resetting the comma_count each time*/
		if (line_len == 0) 					/*line is a empty, we will exit the loop and move to the next line.*/
			continue;
		if (sscanf(current_line, "%s %s", first_word, second_word) == 0)  /*saves the first word of the line, if the line only contains spaces and tabs we will exit the loop and move to the next line.*/
			continue;
		if (first_word[0] == ';')				/*line is a comment, we will exit the loop and move to the next line.*/
			continue;
		label_flag = islabel(first_word, second_word, address, code_line, top_macro, top_symbol);     /*a functionn that detects valid labels*/

		if (label_flag == 1)	         /*if we have a label, we'll save it in the symbol table and continue checking the rest of the line */	
		{		
			remaining = strtok(current_line, " \t"); 		/*split the line using strtok, because we dont need the label anymore */
			remaining = strtok(NULL, "");  				/*stores the rest of the line*/
			strcpy(current_line, remaining);			/*updating current_line so we can check the line after the label.*/ 
		}
		else if (label_flag == ERROR)		/*recognized a label pattern but it's not valid, setting the error_flag and moving to the next line*/
		{
			error_flag = 1;
			continue;
		}
		first_part = strtok(current_line, " \t"); 	/*seperating the string to 2 parts. first part is the command or the declaration*/
		second_part = strtok(NULL, "");  						/*second part - rest of the string - the operands*/

		if (second_part != NULL) 
		{
			if (second_part[0] == ',')				/*if the second part starts with a comma, we have an extra comma - setting error_flag*/
			{
				fprintf(stderr, "Error: comma after a command in line %d in .am file.\n", code_line);
				error_flag = 1;
				continue;
			}
			if (current_line[line_len - 1] == ',')			/*if the second part ends with a comma, we have an extra comma - setting error_flag*/
			{
				fprintf(stderr, "Error: comma after last operand in line %d in .am file.\n", code_line);
				error_flag = 1;
				continue;
			}
			if (strcmp(first_part, ".data") == 0)			/*data declaration, sending the line to function data_list*/
			{			
				words_of_information = data_list(address, second_part, code_line, top_word, bottom_word); 		/*words_of_information stores the number of extra words the function detected. we need it to keep track of the memory addresses (each extra word of information is another address in the memory)*/
				if (words_of_information == ERROR)		/*error was detected, setting error_flag and moving to the next line*/
					error_flag =1 ;
				else
					address += (words_of_information);		/*operation was successful, moving to the next line*/
				continue;
			}
			strcpy(temp_second_part, second_part);					/*copying the second_part to another string we can change*/
			token = strtok(temp_second_part, " \t,");			        /*seperating the second part words to check how many operands there are*/	
			if (token != NULL) 							/*we have at least one operand*/
			{			
			    operands_count++;
			    strcpy(first_operand, token);
			    token = strtok(NULL, " \t,");	
			}			
			if (strcmp(first_part, ".string") == 0)			/*string declaration, sending the line to function string_list*/
			{
				if (token != NULL)				/*we have at least 2 operands and a string has only one operand*/
				{
					fprintf(stderr, "Error: too many operands in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				words_of_information = string_list(address, first_operand, code_line, top_word, bottom_word);
				if (words_of_information == ERROR)
					error_flag = 1;
				else
					address += words_of_information;
				continue;
			}
			if (strcmp(first_part, ".entry") == 0)
			{
				if (token != NULL)			/*we have at least 2 operands and an entry has only one operand*/
				{
					fprintf(stderr, "Error: too many operands in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				end_char = second_part + strlen(second_part) - 1;				/*end_char will point to the last char in the string*/
				while (end_char >= second_part && isspace((unsigned char)*end_char)) 		
					*end_char-- = '\0';				/*if end_char is a white space we will replace it with null terminator*/
				add_symbol_to_table(second_part, address, code_line, first_part, top_symbol); /*entry declaration, sending the line to symbol_table*/
				continue;				/*entry declaration, exiting and loop and moving to the next line*/
			}
			if (strcmp(first_part, ".extern") == 0)		
			{
				if (token != NULL)			/*we have at least 2 operands and an extern has only one operand*/
				{
					fprintf(stderr, "Error: too many operands in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				end_char = second_part + strlen(second_part) - 1;				/*end_char will point to the last char in the string*/
				while (end_char >= second_part && isspace((unsigned char)*end_char)) 		
					*end_char-- = '\0';				/*if end_char is a white space we will replace it with null terminator*/
				add_symbol_to_table(second_part, address, code_line, first_part, top_symbol); /*extern declaration, sending the line to symbol_table*/
				continue;
			}
			if (token != NULL)
			{
				operands_count++;
				strcpy(second_operand, token);
				token = strtok(NULL, " \t,");
			}
			if (token != NULL)			/*we have at least 3 operands. only data can have more than 2 operands and we already checked it*/
			{
				fprintf(stderr, "Error: too many operands in line %d\n", code_line);
				error_flag = 1;
				continue;
			}
			if ((first_operand[0]=='r' && (first_operand[1] < MIN_REGISTER || first_operand[1] > MAX_REGISTER || first_operand[2] != '\0')) || (second_operand[0]=='r' && (second_operand[1] < MIN_REGISTER || second_operand[1] > MAX_REGISTER || second_operand[2]!='\0')))
					/*checking to see if we have a register with invalid number */
			{
				fprintf(stderr, "Error: not a valid register number in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
			}
			if (first_operand[0]=='#')    /*immediate operand*/
			{
				char *endptr;
				strtol(first_operand + 1, &endptr, 10); 		/*cheking if its a valid integer after '#'*/
				if (*endptr != '\0')
				{
					fprintf(stderr, "Error: not a valid operand in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
			}

			for (i = 0; second_part[i] != '\0'; i++)		/*checking how many commas there are in the second part*/
			{
				if (second_part[i] == ',')
					comma_count++;
			}
			if (operands_count == 0 && !(strcmp(first_part, "rts") == 0 || strcmp(first_part, "stop") == 0)) /*a command that should have operands, has none*/
			{
				fprintf(stderr, "Error: a command that should have operands has no operands in line %d in .am file.\n", code_line);
				error_flag = 1;
				continue;
				}
			if (strcmp(first_part, "mov") == 0 || strcmp(first_part, "cmp") == 0 || strcmp(first_part, "sub") == 0 || strcmp(first_part, "add") == 0 || strcmp(first_part, "lea") == 0)			 /*these commands should have 2 operands*/
			{
				if (operands_count == 1)	
				{
					fprintf(stderr, "Error: a command that should have two operands has only one operand in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				if (comma_count != 1) 				/*these commands should have 1 comma*/
				{
					fprintf(stderr, "Error: invalid number of commas in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
			 	words_of_information = command_list(address,code_line, first_part, first_operand, second_operand, top_word, bottom_word); /*command line is valid, we will send it to the command_list function to translate to binary and check how many words of information we have*/
				if (words_of_information == ERROR)
					error_flag = 1;
				else
					address+=words_of_information;
				continue;
			}
			if (strcmp(first_part, "clr") == 0 || strcmp(first_part, "not") == 0  || strcmp(first_part, "inc") == 0 || strcmp(first_part, "dec") == 0 || strcmp(first_part, "jmp") == 0 || strcmp(first_part, "bne") == 0 || strcmp(first_part, "jsr") == 0 || strcmp(first_part, "red") == 0 || strcmp(first_part, "prn") == 0)
			{					/*these commands should have 1 operand*/
				if (operands_count == 2)						
				{
					fprintf(stderr, "Error: a command that should have one operand has two operands in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				if (comma_count != 0)				/*these commands should have 0 commas*/
				{
					fprintf(stderr, "Error: invalid number of commas in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				words_of_information = command_list(address,code_line, first_part, first_operand, second_operand,top_word, bottom_word);
				if (words_of_information == ERROR)
					error_flag = 1;
				else
					address+=words_of_information;
				continue;
			}
			if (strcmp(first_part, "rts") == 0 || strcmp(first_part, "stop") == 0)
			{				/*these commands should have 0 operands*/
				if (operands_count>0)					
				{
					fprintf(stderr, "Error: a command that should have no operands has operands in line %d  in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				if (comma_count != 0)				/*these commands should have 0 commas*/
				{
					fprintf(stderr, "Error: invalid number of commas in line %d in .am file.\n", code_line);
					error_flag = 1;
					continue;
				}
				words_of_information = command_list(address,code_line, first_part, first_operand, second_operand,top_word, bottom_word);
				if (words_of_information == ERROR)
					error_flag = 1;
				else
					address+=words_of_information;
				continue;
			}
		}
		else			/*if second_part is NULL it should be a command with no operands and we'll update the memoey_word here*/
		{
			if (strcmp(first_part, "rts") == 0 || strcmp(first_part, "stop") == 0)
			{
				no_operands_command(first_part, address, top_word, bottom_word);
				address++;
				continue;
			}
		}
		fprintf(stderr, "Error: invalid line in line %d in .am file.\n", code_line); /*the line has an error we didnt detect (like a mispelled command)*/
		error_flag = 1;
		continue;
	}
	
	if (error_flag == 1)			/*if we detected an error, we will return 0 to the main function*/
		return 0; 		
	else
		return 1;
}