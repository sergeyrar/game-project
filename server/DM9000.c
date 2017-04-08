#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "Utils.h"


#include "net.h"
#include "DM9000.h"




#define DM9000_DBG	Uart_Printf

#define DM9000_outb(d,r) 	( *(volatile u8 *)r = d )
#define DM9000_outw(d,r) 	( *(volatile u16 *)r = d )
#define DM9000_outl(d,r) 	( *(volatile u32 *)r = d )
#define DM9000_inb(r) 		(*(volatile u8 *)r)
#define DM9000_inw(r) 		(*(volatile u16 *)r)
#define DM9000_inl(r) 		(*(volatile u32 *)r)


static void dm9000_outblk_8bit(volatile void *data_ptr, int count)
{
	int i;
	for (i = 0; i < count; i++)
		DM9000_outb((((u8 *) data_ptr)[i] & 0xff), DM9000_DATA);
}

static void dm9000_outblk_16bit(volatile void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		DM9000_outw(((u16 *) data_ptr)[i], DM9000_DATA);
}
static void dm9000_outblk_32bit(volatile void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 3) / 4;

	for (i = 0; i < tmplen; i++)
		DM9000_outl(((u32 *) data_ptr)[i], DM9000_DATA);
}

static void dm9000_inblk_8bit(void *data_ptr, int count)
{
	int i;
	for (i = 0; i < count; i++)
		((u8 *) data_ptr)[i] = DM9000_inb(DM9000_DATA);
}

static void dm9000_inblk_16bit(void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		((u16 *) data_ptr)[i] = DM9000_inw(DM9000_DATA);
}
static void dm9000_inblk_32bit(void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 3) / 4;

	for (i = 0; i < tmplen; i++)
		((u32 *) data_ptr)[i] = DM9000_inl(DM9000_DATA);
}


/*
   Read a byte from I/O port
*/
static u8
DM9000_ior(int reg)
{
	u8 u8Tmp;
	DM9000_outb(reg, DM9000_IO);
	u8Tmp = DM9000_inb(DM9000_DATA);
//	Uart_Printf ("DM9000_ior index=0x%x, data=0x%x\n", reg, u8Tmp);
	return u8Tmp;
}

/*
   Write a byte to I/O port
*/
static void
DM9000_iow(int reg, u8 value)
{
	DM9000_outb(reg, DM9000_IO);
	DM9000_outb(value, DM9000_DATA);
//	Uart_Printf ("DM9000_ior index=0x%x, data=0x%x\n", reg, value);	
}


/*
   Write a word to phyxcer
*/
static void
phy_write(int reg, u16 value)
{

	/* Fill the phyxcer register into REG_0C */
	DM9000_iow(DM9000_EPAR, DM9000_PHY | reg);

	/* Fill the written data into REG_0D & REG_0E */
	DM9000_iow(DM9000_EPDRL, (value & 0xff));
	DM9000_iow(DM9000_EPDRH, ((value >> 8) & 0xff));
	DM9000_iow(DM9000_EPCR, 0xa);	/* Issue phyxcer write command */
	Timer_Bdelay_Milli(1);			/* Wait write complete */
	DM9000_iow(DM9000_EPCR, 0x0);	/* Clear phyxcer write command */
	DM9000_DBG("phy_write(reg:0x%x, value:0x%x)\n", reg, value);
}

/*
  Search DM9000 board, allocate space and register it
*/
int
eth_probe(void)
{
	u32 id_val;
	id_val = DM9000_ior(DM9000_VIDL);
	id_val |= DM9000_ior(DM9000_VIDH) << 8;
	id_val |= DM9000_ior(DM9000_PIDL) << 16;
	id_val |= DM9000_ior(DM9000_PIDH) << 24;
	if (id_val == DM9000_ID) {
		Uart_Printf("dm9000 i/o: 0x%x, id: 0x%x \n", CONFIG_DM9000_BASE,
		       id_val);
		return 0;
	} else {
		Uart_Printf("dm9000 not found at 0x%08x id: 0x%08x\n",
		       CONFIG_DM9000_BASE, id_val);
		return -1;
	}
}

 



