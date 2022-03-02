/*****************************************
* Assembler Data Type Operations         *
* @author Zvi Badash                     *
* ****************************************
*/
/* Imports */
#include "dataTypes.h"
#include "externalVariables.h"

/* Functions */
/*** Word Bit Fiddling Operation ***/
void setState(Word *w, unsigned int pos, Bit state) {
    /* If the position does not exist */
    if (pos > BITS_IN_WORD || pos < 0)
        return;

    if (BOOLEANIZE(state) == ON) /* Used BOOLEANIZE to enforce Bit type */
        *w |= 1U << pos; /* Turn the bit on */
    else
        *w &= ~(1U << pos); /* Turn the bit off */
}

Bit getState(Word w, unsigned int pos) {
    unsigned char mask = 1U << pos;

    /* If the position does not exist */
    if (pos > BITS_IN_WORD || pos < 0) {
        return OFF;
    }

    return BOOLEANIZE(w & mask);  /* Used BOOLEANIZE to enforce Bit type */
}

void __prnWord(Word w){
    unsigned int mask = 1U << (BITS_IN_WORD - 1);
        while (mask) {
            printf("%d", BOOLEANIZE(mask & w));
            mask >>= 1U;
        }
        printf("\n");
}

/*** Linked Lists Operations ***/
void insertLabel(LabelList *lst, unsigned int valueP, char *labelNameP, LabelFeature featureP, Boolean isEntryP) {
    LabelPointer nodeToAdd;

    /* If the label already exist */
    if (searchByName(&symbolTable, labelNameP) != NULL) {
        errorCode = LABEL_NAME_ALREADY_EXIST;
        return;
    }

    /* Allocating memory for the new node */
    nodeToAdd = (LabelPointer) malloc(sizeof(struct LabelNode));
    if (nodeToAdd == NULL) {
        /* Alert that the memory allocation failed in #insertLabel */
        perror("insertLabel");
        exit(EXIT_FAILURE);
    }

    /* Initializing the new node */
    nodeToAdd->labelName = (char *) malloc(80);
    nodeToAdd->value     = valueP;            /* Set "value" field to #valueP         */
    strcpy(nodeToAdd->labelName, labelNameP); /* Set "labelName" field to #labelNameP */
    nodeToAdd->feature   = featureP;          /* Set "feature" field to #featureP     */
    nodeToAdd->isEntry   = isEntryP;          /* Set "isEntryP" field to #isEntryP    */

    /* Attaching the node to #lst at the beginning */
    nodeToAdd->next = *lst; /* Set "next" field to the head of the list #lst. (Could be NULL) */
    *lst = nodeToAdd;       /* Set the head of the list to the new label.                     */
}

LabelPointer searchByName(LabelList *lst, char *labelNameP) {
    /* Create a node to traverse the list */
    LabelPointer tNode = *lst;

    /* If the list is empty, return NULL */
    if (tNode == NULL)
        return NULL;

    /* Traverse the list, if found, return the node */
    for (; tNode != NULL; tNode = tNode->next)
        if (strcmp(labelNameP, tNode->labelName) == 0)
            return tNode;

    /* Else, return NULL */
    return NULL;
}

void addExternEvent(ExternEventList *lst, char *labelNameP, unsigned addressP) {
    /* Allocating memory for the new node */
    ExternEventPointer nodeToAdd = (ExternEventPointer) malloc(sizeof(struct ExternNode));

    if (nodeToAdd == NULL) {
        /* Alert that the memory allocation failed in #insertLabel */
          perror("addExternEvent");
        exit(EXIT_FAILURE);
    }

    /* Initializing the new node */
    nodeToAdd->address   = addressP;   /* Set "value" field to #valueP         */
    nodeToAdd->labelName = labelNameP; /* Set "labelName" field to #labelNameP */

    /* Attaching the node to #lst at the beginning */
    nodeToAdd->next = *lst; /* Set "next" field to the head of the list #lst. (Could be NULL) */
    *lst = nodeToAdd;       /* Set the head of the list to the new label.                     */
}

void freeSymbolTable(LabelList *lst) {
    LabelPointer tmp;

    /* Free the linked list */
    while (*lst != NULL) {
        tmp = *lst;
        *lst = (*lst)->next;
        free(tmp);
    }
}

void freeExternEventsTable(ExternEventList *lst) {
    ExternEventPointer tmp;

    /* Free the linked list */
    while (*lst != NULL) {
        tmp = *lst;
        *lst = (*lst)->next;
        free(tmp);
    }
}

void __prnNode(LabelPointer tNode){
    if (!tNode) {
        printf("NULL");
        return;
    }
    printf("Value: %d, Name: %s, Feature: %s, Entry: %s\n",
            tNode->value,
            tNode->labelName,
            (tNode->feature == DATA_FEATURE) ? "DATA" : (tNode->feature == CODE_FEATURE) ? "CODE" : "EXTERN",
            (tNode->isEntry) ? "TRUE" : "FALSE");
}

void __prnExternEvent(ExternEventPointer tNode) {
    if (!tNode) {
        printf("NULL");
        return;
    }
   printf("Address: %d, Name: %s\n", tNode->address, tNode->labelName);
}

void __prnList(LabelList *lst){
    LabelPointer tNode = *lst;
    int counter = 1;

    if (tNode == NULL)
        printf("Label List Is Empty.\n");

    for (; tNode; tNode = tNode->next, ++counter){
        printf("Node No. %d ", counter);
        __prnNode(tNode);
    }

    printf("\n");
}

void __prnExternList(ExternEventList *lst) {
    ExternEventPointer tNode = *lst;
    int counter = 1;

    if (tNode == NULL)
        printf("Label List Is Empty.\n");

    for (; tNode; tNode = tNode->next, ++counter){
        printf("Node No. %d ", counter);
        __prnExternEvent(tNode);
    }

    printf("\n");
}
