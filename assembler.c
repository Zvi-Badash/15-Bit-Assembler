#include "utils.h"
#include "dataTypes.h"
#include "firstPass.h"
#include "fileHandling.h"
#include "externalVariables.h"

int main(int argc, char **argv) {
    FILE *fp; /* Will hold the current file */
    Boolean hadSuccessfulRun = FALSE;

    if (argc == 1) { /* If no file was given */
        printf("No File Was Given, Try The Command \"assembler x y\", Where x.as and y.as "
                        "Are Existing Assembly Files.");
        exit(EXIT_FAILURE);
    }

    while (--argc > 0) { /* Go through all files */
        if ((fp = openFile(*++argv, ASM, "r"))) { /* Open the file as an assembly file */
            initializeGlobalVariables(); /* Reset all global variables from last run */
            firstPass(*argv, fp); /* Do the first pass on the file, second pass and file creation will be called from there */
            fclose(fp); /* Close the current file */
            hadSuccessfulRun = TRUE;
        } else /* If fopen returned NULL */
            printf("\nERROR: Couldn't Open File %s, Try To Check If It Exists,"
                            " And If You Have The Correct Permissions To Open It.\n", *argv);
    }

   if (hadSuccessfulRun) {
        freeSymbolTable(&symbolTable);
        freeExternEventsTable(&externEventTable);
    }

    return EXIT_SUCCESS;
}