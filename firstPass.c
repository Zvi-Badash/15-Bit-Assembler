/*****************************************
* First Pass Operations                  *
* @author Zvi Badash                     *
* ****************************************
*/

/* Imports */
#include "dataTypes.h"
#include "firstPass.h"
#include "secondPass.h"
#include "externalVariables.h"
#include "utils.h"
#include "mainHeader.h"

/* Functions */
int installStringFromLine(char *line) {
    char *args; /* Will hold the string */
    char *noQuotes; /* Will hold the string without the quotes */
    int count = 0;

    args = strip(strstr(line, ".string")); /* go to the directive */
    args += strlen(".string"); /* skip it */
    args = strip(args); /* skip spaces */

    /* If no operand was given */
    if (isEmpty(args)) {
        errorCode = INVALID_OPERANDS_STRING_DIR;
        return -1;
    }

    if (isString(args)) { /* if the argument is a valid string */
        noQuotes = getString(args);
        while (*noQuotes) {
            installWordInData((unsigned int) *noQuotes); /* install each character */
            ++noQuotes;
            ++count;
        }
        installWordInData((unsigned int ) '\0'); /* install the ending '\0' */
        ++count;
    } else { /* if the argument is not valid */
        errorCode = STRING_OPERAND_INVALID;
        return -1;
    }
    return count;
}

int installNumbersFromLine(char *line) {
    char *args; /* Will hold the arguments */
    int count = 0;

    char *tempNumber, delim[] = ", \t"; /* for strtok */
    args = strip(strstr(line, ".data")); /* go to the directive */
    args += strlen(".data"); /* skip it */
    args = strip(args); /* skip spaces */

    /* Go through the arguments */
    tempNumber = strtok(args, delim);
    while (BOOLEANIZE(tempNumber)) { /* While strtok is not NULL */
        if (isNumber(tempNumber)) { /* If its a valid number */
            installWordInData(getNumber(tempNumber)); /* install it */
            ++count;
        }
        else { /* if the argument is invalid */
            errorCode = DATA_OPERAND_INVALID;
            return -1;
        }
        tempNumber = strtok(NULL, delim); /* get the next argument */
    }
    return count;
}

void installExternLabelFromLine(char *line) {
    char *labelArg; /* Will hold the name of the label */
    LabelPointer retVal;

    labelArg =  strip(strstr(line, ".extern")); /* go to the directive */
    labelArg += strlen(".extern"); /* skip it */
    labelArg =  strip(labelArg); /* skip spaces */

    /* remove the '\r\n' or the '\n' in the end */
    removeNewLine(labelArg);

    /* if the operand is an illegal label */
    if (!isLegalLabelNoColon(labelArg)) {
        errorCode = EXTERN_OPERAND_INVALID;
        return;
    }

    if ((retVal = searchByName(&symbolTable, labelArg)) != NULL) { /* If the label already exists */
        if (retVal->feature != EXTERN_FEATURE) { /* if the label exists non-externally */
            errorCode = EXTERN_OPERAND_ALREADY_EXIST;
        }
        else /* if it was already defined as extern */
            return;
    } else { /* insert it to the symbol table */
        insertLabel(&symbolTable, (unsigned int) 0, labelArg, EXTERN_FEATURE, FALSE);
    }

    shouldOutputExtern = TRUE;
}

void setARE(Word *w, ARE opt) {
    /* Set the ARE field to 0 */
    setState(w, ABSOLUTE, OFF);
    setState(w, RELOCATABLE, OFF);
    setState(w, EXTERNAL, OFF);

    switch (opt) { /* Go through ARE cases */
        case ABSOLUTE:
            setState(w, ABSOLUTE, ON);
            break;

        case RELOCATABLE:
            setState(w, RELOCATABLE, ON);
            break;

        case EXTERNAL:
            setState(w, EXTERNAL, ON);
            break;

        default:
            break;
    }
}

void setAddressingModeFirstWord(Word *w, AddressingMode addrMode, Boolean isSource) {
    /* determine the offset of the addressing mode */
    unsigned offset = isSource ? SOURCE_ADDRESSING_MODE_OFFSET : DEST_ADDRESSING_MODE_OFFSET;

    switch (addrMode) { /* go through the addressing modes */
        case IMMEDIATE:
            setState(w, IMMEDIATE + offset, ON);
            break;

        case DIRECT:
            setState(w, DIRECT + offset, ON);
            break;

        case REGISTER_INDIRECT:
            setState(w, REGISTER_INDIRECT + offset, ON);
            break;

        case REGISTER_DIRECT:
            setState(w, REGISTER_DIRECT + offset, ON);
            break;

        case UNKNOWN_ADDRESSING_MODE:
        default:
            break;
    }
}

