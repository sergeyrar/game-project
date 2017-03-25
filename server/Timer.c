#include "PWMRegs.h"
#include "GPIORegs.h"
#include "INTRRegs.h"
#include "INTRDefs.h"
#include "types.h"
#include "Utils.h"

/*
 * PWM Timer is residing on the APB bus. 
 * For the time being - we assume that the clock used to drive this bus has 
 * 50MHZ frequency
*/
  
static U32 PCLK = 50000000;

/*
 * Definitions for delay test.
*/
void Timer_Bdelay_Milli (U32 Millis);
void Timer_Bdelay_Micro (U32 Micros);
void BuzzerSet (U32 StepFreq, U32 StepsInTooth, U32 DutyCycle);
static U32 g_Refs = 0;
/*
 * Definitions for timer ISR test.
*/
static void TimerISR (void) __attribute__ ((interrupt ("IRQ")));
static void TimerIsrInit();

volatile U8  ch = 0;
volatile U32 g_InterruptCount = 0;

void BuzzerSet (U32 StepFreq, U32 StepsInTooth, U32 DutyCycle)
{
	U32 Fraction;
	U32 CFGVal0 = 0;
	U32 CFGVal1 = 0;	
	
	// Set TOUT0
	*GPBCON &= ~3;			//set GPB0 as tout0, pwm output
	*GPBCON |= 2;
	
	Fraction = PCLK/StepFreq;
	*TCFG1	&= ~0xf;
	*TCFG0 &= ~0xff;	
	
	do
	{
		if ((Fraction>>1) < 256)
		{
			Util_Printf ("Option1\n"); 
			*TCFG0 = CFGVal0 = ((Fraction>>1) - 1); 			
			break;
		}
		if ((Fraction>>2) < 256)
		{
			Util_Printf ("Option2\n");			
 			*TCFG1 |= 1;			
			*TCFG0 = CFGVal0 = ((Fraction>>2) - 1); 			
			break;
		} 		
		if ((Fraction>>3) < 256)
		{
			Util_Printf ("Option3\n");			
 			*TCFG1 |= 2;
			*TCFG0 = CFGVal0 = ((Fraction>>3) - 1); 			
			break;
		}
		if ((Fraction>>4) < 256)
		{
			Util_Printf ("Option4\n");			
 			*TCFG1 |= 3;		
			*TCFG0 = CFGVal0 = ((Fraction>>4) - 1); 				
			break;
		}
		Util_Printf ("Invalid frequency value - use 25MHZ   \n");
		

 		
	} while (0);
	
	//Util_Printf ("Buzzer:%d %d %d\n", CFGVal0, StepsInTooth, (StepsInTooth * DutyCycle)/100);   
	
	*TCNTB0 = (short)StepsInTooth; /* Must not exceed 16 bit */
	*TCMPB0 = (StepsInTooth * DutyCycle)/100;
	
	
	*TCON |= 0xb;			//disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 0
	*TCON &= ~2;			//clear manual update bit
	 	 
	
	Util_Getch ();
	
	// Now stop it.
	
 	*TCON &= ~(1<<3); //disable auto reload for timer 0
 	*TCON &= ~(1<<0); //Timer 0 is stoped

	*GPBCON &= ~3;			//set GPB0 as output
	*GPBCON |= 1;
	*GPBDAT &= ~1;
}


/*======================
 * Delay tests
 * ====================*/

void Timer_Bdelay_Micro (U32 Micros)
{
	U32 Prev;
	U32 Current;
	U32 ElapsedSteps = 0;
	U32 StepsInSwar = 0xFFFF;
	
	if (g_Refs++ == 0) /* Check reentrancy */
	{ 
		
		/*
		 * Set timer step period T for Timer 4 to 1 microsecond - Divide PCLK by 50
		*/
		*TCFG0 &= ~(0xff<<8);
		*TCFG0 |= 24<<8;			    //prescaler = 24+1
		*TCFG1 &= ~(0xf<<16);
		*TCFG1 |= TIMER4_MUX_1_2;		//mux = 1/2
		
		/*
		 * Tooth period cT is set by multiplying T by the number of countown steps.
		*/	
		*TCNTB4 = StepsInSwar;
		
		/*
		 * Activate timer 4.
		*/	
		
		*TCON |= (TIMER4_START | TIMER4_MAN_UPDATE | TIMER4_AUTO_RELOAD);	//autoload, update TCNTB4, start timer 4
		*TCON &= ~(TIMER4_MAN_UPDATE);		//clear manual update bit
	}
	
	Prev = *TCNTO4;
	
	while (ElapsedSteps < Micros)
	{
		Current = *TCNTO4;
		if (Current <= Prev)
		{
			ElapsedSteps += (Prev - Current);
		}
		else
		{
			ElapsedSteps += (StepsInSwar - (Current - Prev));
		}
		
		Prev = Current;
	}
	
	if (--g_Refs == 0)
	{
		/* Stop the timer since it is no more referenced */	
		*TCON &= ~(TIMER4_START | TIMER4_AUTO_RELOAD);
	}
}

