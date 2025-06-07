assembler: assembler.o macro.o firstpass.o memorylist.o symboltable.o typecheck.o secondpass.o binarytohexa.o
	gcc -ansi -Wall -pedantic assembler.o macro.o firstpass.o memorylist.o symboltable.o typecheck.o secondpass.o binarytohexa.o -o assembler

assembler.o: assembler.c assembler.h macro.h memorylist.h symboltable.h firstpass.h secondpass.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

macro.o: macro.c macro.h
	gcc -c -ansi -Wall -pedantic macro.c -o macro.o

firstpass.o: firstpass.c firstpass.h macro.h symboltable.h memorylist.h
	gcc -c -ansi -Wall -pedantic firstpass.c -o firstpass.o

memorylist.o: memorylist.c memorylist.h
	gcc -c -ansi -Wall -pedantic memorylist.c -o memorylist.o

symboltable.o: symboltable.c symboltable.h
	gcc -c -ansi -Wall -pedantic symboltable.c -o symboltable.o

typecheck.o: typecheck.c typecheck.h
	gcc -c -ansi -Wall -pedantic typecheck.c -o typecheck.o

secondpass.o: secondpass.c secondpass.h symboltable.h memorylist.h
	gcc -c -ansi -Wall -pedantic secondpass.c -o secondpass.o

binarytohexa.o: binarytohexa.c binarytohexa.h memorylist.h
	gcc -c -ansi -Wall -pedantic binarytohexa.c -o binarytohexa.o