/* 
	Library Created By: Sargis S Yonan
	March 2017
*/ 

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "buffer.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif
#ifndef inb
	#define	inb(addr)			(addr)
#endif
#ifndef outw
	#define	outw(addr, data)	addr = (data)
#endif
#ifndef inw
	#define	inw(addr)			(addr)
#endif
#ifndef cbi
	#define cbi(reg,bit)	reg &= ~(_BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)	reg |= (_BV(bit))
#endif
#ifndef cli
	#define cli()			__asm__ __volatile__ ("cli" ::)
#endif
#ifndef BV
	#define BV _BV
#endif

//! Default uart baud rate.
/// This is the default speed after a uartInit() command,
/// and can be changed by using uartSetBaudRate().
#ifndef UART_DEFAULT_BAUD_RATE
#define UART_DEFAULT_BAUD_RATE	(9600)
#endif

// buffer memory allocation defines
// buffer sizes
#ifndef UART_TX_BUFFER_SIZE
//! Number of bytes for uart transmit buffer.
/// Do not change this value in uart.h, but rather override
/// it with the desired value defined in your project's global.h
#define UART_TX_BUFFER_SIZE		(0x0090)
#endif
#ifndef UART_RX_BUFFER_SIZE
//! Number of bytes for uart receive buffer.
/// Do not change this value in uart.h, but rather override
/// it with the desired value defined in your project's global.h
#define UART_RX_BUFFER_SIZE		(0x0080)
#endif

// define this key if you wish to use
// external RAM for the	UART buffers
//#define UART_BUFFER_EXTERNAL_RAM
#ifdef UART_BUFFER_EXTERNAL_RAM
	// absolute address of uart buffers
	#define UART_TX_BUFFER_ADDR	(0x1000)
	#define UART_RX_BUFFER_ADDR	(0x1100)
#endif

//! Type of interrupt handler to use for uart interrupts.
/// Value may be SIGNAL or INTERRUPT.
/// \warning Do not change unless you know what you're doing.
#ifndef UART_INTERRUPT_HANDLER
#define UART_INTERRUPT_HANDLER	SIGNAL
#endif

// compatibility with most newer processors
#ifdef UCSRB
	#define UCR					UCSRB
#endif
// compatibility with old Mega processors
#if defined(UBRR) && !defined(UBRRL)
	#define	UBRRL				UBRR
#endif
#if defined(__AVR_ATmega328P__)
   #define UDR               UDR0
   #define UCR               UCSR0B
   #define RXCIE            RXCIE0
   #define TXCIE            TXCIE0
   #define RXC               RXC0
   #define TXC               TXC0
   #define RXEN            RXEN0
   #define TXEN            TXEN0
   #define UBRRL            UBRR0L
   #define UBRRH            UBRR0H
   #define SIG_UART_TRANS      USART_TX_vect
   #define SIG_UART_RECV      USART_RX_vect
   #define SIG_UART_DATA      SIG_USART_DATA
#endif
// compatibility with megaXX8 processors
#if	defined(__AVR_ATmega88__)	|| \
	defined(__AVR_ATmega168__)	|| \
	defined(__AVR_ATmega644__)
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define RXCIE				RXCIE0
	#define TXCIE				TXCIE0
	#define RXC					RXC0
	#define TXC					TXC0
	#define RXEN				RXEN0
	#define TXEN				TXEN0
	#define UBRRL				UBRR0L
	#define UBRRH				UBRR0H
	#define SIG_UART_TRANS		SIG_USART_TRANS
	#define SIG_UART_RECV		SIG_USART_RECV
	#define SIG_UART_DATA		SIG_USART_DATA
#endif
// compatibility with mega169 processors
#if	defined(__AVR_ATmega169__)
	#define SIG_UART_TRANS		SIG_USART_TRANS
	#define SIG_UART_RECV		SIG_USART_RECV
	#define SIG_UART_DATA		SIG_USART_DATA
#endif
// compatibility with dual-uart processors
// (if you need to use both uarts, please use the uart2 library)
#if defined(__AVR_ATmega161__)
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define UBRRL				UBRR0
	#define SIG_UART_TRANS		SIG_UART0_TRANS
	#define SIG_UART_RECV		SIG_UART0_RECV
	#define SIG_UART_DATA		SIG_UART0_DATA
#endif
#if defined(__AVR_ATmega128__)
#ifdef UART_USE_UART1
	#define UDR					UDR1
	#define UCR					UCSR1B
	#define UBRRL				UBRR1L
	#define UBRRH				UBRR1H
	#define SIG_UART_TRANS		SIG_UART1_TRANS
	#define SIG_UART_RECV		SIG_UART1_RECV
	#define SIG_UART_DATA		SIG_UART1_DATA
#else
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define UBRRL				UBRR0L
	#define UBRRH				UBRR0H
	#define SIG_UART_TRANS		SIG_UART0_TRANS
	#define SIG_UART_RECV		SIG_UART0_RECV
	#define SIG_UART_DATA		SIG_UART0_DATA
#endif
#endif

// functions

//! Initializes uart.
/// \note	After running this init function, the processor
/// I/O pins that used for uart communications (RXD, TXD)
/// are no long available for general purpose I/O.
void _uart_driver_Init(uint32_t baudrate);

//! Initializes transmit and receive buffers.
/// Automatically called from uartInit()
void _uart_driver_InitBuffers(void);

//! Redirects received data to a user function.
///
void _uart_driver_SetRxHandler(void (*rx_func)(unsigned char c));

//! Sets the uart baud rate.
/// Argument should be in bits-per-second, like \c uartSetBaudRate(9600);
void _uart_driver_SetBaudRate(uint32_t baudrate);

//! Returns pointer to the receive buffer structure.
///
cBuffer* _uart_driver_GetRxBuffer(void);

//! Returns pointer to the transmit buffer structure.
///
cBuffer* _uart_driver_GetTxBuffer(void);

//! Sends a single byte over the uart.
/// \note This function waits for the uart to be ready,
/// therefore, consecutive calls to uartSendByte() will
/// go only as fast as the data can be sent over the
/// serial port.
void _uart_driver_SendByte(uint8_t data);

uint8_t _uart_is_available(void);

//! Gets a single byte from the uart receive buffer.
/// Returns the byte, or -1 if no byte is available (getchar-style).
int _uart_driver_GetByte(void);

//! Gets a single byte from the uart receive buffer.
/// Function returns TRUE if data was available, FALSE if not.
/// Actual data is returned in variable pointed to by "data".
/// Example usage:
/// \code
/// char myReceivedByte;
/// uartReceiveByte( &myReceivedByte );
/// \endcode
uint8_t _uart_driver_ReceiveByte(uint8_t* data);

//! Returns TRUE/FALSE if receive buffer is empty/not-empty.
///
uint8_t _uart_driver_ReceiveBufferIsEmpty(void);

//! Flushes (deletes) all data from receive buffer.
///
void _uart_driver_FlushReceiveBuffer(void);
void _uart_driver_FlushTransmitBuffer(void);

//! Add byte to end of uart Tx buffer.
///	Returns TRUE if successful, FALSE if failed (no room left in buffer).
uint8_t _uart_driver_AddToTxBuffer(uint8_t data);

//! Begins transmission of the transmit buffer under interrupt control.
///
void _uart_driver_SendTxBuffer(void);

//! Sends a block of data via the uart using interrupt control.
/// \param buffer	pointer to data to be sent
///	\param nBytes	length of data (number of bytes to sent)
uint8_t _uart_driver_SendBuffer(char *buffer, uint16_t nBytes);

#endif
