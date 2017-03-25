#include "GPIORegs.h"
#include "types.h"
#include "Utils.h"


void TestGPIO ()
{
	U32 Mask;
	U32 ConRegVal;
	U32 ValToPulUp;
	U32 ValToData;		
	U32 i;
	/*
	 * TBD - get the mask value.
	*/
	Util_Printf ("Enter the Mask (prefixed by 0x for hex values...");	
	Mask = Util_GetIntNum ();
	
	/*
	 * The mask consists of 4 relevan bits 0-3.
	 * BIT0 - LED1...
	 * BIT3 - LED4
	 * Let's look at page 9-11 in the data sheet:
	 * We should use bits 10-17 in GPGCon reg.
	 * We should use use bits 5-8 in GPGPullUp reg and in GPGData register.
	*/
	if ((ConRegVal = (*GPBCON & 0x3FC00 )) != 0x15400)
	{
		*GPBCON &= ~0x3FC00; /* Isolate bits 10-17 */
		*GPBCON |= 0x15400;
		*GPBUP &= ~0x1E0; /* Isolate bits 5-8 */
		*GPBUP |= 0x1E0;				
	} 	
	
	
	for (i = 0; i < 4; i ++)
	{
		if (Mask & 0x1)
		{
			*GPBDAT &= ~(0x20L << i); /* Turn it on */			
		}
		else
		{
			*GPBDAT |= (0x20L << i); /* Turn it off */			
		}
		Mask >>= 1;
	}
}
