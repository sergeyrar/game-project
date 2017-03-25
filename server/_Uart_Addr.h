/*
 ============================================================================
 Name        : _Uart_Addr.h
 Author      : Alexander Vovk
 Version     :
 Created on  : Oct 15, 2011
 Copyright   : RT-ED
 Description :
 ============================================================================
 ============================================================================
 */

#ifndef _UART_ADDR_H_
#define _UART_ADDR_H_


#define ULCON0		((volatile unsigned *)0x50000000)	//UART channel 0 line control register
#define UCON0		((volatile unsigned *)0x50000004)	//UART channel 0 control register
#define UFCON0		((volatile unsigned *)0x50000008)	//UART channel 0 FIFO control register
#define UMCON0		((volatile unsigned *)0x5000000c)	//UART channel 0 Modem control register
#define UTRSTAT0	((volatile unsigned *)0x50000010)	//UART channel 0 Tx/Rx status register
#define UERSTAT0	((volatile unsigned *)0x50000014)	//UART channel 0 Rx error status register
#define UFSTAT0		((volatile unsigned *)0x50000018)	//UART channel 0 FIFO status register
#define UMSTAT0		((volatile unsigned *)0x5000001c)	//UART channel 0 modem status register
#define UTXH0		((volatile unsigned *)0x50000020)	//UART channel 0 transmit buffer register
#define URXH0		((volatile unsigned *)0x50000024)	//UART channel 0 receive buffer register
#define UBRDIV0		((volatile unsigned *)0x50000028)	//UART channel 0 Baud rate divisor

#define ULCON1		((volatile unsigned *)0x50004000)	//UART channel 1 line control register
#define UCON1		((volatile unsigned *)0x50004004)	//UART channel 1 control register
#define UFCON1		((volatile unsigned *)0x50004008)	//UART channel 1 FIFO control register
#define UMCON1		((volatile unsigned *)0x5000400c)	//UART channel 1 Modem control register
#define UTRSTAT1	((volatile unsigned *)0x50004010)	//UART channel 1 Tx/Rx status register
#define UERSTAT1	((volatile unsigned *)0x50004014)	//UART channel 1 Rx error status register
#define UFSTAT1		((volatile unsigned *)0x50004018)	//UART channel 1 FIFO status register
#define UMSTAT1		((volatile unsigned *)0x5000401c)	//UART channel 1 modem status register
#define UTXH1		((volatile unsigned *)0x50004020)	//UART channel 1 transmit buffer register
#define URXH1		((volatile unsigned *)0x50004024)	//UART channel 1 receive buffer register
#define UBRDIV1		((volatile unsigned *)0x50004028)	//UART channel 1 Baud rate divisor

#define ULCON2		((volatile unsigned *)0x50008000)	//UART channel 2 line control register
#define UCON2		((volatile unsigned *)0x50008004)	//UART channel 2 control register
#define UFCON2		((volatile unsigned *)0x50008008)	//UART channel 2 FIFO control register
//		UMCON2		((volatile unsigned *)0x5000800c)	//Reserved
#define UTRSTAT2	((volatile unsigned *)0x50008010)	//UART channel 2 Tx/Rx status register
#define UERSTAT2	((volatile unsigned *)0x50008014)	//UART channel 2 Rx error status register
#define UFSTAT2		((volatile unsigned *)0x50008018)	//UART channel 2 FIFO status register
//		UMSTAT2		((volatile unsigned *)0x5000801c)	//Reserved
#define UTXH2		((volatile unsigned *)0x50008020)	//UART channel 2 transmit buffer register
#define URXH2		((volatile unsigned *)0x50008024)	//UART channel 2 receive buffer register
#define UBRDIV2		((volatile unsigned *)0x50008028)	//UART channel 2 Baud rate divisor

/*
 * UART configuration
 */

#define UOFFSET		(0x1000)							//UART offset to change next UART

//	(R/W)	UART LINE CONTROL REGISTER
//	ULCON0, ULCON1, and ULCON2

#define ULCON_RESET					(0x0)			//	Reset Value 0x0

