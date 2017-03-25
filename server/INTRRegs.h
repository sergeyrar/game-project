#ifndef INTRREGS_H_
#define INTRREGS_H_

#define SRCPND     ((volatile unsigned *)0x4a000000)	//Interrupt request status
#define INTMOD     ((volatile unsigned *)0x4a000004)	//Interrupt mode control
#define INTMSK     ((volatile unsigned *)0x4a000008)	//Interrupt mask control
#define PRIORITY   ((volatile unsigned *)0x4a00000c)	//IRQ priority control
#define INTPND     ((volatile unsigned *)0x4a000010)	//Interrupt request status
#define INTOFFSET  ((volatile unsigned *)0x4a000014)	//Interruot request source offset
#define SUBSRCPND  ((volatile unsigned *)0x4a000018)	//Sub source pending
#define INTSUBMSK  ((volatile unsigned *)0x4a00001c)	//Interrupt sub mask


#define BIT_TIMER0		(0x1<<10)
#define BIT_TIMER4		(0x1<<14)

#endif /*INTRREGS_H_*/
