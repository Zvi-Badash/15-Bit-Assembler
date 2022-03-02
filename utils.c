/*****************************************
* Assembler Utilities                    *
* @author Zvi Badash                     *
* ****************************************
*/

/* Imports */
#include "utils.h"
#include "mainHeader.h"
#include "firstPass.h"
#include "dataTypes.h"
#include "externalVariables.h"

/* Functions */
/** Syntax Analysis And Input Detection **/
char *strip(char *line) {
    /* If the line is completely empty */
    if (line == NULL)
        return NULL;

    /* Remove any whitespace from the beginning */
    while (isspace(*line)) line++;
    return line;
}

char *removeNewLine(char *sequence) {
    int newLineLoc = 0; /* The location of the new line character */

    /* Find the index of the new line */
    for (; !isEmpty(sequence) && sequence[newLineLoc] != '\n' && sequence[newLineLoc] != '\r'; ++newLineLoc)
        ;

    /* remove it by ending the string  */
    sequence[newLineLoc] = '\0';
    return sequence;
}

char *removeColon(char *sequence) {
    int colonLoc = 0; /* The location of the colon character */
    char *noColon = (char *) calloc(MAX_LABEL_SIZE_WITH_COLON * sizeof(char), 1);

    /* Copy the label to the noColon variable */
    strcpy(noColon, sequence);

    /* Find the index of the colon */
    for (; !isEmpty(noColon) && noColon[colonLoc] != ':'; ++colonLoc)
        ;

    /* remove it by ending the string  */
    noColon[colonLoc] = '\0';
    return noColon;
}

Boolean isIgnorable(char *line) {
    /* A line is ignorable if it is empty or if it's starts with ';' */
    return isEmpty(line) || *strip(line) == ';';
}

Boolean isEmpty(char *line) {
    /* If the line is NULL, it's empty */
    if (line == NULL)
        return TRUE;

    /* If the line has no characters, it's empty */
    if (*line == '\0')
        return TRUE;

    /* Line is empty if it has only white spaces */
    line = strip(line);
    return line == NULL || *line == '\0' || *line == '\n';
}

Boolean isRegisterName(char *sequence) {
    /* If the sequence is empty, it can't be a register name */
    if (isEmpty(sequence))
        return FALSE;

    /* If the sequence does not start with 'r', it can't be a register name */
    if (*sequence != 'r')
        return FALSE;

    ++sequence; /* Go to next character */
    /* If the next character is not between '0' and '7', it can't be a register name */
    if (*sequence < '0' || *sequence > '7')
        return FALSE;

    ++sequence; /* Go to next character */
    /* If the sequence is not finished after the second character, it can't be a register name */
    return isEmpty(sequence);
}

Boolean isDirectiveName(char *sequence) {
    /* return if the sequence is either data/string/extern/entry */
    return strcmp(sequence, "data")    == 0 ||
           strcmp(sequence, "string")  == 0 ||
           strcmp(sequence, "extern")  == 0 ||
           strcmp(sequence, "entry")   == 0;
}

Boolean isInstructionName(char *sequence) {
    /* return if the sequence is an instruction */
    return strcmp(sequence, "mov")  == 0 ||
           strcmp(sequence, "cmp")  == 0 ||
           strcmp(sequence, "add")  == 0 ||
           strcmp(sequence, "sub")  == 0 ||
           strcmp(sequence, "lea")  == 0 ||
           strcmp(sequence, "clr")  == 0 ||
           strcmp(sequence, "not")  == 0 ||
           strcmp(sequence, "ind")  == 0 ||
           strcmp(sequence, "dec")  == 0 ||
           strcmp(sequence, "jmp")  == 0 ||
           strcmp(sequence, "bne")  == 0 ||
           strcmp(sequence, "red")  == 0 ||
           strcmp(sequence, "prn")  == 0 ||
           strcmp(sequence, "jsr")  == 0 ||
           strcmp(sequence, "rts")  == 0 ||
           strcmp(sequence, "stop") == 0;

}