static void
eth_reset(void)
{
	DM9000_DBG("resetting DM9000\n");

	/* Reset DM9000,
	   see DM9000 Application Notes V1.22 Jun 11, 2004 page 29 */

	/* DEBUG: Make all GPIO0 outputs, all others inputs */
	DM9000_iow(DM9000_GPCR, GPCR_GPIO0_OUT);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	DM9000_iow(DM9000_GPR, 0);
	/* Step 2: Software reset */
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));

	do {
		DM9000_DBG("resetting the DM9000, 1st reset\n");
		Timer_Bdelay_Milli(1); /* Wait at least 1 ms */
	} while (DM9000_ior(DM9000_NCR) & 1);

	DM9000_iow(DM9000_NCR, 0);
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST)); /* Issue a second reset */

	do {
		DM9000_DBG("resetting the DM9000, 2nd reset\n");
		Timer_Bdelay_Milli(1); /* Wait at least 1 ms */
	} while (DM9000_ior(DM9000_NCR) & 1);

	/* Check whether the ethernet controller is present */
	if ((DM9000_ior(DM9000_PIDL) != 0x0) ||
	    (DM9000_ior(DM9000_PIDH) != 0x90))
		Uart_Printf("ERROR: resetting DM9000 -> not responding\n");
}



static void dm9000_rx_status_32bit(u16 *RxStatus, u16 *RxLen)
{
	u32 tmpdata;

	DM9000_outb(DM9000_MRCMD, DM9000_IO);

	tmpdata = DM9000_inl(DM9000_DATA);
	*RxStatus = __le16_to_cpu(tmpdata);
	*RxLen = __le16_to_cpu(tmpdata >> 16);
}

static void dm9000_rx_status_16bit(u16 *RxStatus, u16 *RxLen)
{
	DM9000_outb(DM9000_MRCMD, DM9000_IO);

	*RxStatus = __le16_to_cpu(DM9000_inw(DM9000_DATA));
	*RxLen = __le16_to_cpu(DM9000_inw(DM9000_DATA));
}

static void dm9000_rx_status_8bit(u16 *RxStatus, u16 *RxLen)
{
	DM9000_outb(DM9000_MRCMD, DM9000_IO);

	*RxStatus =
	    __le16_to_cpu(DM9000_inb(DM9000_DATA) +
			  (DM9000_inb(DM9000_DATA) << 8));
	*RxLen =
	    __le16_to_cpu(DM9000_inb(DM9000_DATA) +
			  (DM9000_inb(DM9000_DATA) << 8));
}

u16
read_srom_word(int offset)
{
	DM9000_iow(DM9000_EPAR, offset);
	DM9000_iow(DM9000_EPCR, 0x4);
	Timer_Bdelay_Milli(8);
	DM9000_iow(DM9000_EPCR, 0x0);
	return (DM9000_ior(DM9000_EPDRL) + (DM9000_ior(DM9000_EPDRH) << 8));
}


/* Initilize dm9000 MAC, without reset
*/
int
eth_set_mac(bd_t * bd)
{
	int i,oft;
	Uart_Printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n", bd->bi_enetaddr[0],
	       bd->bi_enetaddr[1], bd->bi_enetaddr[2], bd->bi_enetaddr[3],
	       bd->bi_enetaddr[4], bd->bi_enetaddr[5]);

	/* fill device MAC address registers */
	for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
		DM9000_iow(oft, bd->bi_enetaddr[i]);
	for (i = 0, oft = 0x16; i < 8; i++, oft++)
		DM9000_iow(oft, 0xff);

	/* read back mac, just to be sure */
	for (i = 0, oft = 0x10; i < 6; i++, oft++)
		DM9000_DBG("%02x:", DM9000_ior(oft));
	DM9000_DBG("\n");
	return 0;
}


