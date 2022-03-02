/*****************************************
* Assembler Data Types Header            *
* @author Zvi Badash                     *
* ****************************************
*/

#ifndef DATATYPES_H
#define DATATYPES_H

/*Imports */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definitions */
#define BITS_IN_WORD ((unsigned ) 15 )
#define MAX_DIRECTIVE_LENGTH 16

/* This macro will enforce a Boolean (/ Bit) type on x (non-zero value yield 1) (0 yield 0) */
#define BOOLEANIZE(x) (!(!(x)))

/* Type Definitions */
/*
 * This data type defines a word (a 15 bits long string of bits).
 * I've chosen to represent a Word with an unsigned int because bitwise operations are common in my code
 * and that data type makes them easier to understand and read.
 * The potential waste is quite big (=> 4096 * ((sizeof(unsigned) * 8) - 15) in my machine that amount evaluates to
 * 69,632 bits (=> 8,704 bytes)) but is bounded as most machines don't exceed a constant amount of bytes
 * (4 in my machine's case)
 */
typedef unsigned Word;
/*** Enumerations ***/
/* Defines a boolean type (FALSE / TRUE) */
typedef enum {FALSE, TRUE} Boolean;

/* Defines a Bit type (enum ON / OFF) */
typedef enum {OFF, ON} Bit;

/* All possible commands, ordered by opcode */
typedef enum {
    MOV_INST = 0, CMP_INST, ADD_INST, SUB_INST, LEA_INST, CLR_INST, NOT_INST, INC_INST, DEC_INST, JMP_INST, BNE_INST,
    RED_INST, PRN_INST, JSR_INST, RTS_INST, STOP_INST, UNKNOWN_INST = -1
} Instruction;

/* All possible directives */
typedef enum {DATA_DIR = 0, STRING_DIR, ENTRY_DIR, EXTERN_DIR, UNKNOWN_DIRECTIVE = -1} Directive;

/* All possible addressing modes ordered by their number */
typedef enum {
    IMMEDIATE = 0, DIRECT, REGISTER_INDIRECT, REGISTER_DIRECT, UNKNOWN_ADDRESSING_MODE = -1
} AddressingMode;

/* Label features (Code / Data) i.e. Is the label pointing to a data segment pr a code segment */
typedef enum {DATA_FEATURE, CODE_FEATURE, EXTERN_FEATURE} LabelFeature;

/* The ARE field can contain A, R or E (Absolute / Relocatable / External) */
typedef enum {ABSOLUTE = 2, RELOCATABLE = 1, EXTERNAL = 0} ARE;

/* This enum contains all registers (r0 / r1 / r2 / r3 / r4 / r5 / r6 / r7)  */
typedef enum {R0, R1, R2, R3, R4, R5, R6, R7} Register;

/* This enum contains all file types (Assembly / Object / Entry / Extern) */
typedef enum {ASM, OBJ, ENT, EXT, UNKNOWN_FILE_TYPE} FileType;

/* This enum contains all error codes */
typedef enum {
              LABEL_NAME_TOO_LONG, LABEL_NAME_INVALID, LABEL_NAME_RESERVED, LABEL_NAME_ALREADY_EXIST,
              INVALID_CMD_NAME, INVALID_DIR_NAME,
              INVALID_OPERANDS_DATA_DIR, INVALID_OPERANDS_STRING_DIR,
              INVALID_OPERAND_NUMBER_CMD, INVALID_ADDR_MODES,
              ENTRY_LABEL_DOSENT_EXIST, EXTERN_OPERAND_INVALID, EXTERN_OPERAND_ALREADY_EXIST,
              INVALID_COMMAS,
              STRING_OPERAND_INVALID, DATA_OPERAND_INVALID,
              NO_ERROR = -1
} Error;

/*** Dynamic Data Storage ***/
/* Definition of a label */
typedef struct LabelNode {
    unsigned int value; /* The address pointed to by the label.    Example: 133   */
    char *labelName; /* The name of the label.                     Example : LIST */
    LabelFeature feature; /* The feature of the label              (CODE / DATA)  */
    Boolean isEntry; /* If the label is to be used in other files. (FALSE / TRUE) */
    struct LabelNode *next; /* A pointer to the next label in the list.           */
} Label;

/* A list is a pointer to the head of the label list */
typedef Label *LabelList;

/* A pointer to a label*/
typedef Label *LabelPointer;

/* Defining An Extern address */
typedef struct ExternNode {
    char *labelName;         /* The name of the label being stored */
    unsigned address;        /* Address of said label */
    struct ExternNode *next; /* A pointer to the next ExternNode */
} externEvent;

/* A list is a pointer to the head of the label list */
typedef externEvent *ExternEventList;

/* A pointer to a label*/
typedef externEvent *ExternEventPointer;

/* Function Prototypes */
/*** Word Bit Fiddling Operation ***/
/** This function will return the state of bit #pos inside Word #w
 * @param w The word. Example: 001010000010100
 * @param pos The position to get the bit state from. Example: pos = 12 (=> return ON)
 * @return ON if bit is 1, else OFF.
**/
Bit getState (Word w, unsigned int pos);

/** This function will set the state of bit #pos inside Word #w to #state
 * @param w The word. Example: 001010000010100
 * @param pos The position to set the bit state to. Example: pos = 12
 * @param state The state to set in #w in position #pos. Example: state = OFF (=> change #w to 000010000010100)
**/
void setState (Word *w, unsigned int pos, Bit state);


/*** Dynamic Data Storage ***/
/**
 * This function will insert a node with given data at the beginning of #lst
 * @param lst The list to insert the data in
 * @param valueP The value to put in the new node
 * @param labelNameP The name to put in the new node
 * @param featureP The feature to put in the new node
 * @param isExternP The isExtern Boolean type to put in the new node
 * @param isEntryP The isEntry Boolean type to put in the new node
 * **/
void insertLabel(LabelList *lst, unsigned int valueP, char *labelNameP, LabelFeature featureP, Boolean isEntryP);

/**
 * This function will insert a node with given data at the beginning of #lst
 * @param lst The list to insert the data in
 * @param labelNameP The name of the extern label
 * @param addressP The address of the external event
 * **/
void addExternEvent(ExternEventList *lst, char *labelNameP, unsigned addressP);

/***
 * This function will search a node in #lst by given name #labelNameP
 * @param lst The list to look in
 * @param labelNameP The name to look for
 * @return The node containing #labelNameP, And NULL if not found
 * ***/
LabelPointer searchByName(LabelList *lst, char *labelNameP);

void freeSymbolTable(LabelList *lst);

void freeExternEventsTable(ExternEventList *lst);

/* Private Functions (Used purely for debugging) */
void __prnWord (Word w);
void __prnList(LabelList *lst);
void __prnNode(LabelPointer tNode);
void __prnExternList(ExternEventList *lst);

#endif
