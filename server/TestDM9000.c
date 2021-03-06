#include "DM9000Regs.h"
#include "Utils.h"
#include "types.h"

void TestReadDM9000_ID ()
{
	U32	u32Tmp;
	U32	u32DM9000_ID;
	
	Util_Printf ("Implementing DM9000 test\n");

	
	*DM9000_INDEX_REGISTER = DM900_VENID_OFFSET_BYTE0;
	u32DM9000_ID = u32Tmp = *DM9000_DATA_REGISTER;
	*DM9000_INDEX_REGISTER = DM900_VENID_OFFSET_BYTE1;
	u32Tmp = *DM9000_DATA_REGISTER;
	u32DM9000_ID	|= (u32Tmp << 8);
	*DM9000_INDEX_REGISTER = DM900_VENID_OFFSET_BYTE2;
	u32Tmp = *DM9000_DATA_REGISTER;
	u32DM9000_ID	|= (u32Tmp << 16);
	*DM9000_INDEX_REGISTER = DM900_VENID_OFFSET_BYTE3;
	u32Tmp = *DM9000_DATA_REGISTER;
	u32DM9000_ID	|= (u32Tmp << 24);
	
	
	Util_Printf(" DM9000 ID is 0x%x\n",u32DM9000_ID);	
	
}
