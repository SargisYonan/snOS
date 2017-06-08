CC           = avr-gcc
CFLAGS       = -Wall -Os -std=gnu99
OBJCOPY      = objcopy

# include path to AVR library
INCLUDE_PATH 	= /usr/lib/avr/include

INCLUDE_PATH 	+= -I system/
INCLUDE_PATH	+= -I system/snos_error_codes/
INCLUDE_PATH	+= -I system/snos_alloc/

INCLUDE_PATH	+= -I structures/
INCLUDE_PATH	+= -I structures/list/
INCLUDE_PATH	+= -I structures/snos_tasks/

CSOURCES	+= snos.c

CSOURCES	+= system/snos_alloc/snos_alloc.c

CSOURCES	+= structures/list/list.c
CSOURCES	+= structures/snos_tasks/snos_task_manager.c


OBJECTS := *.o

snos_substrate :
	# compiling snos.c
	$(CC) $(CFLAGS) -I ${INCLUDE_PATH} -c ${CSOURCES}

snOS : snos_substrate
	# linking object code to binary
	$(CC) -o snOS ${OBJECTS} -lm
	rm -f *.o

clean:
	rm -f *.OBJ *.LST *.o *.gch *.out *.hex *.map