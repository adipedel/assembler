#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binarytohexa.h"

/*this function recieves a file name and a pointer to the memory linked list. the function goes through the linked list and counts how many code words there are and how many data words there are, and writes the numbers into the file. it goes through the linked list again and writes the word's memory address and the word's opcode in hexa format.*/
void binary_to_hexa(const char *file, memory_word *top_word)
{
	FILE *hexafile;
	memory_word *new_word;
	int code_num = 0, data_num = 0;
	new_word = top_word;

	while (new_word != NULL) 							/*going through the memory linked list*/
	{
		if (strcmp(new_word->type, "code") == 0)
			code_num++;							/*counting the code type words*/
		if (strcmp(new_word->type, "data") == 0)
			data_num++;							/*counting the data type words*/
		new_word = new_word->next;						/*moving to the next word in the memory linked list*/
	}
	hexafile = fopen(file, "w");
	fprintf(hexafile, "      %d %d\n", code_num, data_num);				/*printing the number of code lines and number of data lines*/
	new_word = top_word;
	while (new_word != NULL) 							/*going through the memory linked list*/
	{
		fprintf(hexafile, "%07d %06X\n", new_word->memory_address ,new_word->binary_code); 		/*writing to the op file. the memory address is written in decimal (with leading zereos) and the binary opcode in hexa format*/
		new_word = new_word->next;						/*moving to the next word in the memory linked list*/
	}
}