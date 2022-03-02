assembler: assembler.o fileHandling.o secondPass.o firstPass.o dataTypes.o utils.o
	gcc -g -ansi -Wall -pedantic assembler.o fileHandling.o secondPass.o firstPass.o dataTypes.o utils.o  -o assembler

assembler.o: assembler.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

utils.o: utils.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o

dataTypes.o: dataTypes.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic dataTypes.c -o dataTypes.o

firstPass.o: firstPass.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic firstPass.c -o firstPass.o

secondPass.o: secondPass.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic secondPass.c -o secondPass.o

fileHandling.o: fileHandling.c utils.h dataTypes.h externalVariables.h mainHeader.h firstPass.h secondPass.h fileHandling.h
	gcc -c -ansi -Wall -pedantic fileHandling.c -o fileHandling.o
