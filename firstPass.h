/*****************************************
* Assembler First Pass Header            *
* @author Zvi Badash                     *
* ****************************************
*/

#ifndef FIRSTPASS_H
#define FIRSTPASS_H

/*Imports */
#include <stdio.h>

/* Definitions */
#define BITS_IN_OPCODE (4)

#define SOURCE_ADDRESSING_MODE_OFFSET ((unsigned ) 7)
#define DEST_ADDRESSING_MODE_OFFSET ((unsigned ) 3)
#define OPCODE_OFFSET ((unsigned ) 11)

#define MAX_OPERAND_LENGTH (32)
#define MEMORY_OFFSET (100)


/* Type Definitions */

/* Function Prototypes */
/**
 * This function treats a string directive
 * **/
int installStringFromLine(char *line);

/**
 * This function treats a data directive
 * **/
int installNumbersFromLine(char *line);

/**
 * This function treats a extern directive
 * **/
void installExternLabelFromLine(char *line);

/**
 * This function sets the addressing modes bits in word #w
 * **/
void setAddressingModeFirstWord(Word *w, AddressingMode addrMode, Boolean isSource);

/**
 * This function sets the ARE bits in word #w
 * **/
void setARE(Word *w, ARE opt);

/**
 * This function sets the opcode bits in word #w
 * **/
void setOpcodeFirstWord(Word *w, Instruction inst);

/**
 * This function create the first word of instruction
 * **/
Word makeFirstWord(Instruction inst, AddressingMode srcMode, AddressingMode destMode);

/**
 * This function makes an operand word of type immediate
 * **/
Word makeOperandWordImmediate(unsigned val);

/**
 * This function makes an operand word of type register (only one)
 * **/
Word makeOperandWordSingleRegister(Register reg, Boolean isSource);

/**
 * This function makes an operand word of type register (both registers are in one word)
 * **/
Word makeOperandWordDoubleRegisters(Register srcReg, Register destReg);

/**
 * This functions treats the first pass on file #fp
 * **/
void analyzeLineFirstPass(char *line);

/**
 * This functions treats a line in the first pass
 * **/
void firstPass(char *fileName, FILE *fp);

#endif