void setOpcodeFirstWord(Word *w, Instruction inst) {
    int i;

    /* Set the opcode in the word */
    for (i = 0; i < BITS_IN_OPCODE; i++)
        setState(w, (i + OPCODE_OFFSET), getState(inst, i));
}

Word makeFirstWord(Instruction inst, AddressingMode srcMode, AddressingMode destMode) {
    Word firstWord = 0U; /* empty word */

    /* set ARE to absolute */
    setARE(&firstWord, ABSOLUTE);

    /* set the opcode */
    setOpcodeFirstWord(&firstWord, inst);

    /* Set the addressing modes */
    if (srcMode != UNKNOWN_ADDRESSING_MODE)
        setAddressingModeFirstWord(&firstWord, srcMode, TRUE);

    if (destMode != UNKNOWN_ADDRESSING_MODE)
        setAddressingModeFirstWord(&firstWord, destMode, FALSE);

    return firstWord;
}

Boolean hasCorrectAddressingModes(Instruction inst, AddressingMode srcMode, AddressingMode destMode) {
    /* Will hold if the src, dest operands exist */
    Boolean hasSource = srcMode  != UNKNOWN_ADDRESSING_MODE;
    Boolean hasDest   = destMode != UNKNOWN_ADDRESSING_MODE;

    switch (inst) { /* go through the instructions */
        case CMP_INST:
            /* SRC: 0, 1, 2, 3 DEST: 0, 1, 2, 3 */
            return hasDest && hasSource;
        case MOV_INST:
        case ADD_INST:
        case SUB_INST:
            /* SRC: 0, 1, 2, 3 DEST: 1, 2, 3 */
            return hasDest && hasSource && destMode != IMMEDIATE;
        case LEA_INST:
            /* SRC: 1 DEST: 1, 2, 3 */
            return hasDest && hasSource && srcMode == DIRECT && destMode != IMMEDIATE;
        case CLR_INST:
        case NOT_INST:
        case INC_INST:
        case DEC_INST:
        case RED_INST:
            /* SRC: None DEST: 1, 2, 3 */
            return hasDest && !hasSource && destMode != IMMEDIATE;
        case JMP_INST:
        case BNE_INST:
        case JSR_INST:
            /* SRC: None DEST: 1, 2 */
            return hasDest && !hasSource && (destMode == DIRECT || destMode == REGISTER_INDIRECT);
        case PRN_INST:
            /* SRC: None DEST: 0, 1, 2, 3 */
            return hasDest && !hasSource;
        case RTS_INST:
        case STOP_INST:
            /* SRC: None DEST: None */
            return !hasDest && !hasSource;
        case UNKNOWN_INST:
        default:
            return FALSE;
    }
}

