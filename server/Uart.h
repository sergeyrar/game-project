/*
 ============================================================================
 Name        : Uart.h
 Author      : Alexander Vovk
 Version     :
 Created on  : Oct 23, 2011
 Copyright   : RT-ED
 Description :
 ============================================================================
 ============================================================================
 */
#include <stdio.h>

#ifndef UART_H_
#define UART_H_

#include "_Uart.h"

void Uart_Printf(char *, ...);
void Uart_Printf_(U32, char *, ...);
void Uart_SendByte(char);
void Uart_SendString(char *);
char Uart_Getch(void);
char Uart_GetKey(void);
void Uart_GetString(char *);
int Uart_GetIntNum(void);


#endif /* UART_H_ */
