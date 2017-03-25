#ifndef PWMREGS_H_
#define PWMREGS_H_

// PWM TIMER
#define TCFG0  ((volatile unsigned *)0x51000000)	//Timer 0 configuration
#define TCFG1  ((volatile unsigned *)0x51000004)	//Timer 1 configuration
#define TCON   ((volatile unsigned *)0x51000008)	//Timer control
#define TCNTB0 ((volatile unsigned *)0x5100000c)	//Timer count buffer 0
#define TCMPB0 ((volatile unsigned *)0x51000010)	//Timer compare buffer 0
#define TCNTO0 ((volatile unsigned *)0x51000014)	//Timer count observation 0
#define TCNTB1 ((volatile unsigned *)0x51000018)	//Timer count buffer 1
#define TCMPB1 ((volatile unsigned *)0x5100001c)	//Timer compare buffer 1
#define TCNTO1 ((volatile unsigned *)0x51000020)	//Timer count observation 1
#define TCNTB2 ((volatile unsigned *)0x51000024)	//Timer count buffer 2
#define TCMPB2 ((volatile unsigned *)0x51000028)	//Timer compare buffer 2
#define TCNTO2 ((volatile unsigned *)0x5100002c)	//Timer count observation 2
#define TCNTB3 ((volatile unsigned *)0x51000030)	//Timer count buffer 3
#define TCMPB3 ((volatile unsigned *)0x51000034)	//Timer compare buffer 3
#define TCNTO3 ((volatile unsigned *)0x51000038)	//Timer count observation 3
#define TCNTB4 ((volatile unsigned *)0x5100003c)	//Timer count buffer 4
#define TCNTO4 ((volatile unsigned *)0x51000040)	//Timer count observation 4


#define TIMER4_START		0x100000L
#define TIMER4_MAN_UPDATE	0x200000L
#define TIMER4_AUTO_RELOAD	0x400000L

#define TIMER4_MUX_1_2		0x00000L
#define TIMER4_MUX_1_4		0x10000L
#define TIMER4_MUX_1_8		0x20000L
#define TIMER4_MUX_1_16		0x30000L


#endif /*PWMREGS_H_*/
