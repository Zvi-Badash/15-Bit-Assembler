/*****************************************
* Assembler Utilities Header             *
* @author Zvi Badash                     *
* ****************************************
*/

#ifndef UTILS_H
#define UTILS_H

/*Imports */
#include "mainHeader.h"
#include "dataTypes.h"

/* Definitions */
#define MAX_LABEL_SIZE_WITH_COLON 32
#define MAX_OCT_SIZE 16

/* Type Definitions */

/* Function Prototypes */

/** Syntax Analysis & Input Detection **/
/**
 * This function strips the line from leading spaces
 * **/
char *strip (char *line);

/**
 * This functions returns whether if a line is empty
 * **/
Boolean isEmpty (char *line);

/**
 * This functions returns whether if a line ignorable (comment or empty)
 * **/
Boolean isIgnorable (char *line);
/**
 * This functions returns whether if a line has two consecutive commas
 * **/
Boolean hasTwoCommas (char *line);

/**
 * This functions returns whether if a sequence of characters is a legal number
 * **/
Boolean isNumber (char *sequence);

/**
 * This functions returns whether if a sequence of characters is a legal string
 * **/
Boolean isString (char *sequence);

/**
 * This functions removes the newline character at the end of the line
 * **/
char *removeNewLine(char *sequence);

/**
 * This functions returns whether if a sequence of characters is a legal label
 * **/
Boolean isLegalLabel(char *sequence);

/**
 * This functions returns whether if a sequence of characters is a directive
 * **/
Boolean isDirectiveName(char *sequence);

/**
 * This functions returns whether if a label name is present in the symbol table
 * **/
Boolean isDefinedLabel (char *sequence);

/**
 * This functions returns whether if a sequence of characters is a legal register
 * **/
Boolean isRegisterName (char *sequence);

/**
 * This functions returns whether if a sequence of characters is an instruction name
 * **/
Boolean isInstructionName(char *sequence);

/**
 * This functions returns whether if a sequence of characters is a legal label (with out the colon)
 * **/
Boolean isLegalLabelNoColon(char *sequence);

/**
 * This functions returns the addressing mode of an operand in the first pass
 * **/
AddressingMode getAddressingModeFirstPass(char *operand);

/**
 * This functions returns the addressing mode of an operand in the second pass
 * **/
AddressingMode getAddressingModeSecondPass(char *operand);

/**
 * This functions returns whether if a line starts with a '.' (or a label before it)
 * **/
Boolean startWithDot(char *line);

/**
 * This functions returns the directive inside a line (or UNKNOWN if missing)
 * **/
Directive getDirective (char *line);

/**
 * This functions returns the length (in characters) of an instruction
 * **/
int getInstructionLength(Instruction code);

/**
 * This functions returns a number from a sequence
 * **/
int getNumber (char *sequence);

/**
 * This functions returns a string from a sequence
 * **/
char *getString(char *sequence);

/**
 * This functions returns a label from a line (or NULL if missing)
 * **/
char *getLabel (char *line);

/**
 * This functions returns the instruction inside a line (or UNKNOWN if missing)
 * **/
Instruction getInstruction(char *line);

/**
 * This functions returns the first operand of the instruction from a line (or NULL if missing)
 * **/
char *getFirstOperand(char *line);

/**
 * This functions returns the second operand of the instruction from a line (or NULL if missing)
 * **/
char *getSecondOperand(char *line);

/**
 * This functions returns a register number from a sequence ("*r6" => 6)
 * **/
Register getRegister(char *seq);

/**
 * This functions returns the number if words needed for an instruction besides the first one)
 * **/
int getOperandInstructionWidth(AddressingMode src, AddressingMode dest);

/** Memory Operations And Encoding Operations **/
/**
 * This functions installs a word in the data image
 * **/
void installWordInData(Word w);

/**
 * This functions installs a word in the machine image
 * **/
void installWordInCode(Word w);

/** Error Handling Methods **/
void alertLineError(int lineNum, Error err);

/** Misc. Methods **/
/**
 * This functions resets all global variables to defaults
 * **/
void initializeGlobalVariables();

/**
 * This functions returns the number of operands an instruction should get
 * **/
int getNumberOfOperands(Instruction inst);

/**
 * This functions returns the octal representation of a word
 * **/
char *toOctal(unsigned word);
#endif
