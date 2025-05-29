#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "memorylist.h"

/*a function that recieves the current line we are checking and its address, the current code line (for errors prints), and pointers to the memory linked list.       the function enters the words of information of the data into the memory linked list. it returns the extra words of information.*/
int data_list(int current_address, char *current_line, int code_line, memory_word **top_word, memory_word **bottom_word)
{
	char temp_current_line[MAX_LINE];						/*a temp string to check the curent line - the data*/
	char *token, *endptr;
	int numbers_count = 0, comma_count = 0, i, value;
	memory_word *new_word;
	strcpy(temp_current_line, current_line);					/*copying the line string to the temp string*/
	token = strtok(temp_current_line, " \t,");					/*seperating the second part (of numbers)*/
	if (token == NULL)
	{
		fprintf(stderr, "Error: no integers in .data in line %d\n", code_line);
            	return ERROR;
	}
	while (token != NULL)								/*checking how many numbers there are*/
	{
		new_word = allocate_memory_word();					/*calling the function allocate_memory_word for allocation*/
		if (new_word == NULL)
			return ERROR;
		if (*top_word==NULL)							/*if it's the first the top_word and bottom_word pointers will point to it*/
		{				
			*top_word=new_word;
			*bottom_word=new_word;
		}
		else									/*if its not the first we will add them in the bottom of the link list*/
		{
			(*bottom_word)->next = new_word;
			*bottom_word = new_word;
		}
		(*bottom_word)->next = NULL;
		numbers_count++;							/*keep count of the number of the words of information*/
		value = strtol(token, &endptr, 10);	/*value stores the integer (if it detect '+', '-' chars it will update the int afterward accordingly)*/
		if (*endptr != '\0') 		/*if *endptr != '\0', there was a char in token that wasnt a part of an integer. that means the number is not valid*/
		{
            		fprintf(stderr, "Error: invalid number in line %d  in .am file.\n", code_line);
            		return ERROR;
		}
		(*bottom_word)->memory_address = current_address;
		current_address++;							/*each word of information is another memory address*/
		strcpy((*bottom_word)->type, "data");					/*data type*/
		strcpy((*bottom_word)->source, ".data");					/*data type*/
		(*bottom_word)->binary_code = (value & DATA_MASK); 			/*for data, words of information binary code is the integer in 24 bits*/
		token = strtok(NULL, " \t,");
    	}	
	for (i = 0; current_line[i] != '\0'; i++)					/*checking how many commas there are between the numbers*/
	{			
		if (current_line[i] == ',')
			comma_count+=1;
	}
	if (comma_count != numbers_count-1)						/*there should be 1 comma between every following numbers.*/
	{		
		fprintf(stderr, "Error: invalid number of commas in line %d  in .am file.\n", code_line);
		return ERROR;
	}
	return (numbers_count);  			 /*the number of words of information is the number_count*/
}

/*a function that recieves the memory address we are at, a string, the current code line (for errors prints), and pointers to the memory linked list.
the function takes the string and enters each char's ascii code into the memory stack*/
int string_list(int current_address, char string[], int code_line, memory_word **top_word, memory_word **bottom_word)
{
	int string_len = strlen(string);
	int i, value;
	memory_word *new_word;
	if (string_len < 2)
	{
		fprintf(stderr, "Error: invalid string in line %d  in .am file.\n", code_line);
		return ERROR;
	}
	if (string[0] != '"' || string[string_len-1] != '"')				/*a string must start and end with a quot mark */
	{
		fprintf(stderr, "Error: invalid string in line %d  in .am file.\n", code_line);
		return ERROR;
	}
	for (i = 1; i < string_len-1; i++)					/*going through each char in the string (beside the opening and closing quots marks)*/
	{
		new_word = allocate_memory_word();					/*calling the function allocate_memory_word for allocation*/
		if (new_word == NULL) 
			return ERROR;
		if (*top_word==NULL)							/*if it's the first the top_word and bottom_word pointers will point to it*/
		{				
			*top_word=new_word;
			(*bottom_word)=new_word;
		}
		else
		{
			(*bottom_word)->next = new_word;
			*bottom_word = new_word;
		}
		(*bottom_word)->next = NULL;
		(*bottom_word)->memory_address=current_address;
		current_address++;
		sprintf((*bottom_word)->type, "data");						/*data type*/
		sprintf((*bottom_word)->source, ".string");						/*a string*/
		value = (int)string[i];
		(*bottom_word)->binary_code = (value & DATA_MASK);				/*binary code stores the ascii code of the current char (in 24 bits)*/
	}
	new_word = allocate_memory_word();					/*calling the function allocate_memory_word for allocation*/
	(*bottom_word)->next = new_word;							/*another item in the linked list for the null terminator*/
	*bottom_word = new_word;
	(*bottom_word)->next = NULL;			
	(*bottom_word)->memory_address=current_address;					
	strcpy((*bottom_word)->type, "data");						/*data type*/
	sprintf((*bottom_word)->source, ".string");						/*a string*/
	(*bottom_word)->binary_code = (0 & DATA_MASK);					/*ascii code for the null terminator*/
	return (string_len - 1); 				/*the number of words of information is the number of chars in the string (not including the quoats because we dont write them) including the null terminator which is a word of information*/
}