#define ULCON_MODE					(0x40)			// [6]		Infrared Mode
#define ULCON_MODE_NORM				(0x0)			//*0 = Normal mode operation
#define ULCON_MODE_INFRA			(0x40)			// 1 = Infrared Tx/Rx mode

#define ULCON_PARITY				(0x38)			// [5:3]	Parity Mode
#define ULCON_PARITY_NONE			(0x0)			//*0xx = No parity
#define ULCON_PARITY_ODD			(0x20)			// 100 = Odd parity
#define ULCON_PARITY_EVEN			(0x28)			// 101 = Even parity
#define ULCON_PARITY_FORCED1		(0x30)			// 110 = Parity forced/checked as 1
#define	ULCON_PARITY_FORCED0		(0x38)			// 111 = Parity forced/checked as 0

#define ULCON_STOP					(0x4)			// [2]		Number of Stop Bit
#define ULCON_STOP_ONE				(0x0)			//*0 = One stop bit per frame
#define ULCON_STOP_TWO				(0x4)			// 1 = Two stop bit per frame

#define ULCON_LENGTH				(0x4)			// [1:0]	Word Length
#define ULCON_LENGTH_5				(0x0)			//*00 = 5-bits
#define ULCON_LENGTH_6				(0x1)			// 01 = 6-bits
#define ULCON_LENGTH_7				(0x2)			// 10 = 7-bits
#define ULCON_LENGTH_8				(0x3)			// 11 = 8-bits

//	(R/W)	UART CONTROL REGISTER
//	UCON0, UCON1, and UCON2

#define UCON_RESET					(0x0)			//	Reset Value 0x0

#define UCON_CLK					(0xC00)			// [11:10]	Clock Selection
#define UCON_CLK_PCLK				(0x0)			//*00, 10 = PCLK
#define UCON_CLK_UCLK				(0x400)			// 01 = UEXTCLK
#define UCON_CLK_FCLK				(0xC00)			// 11 = FCLK/n

#define UCON_TXINT					(0x200)			// [9]		Tx Interrupt request type
#define UCON_TXINT_PULSE			(0x0)			//*0 = Pulse
#define UCON_TXINT_LEVEL			(0x200)			// 1 = Level

#define UCON_RXINT					(0x100)			// [8]		Rx Interrupt request type
#define UCON_RXINT_PULSE			(0x0)			//*0 = Pulse
#define UCON_RXINT_LEVEL			(0x100)			// 1 = Level

#define UCON_RXTIME					(0x80)			// [7]		Rx Time Out
#define UCON_RXTIME_DISABLE			(0x0)			//*0 = Disable
#define UCON_RXTIME_ENABLE			(0x80)			// 1 = Enable

#define UCON_RXERR					(0x40)			// [6]		Rx Error Status Interrupt Enable
#define UCON_RXERR_DISABLE			(0x0)			//*0 = Do not generate receive error status interrupt
#define UCON_RXERR_ENABLE			(0x40)			// 1 = Generate receive error status interrupt

#define UCON_LOOP					(0x20)			// [5]		Loopback Mode
#define UCON_LOOP_NORM				(0x0)			//*0 = Normal operation
#define UCON_LOOP_LB				(0x20)			// 1 = Loopback mode

#define UCON_BREAK					(0x10)			// [4]		Send Break Signal
#define UCON_BREAK_NORM				(0x0)			//*0 = Normal transmit
#define UCON_BREAK_SEND				(0x10)			// 1 = Send break signal

#define UCON_TXMODE					(0xC)			// [3:2]	Transmit Mode
#define UCON_TXMODE_DISABLE			(0x0)			//*00 = Disable
#define UCON_TXMODE_INT				(0x4)			// 01 = Interrupt request or polling mode
#define UCON_TXMODE_DMA0			(0x8)			// 10 = DMA0 request (Only for UART0), DMA3 request (Only for UART2)
#define UCON_TXMODE_DMA1			(0xC)			// 11 = DMA1 request (Only for UART1)

#define UCON_RXMODE					(0x3)			// [1:0]	Receive Mode
#define UCON_RXMODE_DISABLE			(0x0)			//*00 = Disable
#define UCON_RXMODE_INT				(0x1)			// 01 = Interrupt request or polling mode
#define UCON_RXMODE_DMA0			(0x2)			// 10 = DMA0 request (Only for UART0), DMA3 request (Only for UART2)
#define UCON_RXMODE_DMA1			(0x3)			// 11 = DMA1 request (Only for UART1)

