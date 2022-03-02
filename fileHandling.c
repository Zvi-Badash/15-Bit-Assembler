/*****************************************
* File Handling Operations               *
* @author Zvi Badash                     *
* ****************************************
*/

/* Imports */
#include "dataTypes.h"
#include "secondPass.h"
#include "fileHandling.h"
#include "externalVariables.h"
#include "utils.h"
#include "firstPass.h"

/* Functions */
char *appendFileSuffix(char *fileName, FileType t) {
    char *buffer = (char *) calloc((strlen(fileName) + SUFFIX_LENGTH) * sizeof(char), 1); /* Will hold the filename w/ the extension */

    if (isEmpty(fileName))
        return NULL;

    /* Copy the filename to the buffer */
    strcpy(buffer, fileName);

    switch (t) { /* Go through extension types */
        case ASM: /* Assembly file */
            strcat(buffer, ".as");
            break;
        case OBJ: /* Object file */
            strcat(buffer, ".obj");
            break;
        case ENT: /* Entry file */
            strcat(buffer, ".ent");
            break;
        case EXT: /* Extern file */
            strcat(buffer, ".ext");
            break;

        case UNKNOWN_FILE_TYPE:
        default:
            return NULL;
    }
    return buffer;
}

FILE *openFile(char *fileName, FileType t, char *mode) {
    FILE *fp; /* Will hold the file */
    char *fileNameWithExtension = appendFileSuffix(fileName, t); /* Will hold the file name */

    /* Use fopen to open the file with the extension */
    fp = fopen(fileNameWithExtension, mode);

    /* Free the memory occupied by the filename */
    free(fileNameWithExtension);
    return fp;
}

void writeEntryFile(FILE *file) {
    LabelPointer node = symbolTable;

    /* Go through the symbol table and print the ones marked as entry */
    for (; node; node = node->next)
        if (node->isEntry)
            fprintf(file, "%s\t%d\n", node->labelName, node->value);
}

void writeExternFile(FILE *file) {
    ExternEventPointer node = externEventTable;

    /* Go through the extern events list and print it */
    for (; node; node = node->next)
        fprintf(file, "%s\t%d\n", node->labelName, node->address);
}

void writeObjectFile(FILE *file) {
    unsigned i, currentAddressNumber = 0;

    /* Print the "header" of the object file */
    fprintf(file, "%d\t\t%d\n", codeWordsInstalled, dataWordsInstalled);

    /* Print the machine code image */
    for (i = 0; i < codeWordsInstalled; ++i, ++currentAddressNumber)
        fprintf(file, "%04d\t%s\n", currentAddressNumber + MEMORY_OFFSET, toOctal(machineCodeImage[i]));

    /* Print the data image */
    for (i = 0; i < dataWordsInstalled; ++i, ++currentAddressNumber)
        fprintf(file, "%04d\t%s\n", currentAddressNumber + MEMORY_OFFSET, toOctal(dataImage[i]));

}

void createOutputFiles(char *filename) {
    FILE *objFile, *entFile, *extFile;

    /* Create the object file */
    objFile = openFile(filename, OBJ, "w");
    writeObjectFile(objFile);
    fclose(objFile);

    if (shouldOutputExtern) { /* If an ext file should be created, make it */
        extFile = openFile(filename, EXT, "w");
        writeExternFile(extFile);
        fclose(extFile);
    }
    if (shouldOutputEntry) { /* If an ent file should be created, make it */
        entFile = openFile(filename, ENT, "w");
        writeEntryFile(entFile);
        fclose(entFile);
    }
}