/*a function that recieves the memory address we are at, a line with a command and operands, the command, the current code line (for errors prints), the first and second operands and pointers to the memory linked list. the function defined the binary code of the command in the linked list based on the command and operands, and if needed also the binary code for extra words of information (based on the operands type). it also checks for errors and mismatch between the command and operands.*/
int command_list(int current_address,int code_line, char command[], char first_operand[], char second_operand[], memory_word **top_word, memory_word **bottom_word)
{
	memory_word *new_word, *command_word;
	int i;
	int words_of_information = 1; 	/*stores the words of information number including the command itself (if value is -1 the function discovered an error)*/
	int words_check = 0;		/*stores how many words of information came back from a function (if value is -1 the function discovered an error)*/
	unsigned int opcode = 0;
	int command_opcode = 0;
	char *command_list[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
	int values[] = {0, 1, 2, 2, 4, 5, 5, 5, 5, 9, 9, 9, 12, 13, 14, 15};
	
	new_word = allocate_memory_word();					/*calling the function allocate_memory_word for allocation*/
	if (new_word == NULL) 
		return ERROR;
	if (*top_word == NULL)							/*if it's the first the top_word and bottom_word pointers will point to it*/
	{				
		*top_word = new_word;
		*bottom_word = new_word;
		command_word = new_word;					/*a pointer to the command word*/
	}
	else
	{
		(*bottom_word)->next = new_word;					/*if its not the first word, we will add it to the bottom of the linked list*/
		*bottom_word = new_word;
		command_word = new_word;
	}
	command_word->memory_address = current_address;			
	strcpy(command_word->type, "code"); 					/*code type*/
	strcpy(new_word->source, command);					/*writing the command in the source section of the word*/		

	for (i = 0; i < MAX_OPCODE; i++) 					/*going through the command_list to check which command we are checking*/
	{
        	if (strcmp(command, command_list[i]) == 0)
		{
			command_opcode = values[i];
			opcode = values[i];					/*saving the command opcode based on the command_list*/
			break;
		}
	}
	
	opcode &= OPCODE_MASK;							/*a mask to make sure the opcode is 6 bits*/
	opcode <<= OPCODE_SHIFT;						/*left shift 18 because the opcode is stored in bits 18-23*/
	opcode |= ABSOLUTE_BIT;							/*in the first word of every command bit 'A' is on*/

	if (command_opcode == 2)					/*updating funct based on the command*/
	{
		if (strcmp(command, "add") == 0)
			opcode |= (1 << SHIFT_FUNCT);
		if (strcmp(command, "sub") == 0)
			opcode |= (2 << SHIFT_FUNCT);
	}
	if (command_opcode == 5)					/*updating funct based on the command*/
	{
		if (strcmp(command, "clr") == 0)
			opcode |= (1 << SHIFT_FUNCT);
		if (strcmp(command, "not") == 0)
			opcode |= (2 << SHIFT_FUNCT);
		if (strcmp(command, "inc") == 0)
			opcode |= (3 << SHIFT_FUNCT);
		if (strcmp(command, "dec") == 0)
			opcode |= (4 << SHIFT_FUNCT);
	}
	if (command_opcode == 9)					/*updating funct based on the command*/
	{
		if (strcmp(command, "jmp") == 0)
			opcode |= (1 << SHIFT_FUNCT);
		if (strcmp(command, "bne") == 0)
			opcode |= (2 << SHIFT_FUNCT);
		if (strcmp(command, "jsr") == 0)
			opcode |= (3 << SHIFT_FUNCT);
	}
	command_word->binary_code = opcode;
	if (command_opcode >= MIN_TWO_OPERAND_OPCODE && command_opcode <= MAX_TWO_OPERAND_OPCODE)				/*commands with two operands*/ 
	{	
		words_check = registerupdate(first_operand, &opcode, command_opcode, 1, code_line,&command_word, bottom_word);	
		if (words_check == ERROR)
			return ERROR;
		else
			words_of_information += words_check;
		words_check = registerupdate(second_operand, &opcode, command_opcode, 2, code_line, &command_word, bottom_word);
		if (words_check == ERROR)
			return ERROR;
		else
			words_of_information += words_check;
	}
	if (command_opcode >= MIN_ONE_OPERAND_OPCODE && command_opcode <= MAX_ONE_OPERAND_OPCODE)			/*commands with one operand*/ 
	{
		words_of_information += registerupdate(first_operand, &opcode, command_opcode, 2, code_line, &command_word, bottom_word);
		return words_of_information;
	}
	return words_of_information;
}	

void no_operands_command(char *first_part, int address, memory_word **top_word, memory_word **bottom_word)
{
	memory_word *new_word;
	unsigned int opcode = 0;					/*opcode for memory_word*/
	if (strcmp(first_part, "rts") == 0)
		opcode = 14;						/*rts command's opcode*/
	if (strcmp(first_part, "stop") == 0)
		opcode = 15;						/*stop command's opcode*/
	new_word = allocate_memory_word();				/*calling the function allocate_memory_word for allocation*/
	if (*top_word == NULL)						/*if it's the first the top_word and bottom_word pointers will point to it*/
	{				
		*top_word = new_word;
		*bottom_word = new_word;
	}
	else
	{
		(*bottom_word)->next = new_word;			/*if its not the first word, we will add it to the bottom of the linked list*/
		*bottom_word = new_word;
	}
	opcode &= OPCODE_MASK;						/*a mask to make sure the opcode is 6 bits*/
	opcode <<= OPCODE_SHIFT;					/*left shift 18 because the opcode is stored in bits 18-23*/
	opcode |= ABSOLUTE_BIT;						/*in the first word of every command bit 'A' is on*/
	new_word -> binary_code = opcode;				/*updating the memory_word's binary code*/
	new_word -> memory_address = address;				/*updating the memory_word's memory address*/
	strcpy(new_word->type, "code");					/*updating the memory_word's type*/
	new_word -> binary_code = opcode;				/*updating the memory_word's binary code*/
	strcpy(new_word->source, first_part);				/*updating the memory_word's source*/
}

/*a function that recieves an opernd, an opcode, the operand order(if its the first or second), the code line (for error prints) and pointer to the memory linked list. the function updates the binary code of the command in the linked list based on the operand type, and if needed defined the binary code for extra words of information*/
int registerupdate(char operand[], unsigned int *opcode,int command_opcode, int operand_order, int code_line, memory_word **command_word, memory_word **bottom_word)
{
	int words_of_information = 0, number = 0, memory_address;
	char *endptr = NULL;
	memory_word *new_word;

	if (operand[0]=='r')									/*operand is a register*/
		 number = strtol(operand + 1, &endptr, 10);	  				/*convert char to integer*/
	else if (operand[0]=='#')								/*operand is immediate*/
		number = strtol(operand + 1, &endptr, 10);					/*convert char to integer*/
	if (operand[0]=='#' && ((operand_order == 1 && command_opcode == 4) || (operand_order == 2 && command_opcode != 1 && command_opcode != 13))) /*immediate operand can't be use with these commands*/
		goto ERRO_PRINTS;
	if (operand[0] == '&' && command_opcode != 9)							/*relative operand can't be use with these commands*/
		goto ERRO_PRINTS;
	if ((operand[0]=='r') && ((command_opcode == 4 && operand_order == 1) || command_opcode == 9)) /*register can't be use with these commands*/
		goto ERRO_PRINTS;
	if (operand[0]=='#' && *endptr == '\0')			/*immediate operand,we will make a new memory_word to save it's value (unless endptr isnt null which means it detected another char in the operand after the number)*/
	{
		new_word = allocate_memory_word();						/*calling the function allocate_memory_word for allocation*/
		if (new_word == NULL) 
			return ERROR;
		memory_address = (*bottom_word)->memory_address + 1;				/*new word in a new memory address**/
		(*bottom_word)->next=new_word;							/*the last memory_word will point to the new one*/
		new_word->memory_address = memory_address;
		strcpy(new_word->type, "code");	
		strcpy(new_word->source, "operand");						/*writing the word of information in the source field of the word*/									
		number &= ADDRESS_MASK;								/*a mask to make sure the opcode is 21 bits*/
		new_word->binary_code=((number<<SHIFT_FUNCT) | ABSOLUTE_BIT);			/*shift left and marking the 'A' bit*/
		*bottom_word=new_word;								/*updating the bottom_word pointer to point to the new one*/
		words_of_information ++;
		return words_of_information;
	}
	if ((operand[0] >= 'A' && operand[0] <= 'Z') || operand[0] == '&')	/*operand is a label, we will make a new memory_word for it. in the second pass we will update the label's address in the memory_word*/
	{
		if (operand_order == 1)
		{
			if (operand[0] == '&')
				*opcode |= (1 << 17);				/*updating the opcode based on the operand order*/
			else
				*opcode |= (1 << 16);
			(*command_word)->binary_code = *opcode;
		}
		if (operand_order == 2)
		{
			if (operand[0] == '&')
				*opcode |= (1 << 12);
			else
				*opcode |= (1 << 11);
			(*command_word)->binary_code = *opcode;
		}
		new_word = allocate_memory_word();					/*calling the function allocate_memory_word for allocation*/
		if (new_word == NULL) 
			return ERROR;
		memory_address = (*bottom_word)->memory_address + 1;				/*new word in a new memory address**/
		(*bottom_word)->next = new_word;						/*the last memory_word will point to the new one*/
		new_word->memory_address = memory_address;
		strcpy(new_word->type, "code");					
		strcpy(new_word->source, operand);					/*writing the label name in the name section of the word*/
		*bottom_word = new_word;							/*updating the bottom_word pointer to point to the new one*/
		words_of_information ++;
		return words_of_information;
	}
	if (operand[0]=='r' && *endptr == '\0' && number >= REGISTER_MIN && number <= REGISTER_MAX)		/*first operand is a register and doesn't need a word of information (unless *endptr is null which means it detected another char after the number or unless the numbere is not a valid register number)*/
	{
		if (operand_order == 1)
		{
			*opcode |= (REGISTER_METHOD << SOURCE_METHOD);				/*updating the source addressing method*/
			*opcode |= (number << SOURCE_REGISTER);					/*updating the source register*/
			(*command_word)->binary_code = *opcode;
		}
		if (operand_order == 2)
		{
			*opcode |= (REGISTER_METHOD << DESTINATION_METHOD);			/*updating the destination addressing method*/
			*opcode |= (number << DESTINATION_REGISTER);				/*updating the destination register*/
			(*command_word)->binary_code = *opcode;
		}
		return words_of_information;
	}
	ERRO_PRINTS:
	fprintf(stderr, "Error: invalid operand in line %d  in .am file.\n", code_line);
	return ERROR;
}

memory_word* allocate_memory_word() 		/*a function for allocation memory_word. return the pointer to the new memory_word or NULL if allocation failed*/
{
	memory_word* new_word = (memory_word *)malloc(sizeof(memory_word));
	if (new_word == NULL) 
	{
		fprintf(stderr, "Error: Memory allocation failed.\n");
		return NULL;
	}
	return new_word;
}

void free_memory_stack(memory_word **top_word, memory_word **bottom_word)  	/*a function to free the memory allocation of the linked list*/
{
	memory_word *temp;
	while (*top_word != NULL) 
	{
        	temp = *top_word;
        	*top_word = (*top_word)->next;
        	free(temp);
   	}
	*top_word = NULL;
	*bottom_word = NULL;
}