Boolean isNumber(char *sequence) {
    /* If the sequence is empty, it can't be a register name */
    if (isEmpty(sequence))
        return FALSE;

    /* If the first character is a sign symbol, skip it */
    if (*sequence == '+' || *sequence == '-')
        ++sequence;

    /* Go to end of the digit sequence (if exists) */
    while (!isEmpty(sequence) && isdigit(*sequence))
        ++sequence;

    /* If there are foreign characters after the digit sequence, it is not a number */
    return isEmpty(sequence);
}

Boolean isString(char *sequence) {
    if (sequence == NULL || isEmpty(sequence))
        return FALSE;
    /* If the sequence does not begin with '"', it is not a string */
    if (*sequence != '\"')
        return FALSE;
    /* If it does, skip it */
    ++sequence;

    /* Go to the next '"' if exists */
    while (!isEmpty(sequence) && *sequence != '\"')
        ++sequence;

    /* if the sequence doesn't end with '"', it is not a string */
    if (*sequence != '\"')
        return FALSE;

    /* Skip the '"' character */
    ++sequence;
    /* If the sequence doesn't end after the '"', it is not a string */
    return isEmpty(sequence);
}

Boolean isLegalLabel(char *sequence){
    char *sequenceNoColon = removeColon(sequence);
    /* If the sequence is empty */
    if (isEmpty(sequence))
        return FALSE;

    /* If the label name is too long */
    if (strlen(sequence) > MAX_LABEL_SIZE_WITH_COLON) {
        errorCode = LABEL_NAME_TOO_LONG;
        return FALSE;
    }

    /* If the sequence dose'nt begin with a letter, its not a label */
    if (!isalpha(*sequence)) {
        errorCode = LABEL_NAME_INVALID;
        return FALSE;
    }

    /* If the sequence is a register name, instruction or directive, its not a label */
    if (isRegisterName(sequenceNoColon) || isInstructionName(sequenceNoColon) || isDirectiveName(sequenceNoColon)) {
        errorCode = LABEL_NAME_RESERVED;
        return FALSE;
    }

    /* Skip the alpha-numeric characters until the colon */
    while (!isEmpty(sequence) && isalnum(*sequence) && *sequence != ':')
        ++sequence;

    /* If the sequence doesn't end with ':', it is not a label */
    if (*sequence != ':') {
        errorCode = LABEL_NAME_INVALID;
        return FALSE;
    }

    /* Skip the colon */
    ++sequence;
    /* If the sequence doesn't end with ':', it is not a valid label */
    return isEmpty(sequence);
}

Boolean isLegalLabelNoColon(char *sequence){
    /* If the sequence is empty */
    if (isEmpty(sequence))
        return FALSE;

    /* If the label name is too long */
    if (strlen(sequence) > MAX_LABEL_SIZE_WITH_COLON) {
        errorCode = LABEL_NAME_TOO_LONG;
        return FALSE;
    }

    /* If the sequence is a register name, instruction or directive, its not a label */
    if (isRegisterName(sequence) || isInstructionName(sequence) || isDirectiveName(sequence)) {
        errorCode = LABEL_NAME_RESERVED;
        return FALSE;
    }

    /* If the sequence dose'nt begin with a letter, its not a label */
    if (!isalpha(*sequence)) {
        errorCode = LABEL_NAME_INVALID;
        return FALSE;
    }

    /* Skip the alpha-numeric characters until the colon */
    while (!isEmpty(sequence) && isalnum(*sequence))
        ++sequence;

    return isEmpty(sequence);
}

Boolean isDefinedLabel(char *sequence) {
    /* Return true if the label is legal and if it's inside the symbol table */
    return searchByName(&symbolTable, sequence) != NULL;
}

Boolean hasTwoCommas (char *line) {
    if (isEmpty(line))
        return FALSE;

    /* If there are two consecutive characters that are ',', return true*/
    for (; !isEmpty(line); ++line)
        if (*line == ',' && *(line + 1) == ',')
            return TRUE;

    return FALSE;
}