void analyzeLineFirstPass(char *line) {
    int L = 0; /* The length of the instruction */
    int numberOfActualOps = 0; /* number of operands that exist */
    Directive dir; /* the directive */
    Instruction inst; /* the instruction */
    Word instructionWord; /* the first word */
    Boolean hasLabel = FALSE; /* hold if a label exists */
    AddressingMode srcMode, destMode; /* addressing modes */
    char *label, *firstOperand = NULL, *secondOperand = NULL; /* label, operands */

    /* If the line is ignorable */
    if (isIgnorable(line))
        return;

    /* If the line has two consecutive commas */
    if (hasTwoCommas(line)) {
        errorCode = INVALID_COMMAS;
        return;
    }

    /* If the line has a label definition, turn on the #hasLabel flag */
    if ((label = getLabel(line)) != NULL)
        hasLabel = TRUE;

    /* If the label illegal */
    if (errorCode != NO_ERROR)
        return;

    /* if the label needs to be installed in the symbol table */
    dir = getDirective(line);
    if ((dir == DATA_DIR || dir == STRING_DIR) && hasLabel) {
        if (searchByName(&symbolTable, label) == NULL)
            insertLabel(&symbolTable, dc, label, DATA_FEATURE, FALSE);
        else
            errorCode = LABEL_NAME_ALREADY_EXIST;
    }

    /* Handle different directives, entry will be taken care of in the second pass */
    if (dir == DATA_DIR) { /* If a .data directive was found */
        if (installNumbersFromLine(line) == 0)
            errorCode = INVALID_OPERANDS_DATA_DIR;
        return;
    }
    else if (dir == STRING_DIR) { /* If a .string directive was found */
        installStringFromLine(line);
        return;
    }
    else if (dir == EXTERN_DIR) { /* If an .extern directive was found */
        installExternLabelFromLine(line);
        return;
    }
    else if (dir == ENTRY_DIR)  /* If a .entry directive was found */
        return;

    else if (dir == UNKNOWN_DIRECTIVE && startWithDot(line)) { /* If a non-existing directive appears */
        errorCode = INVALID_DIR_NAME;
        return;
    }


    /* If the line dose'nt have a directive, it's an instruction line */
    /* Add the label as code */
    if (hasLabel)
        insertLabel(&symbolTable, ic + MEMORY_OFFSET, label, CODE_FEATURE, FALSE);

    if ((inst = getInstruction(line)) == UNKNOWN_INST) {
        errorCode = INVALID_CMD_NAME;
        return;
    }

    /* Get the operands */
    firstOperand  = getFirstOperand(line);
    secondOperand = getSecondOperand(line);

    /* Get the number of actual operands */
    if (firstOperand)
        numberOfActualOps++;
    if (secondOperand)
        numberOfActualOps++;

    /* if the umber of actual operands does not match, alert */
    if (numberOfActualOps != getNumberOfOperands(inst)) {
        errorCode = INVALID_OPERAND_NUMBER_CMD;
        return;
    }

    /* Get the addressing modes for the operands */
    srcMode  = getAddressingModeFirstPass(firstOperand);
    destMode = getAddressingModeFirstPass(secondOperand);

    /* If there is only one operand its the dest one */
    if (destMode == UNKNOWN_ADDRESSING_MODE && srcMode != UNKNOWN_ADDRESSING_MODE) {
        destMode = srcMode;
        srcMode = UNKNOWN_ADDRESSING_MODE;
    }

    /* If the addressing modes dose'nt match */
    if (!hasCorrectAddressingModes(inst, srcMode, destMode)) {
        errorCode = INVALID_ADDR_MODES;
        return;
    }

    /* Get the width of the instruction */
    L = getOperandInstructionWidth(srcMode, destMode);

    /* Install the first word */
    instructionWord = makeFirstWord(inst, srcMode, destMode);
    installWordInCode(instructionWord);

    /* Increment IC by the count of the words needed by the operands */
    ic += L;
}

void firstPass(char *fileName, FILE *fp) {
    int lineNum = 1;
    LabelPointer label;
    Boolean hadError = FALSE;
    char line[MAX_LINE_LENGTH];

    printf("\n******************************************\n");
    printf("STARTED FIRST PASS ON FILE %s.\n", fileName);
    printf("******************************************\n");
    /* Set ic, dc to 0 */
    ic = 0, dc = 0;

    /* Read fp line by line */
    while (fgets(line, MAX_LINE_LENGTH, fp)) {
        errorCode = NO_ERROR;
        analyzeLineFirstPass(line);

        if (errorCode != NO_ERROR) { /* If an error was encountered */
            alertLineError(lineNum, errorCode);
            hadError = TRUE;
        }
        ++lineNum;
    }

    /* If an error was encountered in the first pass, there's no need to continue to the second pass */
     if (hadError) {
        printf("\n**********************************************************************\n");
        printf("ERRORS WERE ENCOUNTERED DURING THE FIRST PASS, SECOND PASS "
               "WON'T BEGIN\n");
        printf("**********************************************************************\n");
        return;
    }

    /* Add ic + 100 to all data labels */
    for (label = symbolTable; label; label = label->next)
        if (label->feature == DATA_FEATURE)
            label->value += (ic + MEMORY_OFFSET);

    printf("\n******************************\n");
    printf("FIRST PASS ENDED SUCCESSFULLY \n"  );
    printf("******************************\n"  );

    /* Begin the second pass */
    rewind(fp); /* Return the file cursor to position 0 */
    secondPass(fileName, fp);
}
