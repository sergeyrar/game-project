#ifndef _DM9000REGS_H_
#define _DM9000REGS_H_

#define DM9000_INDEX_REGISTER 		(volatile char *)0x20000300
#define DM9000_DATA_REGISTER 		(volatile char *)0x20000304


/*
 * Offsets within DM9000 block  
 * 
*/
#define DM900_VENID_OFFSET_BYTE0	0x28
#define DM900_VENID_OFFSET_BYTE1	0x29				
#define DM900_VENID_OFFSET_BYTE2	0x2a
#define DM900_VENID_OFFSET_BYTE3	0x2b


#endif
