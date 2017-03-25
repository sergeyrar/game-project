#include "_Uart.h"
#include <stdarg.h>

extern void HaltFunc();

#define W_REG(reg, data) (*reg) = (data)
#define R_REG(reg) (*reg)


/*
 * UART Initialization
 */
void _Uart_Init(U32 u32Pclk, U32 u32baud, U32 u32WhichTxUART)
{
	U32 u32Offset = UOFFSET * (u32WhichTxUART);
	U32 u32Div = 0;
	
    /*
        FIFO control register, FIFO disable
    */
    

	*(UFCON0 + u32Offset) = UFCON_RESET;

    /*
        MODEM control register, AFC disable
    */
	*(UMCON0 + u32Offset) = UMCON_RESET;

    /*
        Line control register : Normal,No parity,1 stop,8 bits
    */
	*(ULCON0 + u32Offset) = (ULCON_MODE_NORM | ULCON_PARITY_NONE | ULCON_STOP_ONE | ULCON_LENGTH_8);

    /*
        Uart Control register
    */
	*(UCON0 + u32Offset) = (UCON_CLK_PCLK | UCON_TXINT_LEVEL | UCON_RXERR_ENABLE | UCON_TXMODE_INT | UCON_RXMODE_INT);

	switch (u32baud)
	{
		case 115200:
			switch (u32Pclk)
			{
				case 50000000:
					u32Div = 26;
					break;
				default:
					while(1);
			}
			break;
		default:
			HaltFunc ();		
			while (1);
	}

    /*
        Baud rate divisior register
    */
	*(UBRDIV0 + u32Offset) = u32Div;

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	
	
/*
	_Uart_SendString(u32WhichTxUART, "\r\n");
	_Uart_SendString(u32WhichTxUART, "Initialization state..\r\n");
	_Uart_SendString(u32WhichTxUART, "FriendlyARM - Mini2440\r\n");
	_Uart_SendString(u32WhichTxUART, "Implementation by RT-ED\r\n");
*/		
}

/*
 * ==============================================================================================================
 */

void _Uart_SendChar(U32 u32WhichTxUART, char cChar)
{
	U32 u32Offset = UOFFSET * (u32WhichTxUART);
	switch((*(UFCON0 + u32Offset)) & UFCON_FIFO)
	{
		case UFCON_FIFO_ENABLE:
			while((*(UFSTAT0 + u32Offset)) & UFSTAT_TXFULL);
			break;
		case UFCON_FIFO_DISABLE:
			while(!((*(UTRSTAT0 + u32Offset)) & UTRSTAT_TX));   //Wait until THR is empty.
			break;
		default:
			break;

	}

	W_REG((UTXH0 + u32Offset), cChar);
}

/*
 * ==============================================================================================================
 */
void _Uart_SendString(U32 u32WhichTxUART, char *sString)
{
	while(*sString)
	{
		_Uart_SendChar(u32WhichTxUART, *(sString++));
	}
}



/*
 * ==============================================================================================================
 */
char _Uart_Getch(unsigned long u32WhichUart)
{
	U32 u32Offset = UOFFSET * (u32WhichUart);
	switch((*(UFCON0 + u32Offset)) & UFCON_FIFO)
	{
		case UFCON_FIFO_ENABLE:
			while(!((*(UFSTAT0 + u32Offset)) & UFSTAT_RXCOUNT));
			break;
		case UFCON_FIFO_DISABLE:
			while(!((*(UTRSTAT0 + u32Offset)) & UTRSTAT_RX_BUF));   //Wait until THR is empty.
			break;
		default:
			break;
	}
	return (char)(*(URXH0 + u32Offset));
}

char _Uart_Getkey(unsigned long u32WhichUart)
{
	U32 u32Offset = UOFFSET * (u32WhichUart);
	switch((*(UFCON0 + u32Offset)) & UFCON_FIFO)
	{
		case UFCON_FIFO_ENABLE:
			if (((*(UFSTAT0 + u32Offset)) & UFSTAT_RXCOUNT))
				return (char)(*(URXH0 + u32Offset));
			break;
		case UFCON_FIFO_DISABLE:
			if (((*(UTRSTAT0 + u32Offset)) & UTRSTAT_RX_BUF))
				return (char)(*(URXH0 + u32Offset));			
			break;
		default:
			break;
	}
	return 0;
}