AddressingMode getAddressingModeFirstPass(char *operand) {
    /* If the operand sequence is empty, it has no addressing mode */
    if (isEmpty(operand))
        return UNKNOWN_ADDRESSING_MODE;

    /*     Immediate Addressing Check     */
    /* If the sequence starts with '#' */
    if (*operand == '#')
        /* If the sequence after the '#' is a valid number, the addressing mode is Immediate */
        if (isNumber(++operand))
            return IMMEDIATE;

    /*  Register Indirect Addressing Check  */
    /* If the sequence starts with '*' */
    if (*operand == '*')
        /* If the sequence after the '*' is a valid register name, the addressing mode is Register Indirect */
        if (isRegisterName(++operand))
            return REGISTER_INDIRECT;

    /* Register Direct Addressing Check */
    /* If the sequence is a valid register name, the addressing mode is Register Direct */
    if (isRegisterName(operand))
        return REGISTER_DIRECT;

    /*     Direct Addressing Check        */
    /* If the operand is a legal label, the addressing mode is Direct */
    if (isLegalLabelNoColon(operand))
        return DIRECT;

    /* If all other cases yielded false, the addressing mode is unknown */
    return UNKNOWN_ADDRESSING_MODE;
}

AddressingMode getAddressingModeSecondPass(char *operand) {
    /* If the operand sequence is empty, it has no addressing mode */
    if (isEmpty(operand))
        return UNKNOWN_ADDRESSING_MODE;

    /*     Immediate Addressing Check     */
    /* If the sequence starts with '#' */
    if (*operand == '#')
        /* If the sequence after the '#' is a valid number, the addressing mode is Immediate */
        if (isNumber(++operand))
            return IMMEDIATE;

    /*     Direct Addressing Check        */
    /* If the operand is a defined label (inside the symbol table), the addressing mode is Direct */
    if (isDefinedLabel(operand))
        return DIRECT;

    /*  Register Indirect Addressing Check  */
    /* If the sequence starts with '*' */
    if (*operand == '*')
        /* If the sequence after the '*' is a valid register name, the addressing mode is Register Indirect */
        if (isRegisterName(++operand))
            return REGISTER_INDIRECT;

    /* Register Direct Addressing Check */
    /* If the sequence is a valid register name, the addressing mode is Register Direct */
    if (isRegisterName(operand))
        return REGISTER_DIRECT;

    /* If all other cases yielded false, the addressing mode is unknown */
    return UNKNOWN_ADDRESSING_MODE;
}

char *getLabel(char *line) {
    int i;
    /* Will hold the label */
    char *label = (char *) calloc(MAX_LABEL_SIZE_WITH_COLON * sizeof(char), 1);

    /* remove all spaces at the beginning */
    line = strip(line);

    if (strchr(line, ':') == NULL)
        return NULL;

    /* Copy the label to #label until ':' */
    for (i = 0; !isEmpty(line) && line[i] != ':'; ++i)
        label[i] = line[i];

    /* If the next character is a ':', copy it to #label, else there is no label definition */
    if (line[i] == ':')
        label[i] = ':';
    else
        return NULL;

    /* If the label is illegal, return NULL */
    if (!isLegalLabel(label))
        return NULL;


    /* Return the label without the colon */
    label[i] = '\0';
    return label;
}

Directive getDirective(char *line) {
    char temp[2];
    char *dir = (char *) calloc(sizeof(char) * MAX_DIRECTIVE_LENGTH, 1);
    line = strip(line);

    /* Skip to the dot */
    while (!isEmpty(line) && *line != '.')
        ++line;

    /* Read all chars until a space */
    while (!isEmpty(line) && !isspace(*line)) {
        temp[0] = *line;
        temp[1] = '\0';
        strcat(dir, temp);
        ++line;
    }

    /* Return the correct directive */
    if (strcmp(dir, ".string") == 0) {
        free(dir);
        return STRING_DIR;

    } else if (strcmp(dir, ".data") == 0) {
        free(dir);
        return DATA_DIR;

    } else if (strcmp(dir, ".entry") == 0) {
        free(dir);
        return ENTRY_DIR;

    } else if (strcmp(dir, ".extern") == 0) {
        free(dir);
        return EXTERN_DIR;

    } else {
        free(dir);
        return UNKNOWN_DIRECTIVE;
    }
}

