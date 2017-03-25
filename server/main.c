#include <stdio.h>
#include "types.h"

#include "Utils.h"
#include "tests.h"

#define	ESC_KEY		0x1b



int main ()
{
	Util_Printf ("RT-ED test platform\n");
	MMU_Table_Init_MMU ();	
	start_eth_device();
	
	while( Uart_Getkey() != ESC_KEY ) 
 	{
		receive_packet();
		
	}
}