void Timer_Bdelay_Milli (U32 Millis)
{
	U32 Prev;
	U32 Current;
	U32 ElapsedSteps = 0;
	U32 StepsInOneMilli = (PCLK>>3)/1000;
	U32 TotalSteps = StepsInOneMilli * Millis;
	

	/*
	 * Set timer step period T for Timer 4 - Divide PCLK by 8
	*/	
	*TCFG0 &= ~(0xff<<8);
	*TCFG0 |= 3<<8;			        //prescaler = 3+1
	*TCFG1 &= ~(0xf<<16);
	*TCFG1 |= TIMER4_MUX_1_2;		//mux = 1/2

	/*
	 * Tooth period cT is set by multiplying T by the number of countown steps.
	*/	
	*TCNTB4 = StepsInOneMilli;
	

	/*
	 * Activate timer 4.
	*/	
	
	*TCON |= (TIMER4_START | TIMER4_MAN_UPDATE | TIMER4_AUTO_RELOAD);	//autoload, update TCNTB4, start timer 4
	*TCON &= ~(TIMER4_MAN_UPDATE);		//clear manual update bit
	
	Prev = *TCNTO4;

	while (ElapsedSteps < TotalSteps)
	{
		Current = *TCNTO4;
		if (Current <= Prev)
		{
			ElapsedSteps += (Prev - Current);
		}
		else
		{
			ElapsedSteps += (StepsInOneMilli - (Current - Prev));
		}
		
		Prev = Current;
	}

	/* Stop the timer */	
	*TCON &= ~(TIMER4_START | TIMER4_AUTO_RELOAD);	
}

void TestPWMBuzzer ()
{
	U32 StepFreq;
	U32 StepsInTooth;
	U32 DutyCycle;	 
	
	Util_Printf ("Enter the input (step) frequency (decimal)...");	
	StepFreq = Util_GetIntNum ();
	Util_Printf ("Enter steps in tooth (decimal)...");	
	StepsInTooth = Util_GetIntNum ();
	Util_Printf ("Enter the duty cycle percentage (decimal)...");	
	DutyCycle = Util_GetIntNum ();
	
	BuzzerSet (StepFreq, StepsInTooth, DutyCycle);	
}



void TestBdelayMilli ()
{
	U32 Val;
	
	Util_Printf ("Enter the delay in milliseconds (decimal)...");	
	Val = Util_GetIntNum ();
	Timer_Bdelay_Milli (Val);	
}

void TestBdelayMicro ()
{
	U32 Val;
	
	Util_Printf ("Enter the delay in microseconds (decimal)...");	
	Val = Util_GetIntNum ();
	Timer_Bdelay_Micro (Val);	
}

/*======================
 * ISR test
 * ====================*/


void TestTimerIsr()

{
	TimerIsrInit();	

    Util_Printf("\nPress s to stop the timer:\n");
    while (1)
    {
    	ch = Util_Getch();
    	if (ch == 's')
    	{
    		break;
    	}
    }

    while (*TCNTO4 != 0);

	Util_Printf("The interrupt counter final value is %d\n", g_InterruptCount);
	g_InterruptCount = 0;    
}



static void TimerIsrInit()
	// This function initilizes only timer 0. Don't mess with other timers
	// Unless you know for sure that they are not dedicated to other operations

{
    ch = 0;

	/* Disable any sperious interrupts from timer 4 */
	*INTMSK	 |= (BIT_TIMER4);
	
	/* Place interrupt routine in interrupt vestor */
	pISR_TIMER4=(U32)TimerISR;
	

	/*
	 * Set timer step period T for Timer 4
	*/	
	*TCFG0 &= ~(0xFF<<8);
	*TCFG0 |= (0xFF<<8);//prescalers values for timers 2,3, 4;
	*TCFG1 &= ~(0xF<<16);
	*TCFG1 |= TIMER4_MUX_1_4;		//mux = 1/4
	
	
	//initializing timer 4 to start counter value
	*TCNTB4=0xBEBC; /* 50000000/(256*4) */
	
	
	*TCON |= (TIMER4_START | TIMER4_MAN_UPDATE | TIMER4_AUTO_RELOAD);	//autoload, update TCNTB4, start timer 4
	*TCON &= ~(TIMER4_MAN_UPDATE);		//clear manual update bit
	 
	/*
		Enable interrupts only here - Enable only the necessary interrupt bit.
	*/
	*INTMSK	 &= ~(BIT_TIMER4);
}
	
static void TimerISR(void)
{
	*SRCPND = BIT_TIMER4;
	*INTPND = BIT_TIMER4;
	 
	 g_InterruptCount ++;
	 
	 if (ch=='s')
		{
	 	*TCON&=~(TIMER4_AUTO_RELOAD); //disable auto reload for timer 4
	 	*TCON&=~(TIMER4_START); //Timer 4 is stoped
	 	ch = 0;
	 	}

}
