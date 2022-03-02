/*****************************************
* Assembler Second Pass Header           *
* @author Zvi Badash                     *
* ****************************************
*/

#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

/*Imports */

/* Definitions */
#define SUFFIX_LENGTH 8
/* Function Prototypes */
/**
 * This function opens a file of type #t with mode #mode
 * **/
FILE *openFile(char *fileName, FileType t, char *mode);

/**
 * This function append a suffix #t to file #fileName
 * **/
char *appendFileSuffix(char *fileName, FileType t);

/**
 * This function create the output files
 * **/
void createOutputFiles(char *filename);


/**
 * This function writes the entry file
 * **/
void writeEntryFile(FILE *file);

/**
 * This function writes the extern file
 * **/
void writeExternFile(FILE *file);

/**
 * This function writes the object file
 * **/
void writeObjectFile(FILE *file);

#endif