/*
  Stop the interface.
  The interface is stopped when it is brought.
*/
void
eth_halt(void)
{
	DM9000_DBG("eth_halt\n");

	/* RESET devie */
	phy_write(0, 0x8000);	/* PHY RESET */
	DM9000_iow(DM9000_GPR, 0x01);	/* Power-Down PHY */
	DM9000_iow(DM9000_IMR, 0x80);	/* Disable all interrupt */
	DM9000_iow(DM9000_RCR, 0x00);	/* Disable RX */
}

int
eth_init(bd_t * bd)
{
	int i, oft, lnk;
	u8 io_mode;
	struct board_info *db = &dm9000_info;

	DM9000_DBG("eth_init()\n");

	/* RESET device */
	eth_reset();

	if (eth_probe() < 0)
		return -1;
		
	io_mode = DM9000_ior(DM9000_ISR) >> 6;
	
	Uart_Printf ("eth_init: io_mode = 0x%x\n", io_mode);	
	
	switch (io_mode) {
	case 0x0:  /* 16-bit mode */
		Uart_Printf("DM9000: running in 16 bit mode\n");
		db->outblk    = dm9000_outblk_16bit;
		db->inblk     = dm9000_inblk_16bit;
		db->rx_status = dm9000_rx_status_16bit;
		break;
	case 0x01:  /* 32-bit mode */
		Uart_Printf("DM9000: running in 32 bit mode\n");
		db->outblk    = dm9000_outblk_32bit;
		db->inblk     = dm9000_inblk_32bit;
		db->rx_status = dm9000_rx_status_32bit;
		break;
	case 0x02: /* 8 bit mode */
		Uart_Printf("DM9000: running in 8 bit mode\n");
		db->outblk    = dm9000_outblk_8bit;
		db->inblk     = dm9000_inblk_8bit;
		db->rx_status = dm9000_rx_status_8bit;
		break;
	default:
		/* Assume 8 bit mode, will probably not work anyway */
		Uart_Printf("DM9000: Undefined IO-mode:0x%x\n", io_mode);
		db->outblk    = dm9000_outblk_8bit;
		db->inblk     = dm9000_inblk_8bit;
		db->rx_status = dm9000_rx_status_8bit;
		break;
	}
	
	
		/* Program operating register, only internal phy supported */
	DM9000_iow(DM9000_NCR, 0x0);
	/* TX Polling clear */
	DM9000_iow(DM9000_TCR, 0);
	/* Less 3Kb, 200us */
	DM9000_iow(DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);
	/* Flow Control : High/Low Water */
	DM9000_iow(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));
	/* SH FIXME: This looks strange! Flow Control */
	DM9000_iow(DM9000_FCR, 0x0);
	/* Special Mode */
	DM9000_iow(DM9000_SMCR, 0);
	/* clear TX status */
	DM9000_iow(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	/* Clear interrupt status */
	DM9000_iow(DM9000_ISR, ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS);

	for (i = 0; i < 6; i++)
		((u16 *) bd->bi_enetaddr)[i] = read_srom_word(i);
		
	if (is_zero_ether_addr(bd->bi_enetaddr) ||
	    is_multicast_ether_addr(bd->bi_enetaddr)) {
		/* try reading from environment */
		u8 i;
		char *s, *e;
		s = "00:aa:bb:aa:bb:aa";
		for (i = 0; i < 6; ++i) {
			bd->bi_enetaddr[i] = s ?
				simple_strtoul (s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}
		Uart_Printf("DM9000: Could not read MAC address from srom. Adjusted by definition\n");
	}
		
	
	
	eth_set_mac(bd);

	/* Activate DM9000 */
	/* RX enable */
	DM9000_iow(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	DM9000_iow(DM9000_IMR, IMR_PAR);

	// PHY read status doesn't work anyway
	return 0;
}


/*
  Received a packet and pass to upper layer
*/
int
eth_rx(unsigned char *player_id, unsigned char* action, unsigned char *station_id)
{

	u8 rxbyte, *rdptr = (u8 *) NetRxPackets[0];
	u16 RxStatus, RxLen = 0;
//	int iStatus = 0;

	struct board_info *db = &dm9000_info;

	//DM9000_DBG("eth_rx\n");	


	/* Check packet ready or not, we must check
	   the ISR status first for DM9000A */
	if (!(DM9000_ior(DM9000_ISR) & 0x01)) /* Rx-ISR bit must be set. */
	{
		//DM9000_DBG("ISR status is not set\n");		
		return 0;
	}	

	DM9000_iow(DM9000_ISR, 0x01); /* clear PR status latched in bit 0 - SHOULD BE 0x00 ?????*/

	/* There is _at least_ 1 package in the fifo, read them all */
	for (;;) {
		DM9000_ior(DM9000_MRCMDX);	/* Dummy read */

		/* Get most updated data,
		   only look at bits 0:1, See application notes DM9000 */
		rxbyte = DM9000_inb(DM9000_DATA) & 0x03;

		/* Status check: this byte must be 0 or 1 */
		if (rxbyte > DM9000_PKT_RDY) {
			DM9000_iow(DM9000_RCR, 0x00);	/* Stop Device */
			DM9000_iow(DM9000_ISR, 0x80);	/* Stop INT request */
			DM9000_DBG("DM9000 error: status check fail: 0x%x\n",
				rxbyte);
			return 0;
		}

		if (rxbyte != DM9000_PKT_RDY)
		{
			return 0; /* No packet received, ignore */
		}

		DM9000_DBG("receiving packet\n");

		/* A packet ready now  & Get status/length */
		(db->rx_status)(&RxStatus, &RxLen);

		DM9000_DBG("rx status: 0x%04x rx len: %d\n", RxStatus, RxLen);

		/* Move data from DM9000 */
		/* Read received packet from RX SRAM */
		(db->inblk)(rdptr, RxLen);

		if ((RxStatus & 0xbf00) || (RxLen < 0x40)
			|| (RxLen > DM9000_PKT_MAX))
		{
			if (RxStatus & 0x100) {
				DM9000_DBG("rx fifo error\n");
			}
			if (RxStatus & 0x200) {
				DM9000_DBG("rx crc error\n");
			}
			if (RxStatus & 0x8000) {
				DM9000_DBG("rx length error\n");
			}
			if (RxLen > DM9000_PKT_MAX) {
				DM9000_DBG("rx length too big\n");
				eth_reset();
			}
		}
		else 
		{
			DM9000_DMP_PACKET("eth_rx", rdptr, RxLen);

			DM9000_DBG("passing packet to upper layer\n");
			NetReceive(NetRxPackets[0], RxLen, player_id, action, station_id);
//			break;
		}
	}
	return 0;
}


/*
  Hardware start transmission.
  Send a packet to media from the upper layer.
*/
int
eth_send(volatile void *packet, int length)
{ 
	int tmo;
	int iNumOfMsecItersToWait = 0;
	struct board_info *db = &dm9000_info;
	
	Uart_Printf ("eth_send: Enter call\n");	

	DM9000_DMP_PACKET("eth_send", packet, length);

	DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

	/* Move data to DM9000 TX RAM */
	DM9000_outb(DM9000_MWCMD, DM9000_IO); /* Prepare for TX-data */

	/* push the data to the TX-fifo */
	(db->outblk)(packet, length);

	/* Set TX length to DM9000 */
	DM9000_iow(DM9000_TXPLL, length & 0xff);
	DM9000_iow(DM9000_TXPLH, (length >> 8) & 0xff);

	/* Issue TX polling command */
	DM9000_iow(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

	/* wait for end of transmission */
	while ( !(DM9000_ior(DM9000_NSR) & (NSR_TX1END | NSR_TX2END)) ||
		!(DM9000_ior(DM9000_ISR) & IMR_PTM) ) {
		if (iNumOfMsecItersToWait ++ == 1000)
		{
			Uart_Printf ("transmission timeout\n");
			break;
		}
		Timer_Bdelay_Milli (2);
	}
	DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

	DM9000_DBG("transmit done\n\n");

	return 0;
	
}