Boolean startWithDot(char *line) {
    char *label = getLabel(line);

    if (isEmpty(line))
        return FALSE;

    line = strip(line);
    if (label != NULL) {
        line += strlen(label) + 1; /* Skip the label and the ':' */
        line = strip(line); /* Skip spaces */
    }

    return *line == '.'; /* Return whether if the line starts with '.' */
}

int getNumber(char *sequence) {
    /* sign wil hold the sign of the number (+1 / -1) */
    int sign = 1;
    /* abs will hold the absolute value of the number */
    int abs = 0;

    /* remove spaces at the beginning */
    sequence = strip(sequence);

    /* Remove starting # */
    if (*sequence == '#')
        ++sequence;

    /* if the sequence starts with '-' */
    if (*sequence == '-')
        sign = -1;

    /* If the sequence has a sign symbol */
    if (*sequence == '-' || *sequence == '+')
        /* skip it */
        ++sequence;

    /* Extract the number from the sequence */
    for (; *sequence; ++sequence)
        if (isdigit(*sequence))
            abs = (10 * abs) + (*sequence - '0');

    /* Return the number with sign */
    return sign * abs;
}

char *getString(char *sequence) {
    int secondQuoteLoc = 0;
    /* Get rid of the first '"' */
    ++sequence;

    /* Get rid of the second '"' */
    for (; !isEmpty(sequence) && sequence[secondQuoteLoc] != '\"'; ++secondQuoteLoc)
        ;

    sequence[secondQuoteLoc] = '\0';
    return sequence;
}

Instruction getInstruction(char *line) {
    char inst[8] = {0}, *label;
    int i = 0;

    if (isEmpty(line))
        return UNKNOWN_INST;

    line = strip(line);

    /* If there is no label, skip spaces */
    if ((label = getLabel(line)) == NULL)
        line = strip(line);
    else { /* If there is a label, skip it and following spaces */
        line += strlen(label) + 1;
        line = strip(line);
    }

    /* Copy the instruction */
    while (!isEmpty(line) && !isspace(*line)) {
        inst[i] = *line;
        ++i, ++line;
    }

    /* Return the instruction code */
    if      (strcmp(inst, "mov") == 0)
        return MOV_INST;
    else if (strcmp(inst, "cmp") == 0)
        return CMP_INST;
    else if (strcmp(inst, "add") == 0)
        return ADD_INST;
    else if (strcmp(inst, "sub") == 0)
        return SUB_INST;
    else if (strcmp(inst, "lea") == 0)
        return LEA_INST;
    else if (strcmp(inst, "clr") == 0)
        return CLR_INST;
    else if (strcmp(inst, "not") == 0)
        return NOT_INST;
    else if (strcmp(inst, "inc") == 0)
        return INC_INST;
    else if (strcmp(inst, "dec") == 0)
        return DEC_INST;
    else if (strcmp(inst, "jmp") == 0)
        return JMP_INST;
    else if (strcmp(inst, "bne") == 0)
        return BNE_INST;
    else if (strcmp(inst, "red") == 0)
        return RED_INST;
    else if (strcmp(inst, "prn") == 0)
        return PRN_INST;
    else if (strcmp(inst, "jsr") == 0)
        return JSR_INST;
    else if (strcmp(inst, "rts") == 0)
        return RTS_INST;
    else if (strcmp(inst, "stop") == 0)
        return STOP_INST;
    else
        return UNKNOWN_INST;
}

int getInstructionLength(Instruction code) {
    switch (code) {
        case MOV_INST:
        case CMP_INST:
        case ADD_INST:
        case SUB_INST:
        case LEA_INST:
        case CLR_INST:
        case NOT_INST:
        case INC_INST:
        case DEC_INST:
        case JMP_INST:
        case BNE_INST:
        case RED_INST:
        case PRN_INST:
        case JSR_INST:
        case RTS_INST:
        /* Those instructions has 3 letters */
            return 3;
        case STOP_INST:
        /* That instructions has 4 letters */
            return 4;
        case UNKNOWN_INST:
        default:
            return 0;
    }
}

