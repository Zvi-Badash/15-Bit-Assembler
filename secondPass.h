/*****************************************
* Assembler Second Pass Header           *
* @author Zvi Badash                     *
* ****************************************
*/

#ifndef SECONDPASS_H
#define SECONDPASS_H

/*Imports */
#include <stdio.h>

/* Definitions */
#define ARE_OFFSET ((unsigned ) 3)
#define SOURCE_REGISTER_NUM_OFFSET ((unsigned ) 6)
#define DEST_REGISTER_NUM_OFFSET ((unsigned ) 3)

#define BITS_IN_REGISTER_NUM (3)

/* Type Definitions */

/* Function Prototypes */
/**
 * This functions treats an entry directive
 * **/
void installEntryLabelFromLine(char *line);

/**
 * This functions treats the second pass on file #fp
 * **/
void secondPass(char *fileName, FILE *fp);

/**
 * This functions treats a line in the second pass
 * **/
void analyzeLineSecondPass(char *line);

/**
 * This functions make an operand word from a label
 * **/
Word makeOperandWordDirect(char *label);

/**
 * This functions install an operand word in the memory
 * **/
void installOperandWord(char *op, AddressingMode mode, Boolean isSource);

#endif
