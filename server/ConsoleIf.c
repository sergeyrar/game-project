/*
 ============================================================================
 Name        : ConsoleIf.c
 Author      : Alexander Vovk & Yoav Segal
 Version     :
 Created on  : Oct 23, 2011
 Copyright   : RT-ED
 Description :
 ============================================================================
 ============================================================================
 */

#include "Uart.h"
#include <stdarg.h>
#include <string.h>

static U32 u32WhichUart = 0;


/*
 * ==============================================================================================================
 */
void Uart_Printf(char *msg, ...)
{
	char string[256];
	va_list argp;
	va_start(argp, msg);
	vsprintf(string, msg, argp);
	Uart_SendString(string);
	va_end(argp);
}

/*
 * ==============================================================================================================
 */
void Uart_SendByte(char data)
{
	_Uart_SendChar(u32WhichUart, data);
	if (data == '\n')
		_Uart_SendChar(u32WhichUart, '\r');
}

/*
 * ==============================================================================================================
 */
void Uart_SendString(char *sString)
{
	while(*sString)
	{
		Uart_SendByte(*(sString++));
	}
}

/*
 * ==============================================================================================================
 */
char Uart_Getch(void)
{
	return _Uart_Getch(u32WhichUart);
}

/*
 * ==============================================================================================================
 */
char Uart_Getkey(void)
{
	return _Uart_Getkey(u32WhichUart);
}


/*
 * ==============================================================================================================
 */
void Uart_GetString(char *string)
{
    char *string2 = string;
    char c;
    while((c = Uart_Getch())!='\r')
    {
        if(c=='\b')
        {
            if( (int)string2 < (int)string )
            {
                Uart_Printf("\b \b");
                string--;
            }
        }
        else
        {
            *string++ = c;
            Uart_SendByte(c);
        }
    }
    *string='\0';
    Uart_SendByte('\n');
}

/*
 * ==============================================================================================================
 */
int Uart_GetIntNum(void)
{
    char str[30];
    char *string = str;
    int base     = 10;
    int minus    = 0;
    int result   = 0;
    int lastIndex;
    int i;

    Uart_GetString(string);

    if(string[0] == '-')
    {
        minus = 1;
        string++;
    }

    if(string[0] == '0' && (string[1] == 'x' || string[1] == 'X'))
    {
        base    = 16;
        string += 2;
    }

    lastIndex = strlen(string) - 1;

    if(lastIndex < 0)
        return -1;

    if(string[lastIndex] == 'h' || string[lastIndex] == 'H' )
    {
        base = 16;
        string[lastIndex] = 0;
        lastIndex--;
    }

    if(base == 10)
    {
        result = atoi(string);
        result = minus ? (-1 * result) : result;
    }
    else
    {
        for(i=0; i <= lastIndex; i++)
        {
            if ((string[i] > 47) && (string[i] < 58))
                result = (result << 4) + (string[i] - '0');
			else if ((string[i] > 64) && (string[i] < 71))
                result = (result << 4) + (string[i] - 'A' + 10);
			else if ((string[i] > 96) && (string[i] < 103))
                result = (result << 4) + (string[i] - 'a' + 10);
            else
            	return -1;
        }
        result = minus ? (-1 * result) : result;
    }
    return result;
}