char *getFirstOperand(char *line) {
    int i = 0;
    char *operand = (char *) calloc(MAX_OPERAND_LENGTH * sizeof(char), 1), *label;
    Instruction inst = getInstruction(line);

    /* If the line is empty */
    if (isEmpty(line))
        return NULL;

    /* Skip leading spaces */
    line = strip(line);

    /* If there is no label, skip spaces */
    if ((label = getLabel(line)) == NULL)
        line = strip(line);
    else { /* If there is a label, skip it and following spaces */
        line += strlen(label) + 1;
        line = strip(line);
    }

    /* Skip the instruction and following spaces */
    line += getInstructionLength(inst);
    line = strip(line);

    if (isEmpty(line))
        return NULL;

    /* Copy the operand */
    while (!isEmpty(line) && *line != ',') {
        operand[i] = *line;
        ++i, ++line;
    }

    /* remove trailing spaces from the operand */
    for (i = 0; operand[i]; ++i)
        if (isspace(operand[i])) {
            operand[i] = '\0';
            break;
        }

    return operand;
}

char *getSecondOperand(char *line) {
    int i = 0;
    Instruction inst = getInstruction(line);
    char *operand    = (char *) calloc(MAX_OPERAND_LENGTH * sizeof(char), 1), *label;
    char *firstOp    = getFirstOperand(line);

    /* If the line is empty */
    if (isEmpty(line))
        return NULL;

    /* Skip leading spaces */
    line = strip(line);

    /* If there is no label, skip spaces */
    if ((label = getLabel(line)) == NULL)
        line = strip(line);
    else { /* If there is a label, skip it and following spaces */
        line += strlen(label) + 1;
        line = strip(line);
    }

    /* Skip the instruction and following spaces */
    line += getInstructionLength(inst);
    line = strip(line);

    /* Skip the first operand */
    if (firstOp == NULL)
        return NULL;

    line += strlen(firstOp);
    line = strip(line);

    if (isEmpty(line))
        return NULL;

    /* Skip the comma */
    ++line;
    line = strip(line);

    /* Copy the operand */
    while (!isEmpty(line) && !isspace(*line)) {
        operand[i] = *line;
        ++i, ++line;
    }

    /* remove trailing spaces from the operand */
    for (i = 0; operand[i]; ++i)
        if (isspace(operand[i])) {
            operand[i] = '\0';
            break;
        }

    return operand;
}

Register getRegister(char *seq) {
    /* If the sequence starts with '*', return the number 2 chars after it else, return the number 1 char after it */
    return (*seq == '*') ? *(seq + 2) - '0' : *(seq + 1) - '0';
}

int getOperandInstructionWidth(AddressingMode src, AddressingMode dest) {
    Boolean isSrcRegister  = BOOLEANIZE(src  == REGISTER_INDIRECT || src  == REGISTER_DIRECT);
    Boolean isDestRegister = BOOLEANIZE(dest == REGISTER_INDIRECT || dest == REGISTER_DIRECT);
    int count = 2;

    /* If both operands are registers */
    if (isDestRegister && isSrcRegister)
        count--;

    /* If one or more of the operands are missing */
    if (dest == UNKNOWN_ADDRESSING_MODE)
        count--;

    if (src == UNKNOWN_ADDRESSING_MODE)
        count--;

    return count;
}

/** Memory Operations And Encoding Operations **/
void installWordInData(Word w) {
    dataImage[dc] = w; /* Install the word */
    ++dc; /* Increment dc to point to the new free location */
    ++dataWordsInstalled;
}

void installWordInCode(Word w) {
    machineCodeImage[ic] = w; /* Install the word */
    ++ic; /* Increment ic to point to the new free location */
    ++codeWordsInstalled;
}

