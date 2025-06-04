#ifndef SECONDPASS_H
#define SECONDPASS_H

#include "symboltable.h"
#include "memorylist.h"

#define FALSE 0
#define TRUE 1
#define ADDRESS_MASK 0x1FFFFF  		/*21 bit mask*/
#define SHIFT_AMOUNT 3			/*the shift amount to go from 21 bits to 24 bits*/
#define EXTERNAL_BIT 0x1		/*marks an external label in the right bit */
#define RELOCATABLE_BIT 0x2		/*marks a relocatable label in the right bit */
#define ABSOLUTE_BIT 0x4		/*marks an absolute address in the right bit*/
#define EXTENSION_EXT ".ext"		/*external file extention*/
#define EXTENSION_ENT ".ent"		/*entry file extention*/
#define MAX_NAME 100

int secondpass(memory_word **top_word, symbol **top_symbol, char *file_name);

#endif