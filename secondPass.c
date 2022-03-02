/*****************************************
* Second Pass Operations                 *
* @author Zvi Badash                     *
* ****************************************
*/

/* Imports */
#include "utils.h"
#include "firstPass.h"
#include "dataTypes.h"
#include "mainHeader.h"
#include "secondPass.h"
#include "externalVariables.h"
#include "fileHandling.h"

/* Functions */
void installEntryLabelFromLine(char *line) {
    char *entryLabel; /* Will hold the name of the label */
    LabelPointer retVal;

    entryLabel =  strip(strstr(line, ".entry")); /* Go to the directive */
    entryLabel += strlen(".entry"); /* skip it */
    entryLabel =  strip(entryLabel); /* skip spaces */
    entryLabel =  removeNewLine(entryLabel); /* remove the "\r\n" or '\n' */

    if ((retVal = searchByName(&symbolTable, entryLabel)) == NULL) { /* If the label dose'nt exist */
        errorCode = ENTRY_LABEL_DOSENT_EXIST;
        return;
    }
    else { /* Set it as entry */
        retVal->isEntry = TRUE;
        shouldOutputEntry = TRUE;
    }
}

Word makeOperandWordImmediate(unsigned val) {
    /* insert the value */
    Word operandWord = (val << ARE_OFFSET);

    /* insert ARE */
    setARE(&operandWord, ABSOLUTE);

    return operandWord;
}

Word makeOperandWordDirect(char *label) {
    Word operandWord = 0U; /* An empty word */
    LabelPointer node;

    if ((node = searchByName(&symbolTable, label)) != NULL) { /* if the label exist */
        /* Set its value in the word */
        operandWord = (node->value << ARE_OFFSET);
        /* Set ARE */
        if (node->feature != EXTERN_FEATURE) /* If the feature is data/code */
            setARE(&operandWord, RELOCATABLE);
        else { /* If the feature is external */
            setARE(&operandWord, EXTERNAL);
            /* Add the extern event to the ExternEvent table  */
            addExternEvent(&externEventTable, label, ic + MEMORY_OFFSET);
        }
    }

    return operandWord;
}

Word makeOperandWordSingleRegister(Register reg, Boolean isSource) {
    int i;
    Word operandWord = 0U;
    /* determine the offset */
    unsigned offset = isSource ? SOURCE_REGISTER_NUM_OFFSET : DEST_REGISTER_NUM_OFFSET;

    /* insert the register value */
    for (i = 0; i < BITS_IN_REGISTER_NUM; i++)
        setState(&operandWord, (i + offset), getState(reg, i));

    /* Set ARE */
    setARE(&operandWord, ABSOLUTE);

    return operandWord;
}

Word makeOperandWordDoubleRegisters(Register srcReg, Register destReg) {
    int i;
    Word operandWord = 0U;

    /* insert the register value */
    for (i = 0; i < BITS_IN_REGISTER_NUM; i++)
        setState(&operandWord, (i + SOURCE_REGISTER_NUM_OFFSET), getState(srcReg, i));

    /* insert the register value */
    for (i = 0; i < BITS_IN_REGISTER_NUM; i++)
        setState(&operandWord, (i + DEST_REGISTER_NUM_OFFSET), getState(destReg, i));

    /* Set ARE */
    setARE(&operandWord, ABSOLUTE);

    return operandWord;
}

void installOperandWord(char *op, AddressingMode mode, Boolean isSource) {
    if (mode == IMMEDIATE) /* Addressing mode 0 */
        installWordInCode(makeOperandWordImmediate(getNumber(op)));

    else if (mode == DIRECT) /* Addressing mode 1 */
        installWordInCode(makeOperandWordDirect(op));

    else if (mode == REGISTER_DIRECT || mode == REGISTER_INDIRECT) /* Addressing modes 2/3 */
        installWordInCode(makeOperandWordSingleRegister(getRegister(op), isSource));

}

void analyzeLineSecondPass(char *line) {
    AddressingMode srcMode, destMode;
    Directive dir = getDirective(line);
    Boolean isSrcRegister, isDestRegister;
    char *firstOperand = NULL, *secondOperand = NULL;

    /* If the line is ignorable */
    if (isIgnorable(line))
        return;

    /* If the line is a non-entry directive */
    if (dir == DATA_DIR || dir == STRING_DIR || dir == EXTERN_DIR)
        return;

    /* If the line is an entry directive */
    if (dir == ENTRY_DIR) {
        installEntryLabelFromLine(line);
        return;
    }

    /* Else, the line is an instruction */
    /* Get the operands */
    firstOperand  = getFirstOperand(line);
    secondOperand = getSecondOperand(line);

    /* Get the addressing modes for the operands */
    srcMode  = getAddressingModeSecondPass(firstOperand);
    destMode = getAddressingModeSecondPass(secondOperand);

    /* If there is only one operand its the dest one */
    if (destMode == UNKNOWN_ADDRESSING_MODE && srcMode != UNKNOWN_ADDRESSING_MODE) {
        destMode = srcMode;
        srcMode = UNKNOWN_ADDRESSING_MODE;

        secondOperand = firstOperand;
        firstOperand = NULL;
    }

    /* Will hold whether if any operand is a register operand (direct or indirect) */
    isSrcRegister  = BOOLEANIZE(srcMode  == REGISTER_INDIRECT || srcMode  == REGISTER_DIRECT);
    isDestRegister = BOOLEANIZE(destMode == REGISTER_INDIRECT || destMode == REGISTER_DIRECT);

    /* The first word of the was already installed in the first pass, skip it */
    ++ic;

    if (isDestRegister && isSrcRegister) { /* If both operands are registers */
        installWordInCode(makeOperandWordDoubleRegisters(getRegister(firstOperand), getRegister(secondOperand)));
    } else {
        /* If there's a source operand */
        if (srcMode != UNKNOWN_ADDRESSING_MODE)
            installOperandWord(firstOperand, srcMode, TRUE);


        /* If there's a destination operand */
        if (destMode != UNKNOWN_ADDRESSING_MODE)
            installOperandWord(secondOperand, destMode, FALSE);
    }
}

void secondPass(char *fileName, FILE *fp) {
    int lineNum = 1;
    Boolean hadError = FALSE;
    char line[MAX_LINE_LENGTH];

    printf("\n******************************************\n");
    printf("STARTED SECOND PASS ON FILE %s.\n", fileName);
    printf("******************************************\n");
    /* Set ic to 0 */
    ic = 0;

    /* Read fp line by line */
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        errorCode = NO_ERROR;
        analyzeLineSecondPass(line);

        if (errorCode != NO_ERROR) { /* If an error was encountered */
            alertLineError(lineNum, errorCode);
            hadError = TRUE;
        }
        ++lineNum;
    }

    /* If an error was encountered in the first pass, there's no need to continue to the second pass */
    if (hadError) {
        printf("\n**********************************************************************\n");
        printf("ERRORS WERE ENCOUNTERED DURING THE SECOND PASS, OUTPUT FILES "
               "WON'T BE CREATED\n");
        printf("**********************************************************************\n");
        return;
    }

    printf("\n******************************\n");
    printf("SECOND PASS ENDED SUCCESSFULLY \n" );
    printf("******************************\n"  );

    /* Create output files */
    createOutputFiles(fileName);
}