/** Error Handling Functions **/
void alertLineError(int lineNum, Error err) {
    switch (err) { /* Go through all errors */
        case LABEL_NAME_TOO_LONG:
            printf("Error Stats For Line no. %d:\tThe Label Is Too Long\n", lineNum);
            break;
        case LABEL_NAME_INVALID:
            printf("Error Stats For Line no. %d:\tThe Label Name Is Invalid\n", lineNum);
            break;
        case LABEL_NAME_RESERVED:
            printf("Error Stats For Line no. %d:\tThe Label Name Is Reserved\n", lineNum);
            break;
        case LABEL_NAME_ALREADY_EXIST:
            printf("Error Stats For Line no. %d:\tThe Label Name Is Already Defined\n", lineNum);
            break;
        case INVALID_CMD_NAME:
            printf("Error Stats For Line no. %d:\tThe Instruction Name Is Invalid\n", lineNum);
            break;
        case INVALID_DIR_NAME:
            printf("Error Stats For Line no. %d:\tThe Directive Name Is Invalid\n", lineNum);
            break;
        case INVALID_OPERANDS_DATA_DIR:
            printf("Error Stats For Line no. %d:\tNo Operands Were Given To The Data Directive\n", lineNum);
            break;
        case INVALID_OPERANDS_STRING_DIR:
            printf("Error Stats For Line no. %d:\tNo Operand Were Given To The String Directive\n", lineNum);
            break;
        case INVALID_OPERAND_NUMBER_CMD:
            printf("Error Stats For Line no. %d:\tInvalid Number Of Operands For That Instruction\n", lineNum);
            break;
        case INVALID_ADDR_MODES:
            printf("Error Stats For Line no. %d:\tInvalid Addressing Modes For That Instruction\n", lineNum);
            break;
        case ENTRY_LABEL_DOSENT_EXIST:
            printf("Error Stats For Line no. %d:\tEntry Operand Is Not Defined\n", lineNum);
            break;
        case EXTERN_OPERAND_INVALID:
            printf("Error Stats For Line no. %d:\tExtern Operand Is Not Valid\n", lineNum);
            break;
        case EXTERN_OPERAND_ALREADY_EXIST:
            printf("Error Stats For Line no. %d:\tExtern Operand Already Exist\n", lineNum);
            break;
        case INVALID_COMMAS:
            printf("Error Stats For Line no. %d:\tTwo Or More Consecutive Commas\n", lineNum);
            break;
        case STRING_OPERAND_INVALID:
            printf("Error Stats For Line no. %d:\tThe Operand Of The String Directive Is Invalid\n", lineNum);
            break;
        case DATA_OPERAND_INVALID:
            printf("Error Stats For Line no. %d:\tOne Or More Operands Of The Data Directive Is Invalid\n", lineNum);
            break;
        case NO_ERROR:
        default:
            break;
    }
}

/** Misc. **/
void initializeGlobalVariables() {
    int i;
    /* Reset memory counters */
    ic = 0, dc = 0;
    codeWordsInstalled = 0, dataWordsInstalled = 0;

    /* Reset error code */
    errorCode = NO_ERROR;

    /* Reset dynamic tables */
    symbolTable = NULL;
    externEventTable = NULL;

    /* Reset file related info */
    shouldOutputEntry = FALSE, shouldOutputExtern = FALSE;

    /* Reset data & code images */
    for (i = 0; i < MEMORY_SIZE; ++i) {
        dataImage[i] = (unsigned ) 0;
        machineCodeImage[i] = (unsigned ) 0;
    }
}

int getNumberOfOperands(Instruction inst) {
    switch (inst) {
        case MOV_INST:
        case CMP_INST:
        case ADD_INST:
        case SUB_INST:
        case LEA_INST:
            return 2;
        case CLR_INST:
        case NOT_INST:
        case INC_INST:
        case DEC_INST:
        case JMP_INST:
        case BNE_INST:
        case RED_INST:
        case PRN_INST:
        case JSR_INST:
            return 1;
        case RTS_INST:
        case STOP_INST:
            return 0;
        case UNKNOWN_INST:
        default:
            return -1;
    }
}

char *toOctal(unsigned word) {
    char *oct = (char *) calloc(MAX_OCT_SIZE * sizeof(char), 1);
    sprintf(oct, "%05o", word);

    /* If #word represent a negative number, it will have leading 7's */
    while (strlen(oct) > 5)
        ++oct;

    return oct;
}
