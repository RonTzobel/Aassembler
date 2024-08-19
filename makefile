
CC = gcc 
CFLAGS = -ansi -Wall -pedantic 
EXE_DEPS = assembler.o macro.o firstpass.o secondpass.o assemblytomachine.o validation.o symbols.o coding.o 

## Executable
assembler: $(EXE_DEPS) 
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler.o: assembler.c assembly_to_machine.h macro.h validation.h first_pass.h second_pass.h coding.h symbols.h
	$(CC) -c assembler.c $(CFLAGS) -o $@

macro.o: macro.c macro.h assembly_to_machine.h macro.h validation.h symbols.h
	$(CC) -c macro.c $(CFLAGS) -o $@

firstpass.o: first_pass.c first_pass.h assembly_to_machine.h validation.h symbols.h 
	$(CC) -c first_pass.c $(CFLAGS) -o $@

secondpass.o: second_pass.c second_pass.h assembly_to_machine.h validation.h coding.h symbols.h
	$(CC) -c second_pass.c $(CFLAGS) -o $@

validation.o: validation.c validation.h  assembly_to_machine.h symbols.h
	$(CC) -c validation.c $(CFLAGS) -o $@

assemblytomachine.o: assembly_to_machine.c assembly_to_machine.h macro.h validation.h first_pass.h second_pass.h coding.h symbols.h
	$(CC) -c assembly_to_machine.c $(CFLAGS) -o  $@

symbols.o: symbols.c symbols.h 
	$(CC) -c symbols.c $(CFLAGS) -o  $@

coding.o: coding.c coding.h assembly_to_machine.h symbols.h 
	$(CC) -c coding.c $(CFLAGS) -o  $@

