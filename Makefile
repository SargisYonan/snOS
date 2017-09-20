# MICROCONTROLELR DEFINITIONS #
SHELL := /bin/bash

CC						= avr-gcc
BURNER					= avrdude
CC_AVR					= atmega328p
F_CPU					= 16000000UL
BURNER_AVR				= ATMEGA328P
BURNER_PROGRAMMER		= Arduino
COMPILER_PATH			= /dev/tty.usbmodem1411
OBJECT_COPY				= avr-objcopy
BAUDRATE				= 115200
###########################################

# CC GLOBAL DEFINES
CC_GLOBAL_DEFINES 			= -D F_CPU=${F_CPU}

# MAIN SOURCE
PLATFORM_CSOURCES	= platform/avr/avr_pub_sub_main.c

####################
# UART DRIVER & BUFFER
PLATFORM_INCLUDES	+= -I platform/avr/drivers/uart_driver
PLATFORM_INCLUDES	+= -I platform/avr/protocols/leds
PLATFORM_INCLUDES	+= -I platform/avr/protocols/button
PLATFORM_INCLUDES	+= -I platform/avr/protocols/interrupts

# UART PROTOCOL PLATFORM
PLATFORM_INCLUDES += -I platform/avr/protocols/uart

####################
# UART #
PLATFORM_CSOURCES	+= platform/avr/drivers/uart_driver/uart_driver.c
PLATFORM_CSOURCES	+= platform/avr/protocols/leds/led.c
PLATFORM_CSOURCES	+= platform/avr/protocols/button/button.c
PLATFORM_CSOURCES	+= platform/avr/protocols/interrupts/interrupts.c
# BUFFER #
PLATFORM_CSOURCES	+= platform/avr/drivers/uart_driver/buffer.c
# snOS PLATFORM #
PLATFORM_CSOURCES	+= platform/avr/protocols/uart/uart.c
####################

######################################################
C_STD						= gnu99
OBJECTS	 					= ./*.o
EXECUTABLE					= snos_avr_test
########### COMPILER OPTIONS ##############
CFLAGS						+= -Wall
CFLAGS						+= -Werror
CFLAGS						+= -Wextra
CFLAGS						+= -Wimplicit
CFLAGS						+= -std=${C_STD}
# CC OPTIONS #
# OPTIMIZATION FLAG
CC_COMP_OPTIONS				+= -Os
# UC DEF
CC_COMP_OPTIONS				+= -mmcu=${CC_AVR}
# GLOBAL DEFINES FLAGS
CC_COMP_OPTIONS				+= ${CC_GLOBAL_DEFINES}
# CC WARNING FLAGS
CC_COMP_OPTIONS				+= ${CFLAGS}
# HEADER FILES
CC_COMP_OPTIONS				+= ${PLATFORM_INCLUDES}
# SOURCE FILES
CC_COMP_OPTIONS				+= -c ${CSOURCE}

# CC LINKER OPTIONS #
CC_LN_OPTIONS				+= -mmcu=${CC_AVR}
CC_LN_OPTIONS				+= -Wl,-u,vfprintf
CC_LN_OPTIONS				+= -lm
CC_LN_OPTIONS				+= -lprintf_flt
CC_LN_OPTIONS				+= -o${EXECUTABLE}.elf
CC_LN_OPTIONS				+= ${OBJECTS}

SNOS_INCLUDE_PATH	+= -I .

SNOS_INCLUDE_PATH	+= -I platform/

SNOS_INCLUDE_PATH 	+= -I system/
SNOS_INCLUDE_PATH	+= -I system/snos_error_codes/
SNOS_INCLUDE_PATH	+= -I system/snos_alloc/
SNOS_INCLUDE_PATH	+= -I system/snos_hash/
SNOS_INCLUDE_PATH	+= -I system/snos_timer/

SNOS_INCLUDE_PATH	+= -I structures/
SNOS_INCLUDE_PATH	+= -I structures/list/
SNOS_INCLUDE_PATH	+= -I structures/snos_tasks/

SNOS_INCLUDE_PATH	+= -I connect/

SNOS_CSOURCES	+= snos.c

SNOS_CSOURCES	+= platform/ports/snos_port.c

SNOS_CSOURCES	+= connect/snos_connect.c
SNOS_CSOURCES	+= connect/snos_pub_sub.c

SNOS_CSOURCES	+= system/snos_alloc/snos_alloc.c
SNOS_CSOURCES	+= system/snos_hash/snos_hash.c
SNOS_CSOURCES	+= system/snos_timer/snos_timer.c

SNOS_CSOURCES	+= structures/list/list.c
SNOS_CSOURCES	+= structures/snos_tasks/snos_task_manager.c


OBJECTS := *.o

all:
	# compiling snos
	${CC} ${CC_COMP_OPTIONS} $(CFLAGS) ${SNOS_INCLUDE_PATH} ${PLATFORM_INCLUDES} -c ${SNOS_CSOURCES} ${PLATFORM_CSOURCES}
	# making platform
	${CC} ${CC_LN_OPTIONS}

	${OBJECT_COPY} -j .text -j .data -O ihex ${EXECUTABLE}.elf ${EXECUTABLE}.hex

burn:
	${BURNER} -V -P ${COMPILER_PATH} -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex
	#${BURNER} -F -p ${BURNER_AVR} -c ${BURNER_PROGRAMMER} -e -b ${BAUDRATE} -U flash:w:${EXECUTABLE}.hex

clean:
	rm *.elf *.hex *.o