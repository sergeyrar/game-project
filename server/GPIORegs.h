#ifndef GPIOREGS_H_
#define GPIOREGS_H_

// I/O PORT 
#define GPACON    ((volatile unsigned *)0x56000000)	//Port A control
#define GPADAT    ((volatile unsigned *)0x56000004)	//Port A data

#define GPBCON    ((volatile unsigned *)0x56000010)	//Port B control
#define GPBDAT    ((volatile unsigned *)0x56000014)	//Port B data
#define GPBUP     ((volatile unsigned *)0x56000018)	//Pull-up control B

#define GPCCON    ((volatile unsigned *)0x56000020)	//Port C control
#define GPCDAT    ((volatile unsigned *)0x56000024)	//Port C data
#define GPCUP     ((volatile unsigned *)0x56000028)	//Pull-up control C

#define GPDCON    ((volatile unsigned *)0x56000030)	//Port D control
#define GPDDAT    ((volatile unsigned *)0x56000034)	//Port D data
#define GPDUP     ((volatile unsigned *)0x56000038)	//Pull-up control D

#define GPECON    ((volatile unsigned *)0x56000040)	//Port E control
#define GPEDAT    ((volatile unsigned *)0x56000044)	//Port E data
#define GPEUP     ((volatile unsigned *)0x56000048)	//Pull-up control E

#define GPFCON    ((volatile unsigned *)0x56000050)	//Port F control
#define GPFDAT    ((volatile unsigned *)0x56000054)	//Port F data
#define GPFUP     ((volatile unsigned *)0x56000058)	//Pull-up control F

#define GPGCON    ((volatile unsigned *)0x56000060)	//Port G control
#define GPGDAT    ((volatile unsigned *)0x56000064)	//Port G data
#define GPGUP     ((volatile unsigned *)0x56000068)	//Pull-up control G

#define GPHCON    ((volatile unsigned *)0x56000070)	//Port H control
#define GPHDAT    ((volatile unsigned *)0x56000074)	//Port H data
#define GPHUP     ((volatile unsigned *)0x56000078)	//Pull-up control H

#define GPJCON    ((volatile unsigned *)0x560000d0)	//Port J control
#define GPJDAT    ((volatile unsigned *)0x560000d4)	//Port J data
#define GPJUP     ((volatile unsigned *)0x560000d8)	//Pull-up control J

#define MISCCR    ((volatile unsigned *)0x56000080)	//Miscellaneous control
#define DCLKCON   ((volatile unsigned *)0x56000084)	//DCLK0/1 control
#define EXTINT0   ((volatile unsigned *)0x56000088)	//External interrupt control register 0
#define EXTINT1   ((volatile unsigned *)0x5600008c)	//External interrupt control register 1
#define EXTINT2   ((volatile unsigned *)0x56000090)	//External interrupt control register 2
#define EINTFLT0  ((volatile unsigned *)0x56000094)	//Reserved
#define EINTFLT1  ((volatile unsigned *)0x56000098)	//Reserved
#define EINTFLT2  ((volatile unsigned *)0x5600009c)	//External interrupt filter control register 2
#define EINTFLT3  ((volatile unsigned *)0x560000a0)	//External interrupt filter control register 3
#define EINTMASK  ((volatile unsigned *)0x560000a4)	//External interrupt mask
#define EINTPEND  ((volatile unsigned *)0x560000a8)	//External interrupt pending
#define GSTATUS0  ((volatile unsigned *)0x560000ac)	//External pin status
#define GSTATUS1  ((volatile unsigned *)0x560000b0)	//Chip ID(0x32440000)
#define GSTATUS2  ((volatile unsigned *)0x560000b4)	//Reset type
#define GSTATUS3  ((volatile unsigned *)0x560000b8)	//Saved data0(32-bit) before entering POWER_OFF mode 
#define GSTATUS4  ((volatile unsigned *)0x560000bc)	//Saved data0(32-bit) before entering POWER_OFF mode 



#endif /*GPIOREGS_H_*/