//	(R/W)	UART FIFO CONTROL REGISTER
//	UFCON0, UFCON1, and UFCON2

#define UFCON_RESET					(0x0)			//	Reset Value 0x0

#define UFCON_TXTRIG				(0xC0)			// [7:6]	Tx FIFO Trigger Level
#define UFCON_TXTRIG_EMPTY			(0x0)			//*00 = Empty
#define UFCON_TXTRIG_16				(0x40)			// 01 = 16-byte
#define UFCON_TXTRIG_32				(0x80)			// 10 = 32-byte
#define UFCON_TXTRIG_48				(0xC0)			// 11 = 48-byte

#define UFCON_RXTRIG				(0x30)			// [5:4]	Rx FIFO Trigger Level
#define UFCON_RXTRIG_1				(0x0)			//*00 = 1-byte
#define UFCON_RXTRIG_8				(0x10)			// 01 = 8-byte
#define UFCON_RXTRIG_16				(0x20)			// 10 = 16-byte
#define UFCON_RXTRIG_32				(0x30)			// 11 = 32-byte

#define UFCON_TXRESET				(0x4)			// [2]		Tx FIFO Reset
#define UFCON_TXRESET_NORM			(0x0)			//*0 = Normal
#define UFCON_TXRESET_RESET			(0x4)			// 1 = Tx FIFO Reset

#define UFCON_RXRESET				(0x2)			// [1]		Rx FIFO Reset
#define UFCON_RXRESET_NORM			(0x0)			//*0 = Normal
#define UFCON_RXRESET_RESET			(0x2)			// 1 = Rx FIFO Reset

#define UFCON_FIFO					(0x1)			// [0]		FIFO Enable
#define UFCON_FIFO_DISABLE			(0x0)			//*0 = Disable
#define UFCON_FIFO_ENABLE			(0x1)			// 1 = Enable

//	(R/W)		UART MODEM CONTROL REGISTER
//	UMCON0 and UMCON1
//	(UART 2 does not support AFC function, because the S3C2440A has no nRTS2 and nCTS2)

#define UMCON_RESET					(0x0)			//	Reset Value 0x0

//	(R)			UART TX/RX STATUS REGISTER
//	UTRSTAT0, UTRSTAT1 and UTRSTAT2

#define UTRSTAT_RESET				(0x6)			//	Reset Value 0x6

#define UTRSTAT_TX					(0x4)			// [2]		Transmitter empty
													// 0 = Not empty
													//*1 = Transmitter (transmit buffer & shifter register) empty
#define UTRSTAT_TX_BUF				(0x2)			// [1]		Transmit buffer empty
													// 0 = The buffer register is not empty
													//*1 = Empty
#define UTRSTAT_RX_BUF				(0x1)			// [0]		Receive buffer data ready
													//*0 = Empty
													// 1 = The buffer register is not empty

//	(R)		UART FIFO STATUS REGISTER
//	UFSTAT0, UFSTAT1, and UFSTAT2

#define UFSTAT_RESET				(0x0)			//	Reset Value 0x0

#define UFSTAT_TXFULL				(0x4000)		// [14]		Tx FIFO Full
#define UFSTAT_TXFULL_NORM			(0x0)			//*00 = 0-byte = Tx FIFO data = 63-byte
#define UFSTAT_TXFULL_FULL			(0x4000)		// 01 = Full

#define UFSTAT_TXCOUNT				(0x3F00)		// [13:8]	Number of data in Tx FIFO

#define UFSTAT_RXFULL 				(0x40)			// [6]		Rx FIFO Full
#define UFSTAT_RXFULL_NORM			(0x0)			//*00 = 0-byte = Rx FIFO data = 63-byte
#define UFSTAT_RXFULL_FULL			(0x40)			// 01 = Full

#define UFSTAT_RXCOUNT				(0x3F)			// [5:0]	Number of data in Rx FIFO

#endif /* _UART_ADDR_H_ */
