/*****************************************
* External Variables Definitions         *
* @author Zvi Badash                     *
* ****************************************
*/
#ifndef EXTERN_VARS_H
#define EXTERN_VARS_H

#include "dataTypes.h"

/* Definitions */
#define MEMORY_SIZE 4096

int errorCode;
LabelList symbolTable;
ExternEventList externEventTable;
unsigned ic, codeWordsInstalled;
unsigned dc, dataWordsInstalled;
Word dataImage[MEMORY_SIZE];
Word machineCodeImage[MEMORY_SIZE];
Boolean shouldOutputEntry, shouldOutputExtern;

/* Type Definitions */

/* Function Prototypes */


#endif
