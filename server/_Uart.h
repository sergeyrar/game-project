/*
 ============================================================================
 Name        : _Uart.h
 Author      : Alexander Vovk
 Version     :
 Created on  : Oct 8, 2011
 Copyright   : RT-ED
 Description :
 ============================================================================
 ============================================================================
 */

#ifndef _UART_H_
#define _UART_H_

#include <stdio.h>
#include "types.h"
#include "_Uart_Addr.h"


void _Uart_Init(U32 u32Pclk, U32 u32baud, U32 u32WhichTxUART);
void _Uart_SendChar(U32 u32WhichTxUART, char cChar);
void _Uart_SendString(U32 u32WhichTxUART, char *sString);
char _Uart_Getch(unsigned long u32WhichUart);
char _Uart_GetKey(unsigned long u32WhichUart);

#endif /* _UART_H_ */
