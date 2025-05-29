#ifndef MEMORYLIST_H
#define MEMORYLIST_H
#define ERROR -1
#define DATA_MASK 0xFFFFFF   			/*24 bit mask*/
#define ADDRESS_MASK 0x1FFFFF   		/*21 bit mask*/
#define OPCODE_MASK 0x3F       			/*6 bits mask for the opcode*/
#define OPCODE_SHIFT 18       			/*shift for 6 bit opcode*/
#define REGISTER_METHOD 0x3   			/*mask for register addressing method*/
#define SOURCE_METHOD 16			/*shift for source method*/
#define DESTINATION_METHOD 11			/*shift for dest method*/
#define SOURCE_REGISTER 13			/*shift for source register*/
#define DESTINATION_REGISTER 8			/*shift for dest register*/
#define SHIFT_FUNCT 3				/*shift for funct*/
#define MAX_OPCODE 16				/*opcode < 16 */
#define ABSOLUTE_BIT 0x4
#define REGISTER_MIN 0				/*register minimun number*/
#define REGISTER_MAX 7				/*register maximum number*/
#define MIN_ONE_OPERAND_OPCODE 5		/*commad with one operand min number*/
#define MAX_ONE_OPERAND_OPCODE 13		/*command with one operand max number*/
#define MIN_TWO_OPERAND_OPCODE 0		/*commad with two operands min number*/
#define MAX_TWO_OPERAND_OPCODE 4		/*commad with two operands max number*/

#include "macro.h"

typedef struct memory_stack	/*creating a linked list to store the binary command based on memory location*/
	{
		int memory_address;			/*address*/
		char type[MAX_LINE];			/*stores one of the strings "code" or "data" according to the type of line*/
		char source[MAX_LINE];			/*stores the source (command/label/data/word of information)*/
		unsigned int binary_code;  		/*24-bit unsigned number*/
		struct memory_stack *next;	
	} memory_word;

int data_list(int current_address, char *current_line, int code_line, memory_word **top_word, memory_word **bottom_word);
int string_list(int current_address, char string[], int code_line, memory_word **top_word, memory_word **bottom_word);
int command_list(int current_address,int code_line, char command[], char first_operand[], char second_operand[], memory_word **top_word, memory_word **bottom_word);
void no_operands_command(char *first_part, int address, memory_word **top_word, memory_word **bottom_word);
int registerupdate(char operand[], unsigned int *opcode,int command_opcode, int operand_order, int code_line, memory_word **command_word, memory_word **bottom_word);
memory_word* allocate_memory_word();
void free_memory_stack(memory_word **top_word, memory_word **bottom_word);

#endif