#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
//#include <arpa/inet.h>

#include "types.h"
#include "Utils.h"
#include "tests.h"

#define NET_MAIN
#define ET_DEBUG
#include "net.h"
//#include "game.h"

#define	ESC_KEY		0x1b
#define MAC_SIZE    6
#define UDP_HDR_SIZE 8

#define DEST_PORT 9999
#define SOURCE_PORT 8888

/*#define diag_Uart_Printf	Uart_Printf*/

//char *cDestIP="10.0.0.1";
char cDestIP[32];
char cSourceIP[32];
char cGateway[32];
char cSubnet[32];
int  bHasBeenSet = 0;

#define debug	Uart_Printf


IPaddr_t		NetPingIP = 0x0000000a; // replace first byte with user-id , in network byte order.
IPaddr_t		NetOurIP = 0x0a000064;
IPaddr_t		NetOurIP_network_order = 0x6400000a;
IPaddr_t		NetOurSubnetMask=0;		/* Our subnet mask (0=unknown)	*/
IPaddr_t		NetOurGatewayIP=0;		/* Our gateways IP address	*/
ushort			NetOurVLAN = 0xFFFF;		/* default is without VLAN	*/
ushort			NetOurNativeVLAN = 0xFFFF;	/* ditto			*/
uchar			NetOurEther[6] = {0x00, 0xaa, 0xbb, 0xaa, 0xbb, 0xaa};
int				NetState;		/* Network loop state			*/
static rxhand_f *packetHandler;		/* Current RX packet handler		*/
static thand_f *timeHandler;		/* Current timeout handler		*/
static ulong	timeStart;		/* Time base value			*/
static ulong	timeDelta;		/* Current timeout value		*/

uchar	       *NetArpWaitPacketMAC;	/* MAC address of waiting packet's destination	*/
uchar	       *NetArpWaitTxPacket;	/* THE transmit packet			*/
uchar			NetEtherNullAddr[6] =	{ 0, 0, 0, 0, 0, 0 };
uchar			NetBcastAddr[6] =	/* Ethernet bcast address		*/
					{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
ulong			NetBootFileXferSize;	/* The actual transferred size of the bootfile (in bytes) */

unsigned		NetIPID = 0;		/* IP packet ID				*/
static ushort 	PingSeqNo = 0;

volatile uchar *NetRxPkt;		/* Current receive packet		*/
int				NetRxPktLen;		/* Current rx packet length		*/

static char 		bDoActAsPingServer = 0;

unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

static void register_player(u8 key)
{
	Uart_Printf ("sending position info for player %u\n", key);
	
	
}

/*should move to game.c file once I understand why undefined referecne error appears*/

static void player_event_handle(char *action, u8 *player_ip, unsigned char *player_id, unsigned char *player_action)
{
	assert(action != NULL);
	u8 player_key = *(player_ip + 3);	
	*player_id = player_key;
	*player_action = *action; 
	
	Uart_Printf ("inside handle_player_event player ID is %u action is 0x%02x\n addresses: p_id=%p, action=%p \n", *player_id, *action, player_id, action);

}


unsigned
NetCksum(uchar * ptr, int len)
{
	ulong	xsum;
	ushort *p = (ushort *)ptr;

	xsum = 0;
	while (len-- > 0)
		xsum += *p++;
	xsum = (xsum & 0xffff) + (xsum >> 16);
	xsum = (xsum & 0xffff) + (xsum >> 16);
	return (xsum & 0xffff);
}


unsigned short csum_calc(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}


static void common_fields_info_set(Ethernet_t *et, IP_t	*ip, udp_ph_t *psh)
{
	/* Fill data of common fields */
	/* ETH header */
	et->et_protlen = htons(0x0800);
	memcpy(et->et_src, NetOurEther, MAC_SIZE);	
	
	/* IP header */
	ip->ip_hl_v  = 0x45;		/* IP_HDR_SIZE / 4 (not including UDP) */	
	ip->ip_tos   = 0;	
	ip->ip_off   = htons(0x4000);	/* No fragmentation */
	ip->ip_ttl   = 255;
	ip->ip_p     = IPPROTO_UDP;			
	NetCopyIP((void*)&ip->ip_src, &NetOurIP); /* already in network byte order */
	
	/* UDP header */
	ip->udp_src = htons(SOURCE_PORT);
	ip->udp_dst = htons(DEST_PORT);	
	
	// Pseudo header for UDP checksum calculation 
    psh->source_address = NetOurIP;
    psh->placeholder = 0;
    psh->protocol = IPPROTO_UDP;
}


/*---------------------------------------------------------------------------------*/






/* write IP *in network byteorder* */
static void NetWriteIP(void *to, IPaddr_t ip)
{
	memcpy(to, (void*)&ip, sizeof(ip));
}

void ip_to_string (IPaddr_t x, char *s)
{
	x = htonl /* ntohl */ (x);
	sprintf (s, "%d.%d.%d.%d",
		 (int) ((x >> 24) & 0xff),
		 (int) ((x >> 16) & 0xff),
		 (int) ((x >> 8) & 0xff), (int) ((x >> 0) & 0xff)
	);
}


IPaddr_t string_to_ip(char *s)
{
	IPaddr_t addr;
	char *e;
	int i;

	if (s == NULL)
		return(0);

	for (addr=0, i=0; i<4; ++i) {
		ulong val = s ? simple_strtoul(s, &e, 10) : 0;
		addr <<= 8;
		addr |= (val & 0xFF);
		if (s) {
			s = (*e) ? e+1 : e;
		}
	}

	return (htonl(addr));
}

IPaddr_t getenv_IPaddr (char *var)
{
	return (string_to_ip(var));
}

void print_IPaddr (IPaddr_t x)
{
	char tmp[16];

	ip_to_string (x, tmp);

	Uart_Printf ("%s\n", tmp);
}
	
void SetIP(void)
{
	 // Getting string input from user
	Uart_Printf ("Please enter IP address....");
	Uart_GetString (cSourceIP);
	
	Uart_Printf ("Please enter Subnet Mask....");
	Uart_GetString (cSubnet);
	
	Uart_Printf ("Please enter Gateway....");
	Uart_GetString (cGateway);
	
	 // Setting IP Parameters
	NetOurIP = string_to_ip(cSourceIP);
	NetOurSubnetMask = string_to_ip(cSubnet);
	NetOurGatewayIP = string_to_ip(cGateway);
	
	 // Printing Output of Intercepted Data
	Uart_Printf ("Current Data Configured:\n");
	Uart_Printf ("IP %s\n", cSourceIP);
	Uart_Printf ("SM %s\n", cSubnet);
	Uart_Printf ("GW %s\n", cGateway);
	
	bHasBeenSet = 1;
}


void ArpRequest (void)
{
	int i;
	volatile uchar *pkt;
	ARP_t *arp;

#ifdef ET_DEBUG
	Uart_Printf ("ARP broadcast %d\n", NetArpWaitTry);
#endif

	Uart_Printf ("ArpRequest: Enter Call\n");
	pkt = NetTxPacket;

	pkt += NetSetEther (pkt, NetBcastAddr, PROT_ARP);

	arp = (ARP_t *) pkt;

	arp->ar_hrd = htons (ARP_ETHER);
	Uart_Printf ("ArpRequest: arp->ar_hrd=0x%x\n", arp->ar_hrd);	
	arp->ar_pro = htons (PROT_IP);
	Uart_Printf ("ArpRequest: arp->ar_pro=0x%x\n", arp->ar_pro);	
	arp->ar_hln = 6;
	Uart_Printf ("ArpRequest: arp->ar_hln=0x%x\n", arp->ar_hln);	
	arp->ar_pln = 4;
	Uart_Printf ("ArpRequest: arp->ar_pln=0x%x\n", arp->ar_pln);	
	arp->ar_op = htons (ARPOP_REQUEST);
	Uart_Printf ("ArpRequest: arp->ar_op=0x%x\n", arp->ar_op);	

	memcpy (&arp->ar_data[0], NetOurEther, 6);		/* source ET addr	*/
	NetWriteIP ((uchar *) & arp->ar_data[6], NetOurIP);	/* source IP addr	*/
	for (i = 10; i < 16; ++i) {
		arp->ar_data[i] = 0;				/* dest ET addr = 0     */
	}

	if ((NetArpWaitPacketIP & NetOurSubnetMask) !=
	    (NetOurIP & NetOurSubnetMask)) {
		if (NetOurGatewayIP == 0) {
			Uart_Printf ("## Warning: gatewayip needed but not set\n");
			NetArpWaitReplyIP = NetArpWaitPacketIP;
		} else {
			NetArpWaitReplyIP = NetOurGatewayIP;
		}
	} else {
		NetArpWaitReplyIP = NetArpWaitPacketIP;
	}

	NetWriteIP ((uchar *) & arp->ar_data[16], NetArpWaitReplyIP);
	if (bDoActAsPingServer == 0)
	{
		(void) eth_send (NetTxPacket, (pkt - NetTxPacket) + ARP_HDR_SIZE);
	}
}

void start_eth_device(void)
{
		// add read from file for IP/mask/info
	bd_t *bd = &sBoardDesc;
	
	if (eth_probe () != 0)
	{
		Uart_Printf ("TestGame: Ethernet interface not found\n");
		return;
	}	
	
	
	eth_halt();
	if (eth_init(bd) < 0)
	{
		eth_halt();
		return;
	}
	
}

void receive_players_actions(unsigned char *player_id, unsigned char *action, unsigned char *station_id)
{
		eth_rx (player_id, action, station_id);
		//Timer_Bdelay_Milli (100); 
}






void send_updates(player_t *player, unsigned char player_id, unsigned int num_of_players , position_t *maze, unsigned char action)
{
	Ethernet_t *et;
	IP_t	*ip;
	char *data_id, *data, *pseudogram_start, *pseudogram_game;
	IPaddr_t tmp;
	int	i,j;
	uchar *packet = (uchar*)malloc(1000);
	udp_ph_t psh;                                    // pseudo header struct variable for UDP checksum
	IPaddr_t player_ip;
	int total_length = E802_HDR_SIZE + sizeof(IP_t);  // Total length of headers - without data, will be added later. (IP struct includes UDP headet as well)
	uchar *player_ptr = NULL;

	
	unsigned short int map_msg_id = 0xaaaa;
	unsigned short int action_msg_id = 0xbbbb;
	
	uchar map_data_size = sizeof(map_msg_id) + sizeof(position_t);
	uchar action_data_size = sizeof(action_msg_id) + sizeof(player_t);
	
	
		
	/* PLACE header/data pointers */
	et = (Ethernet_t *)packet;
	ip = (IP_t *)(packet + ETHER_HDR_SIZE);
	data_id = packet + ETHER_HDR_SIZE + sizeof(IP_t);
	data =  packet + ETHER_HDR_SIZE + sizeof(IP_t) + sizeof(unsigned short int);
	
	/* set common eth/ip header info */
	common_fields_info_set(et, ip, &psh);
	
	
	/* allocate space for pseudo datagrams for UDP checksum calculation.
	 * two different datagrams are needed for the two different message types
	 *  */
	int psize_start = sizeof(udp_ph_t) + UDP_HDR_SIZE + map_data_size;
	int psize_game = sizeof(udp_ph_t) + UDP_HDR_SIZE + action_data_size;
    pseudogram_start = malloc(psize_start);
    pseudogram_game = malloc(psize_game);


	// In case it's a new player send map only to him
	if (action == START) 
	{
		Util_Printf("sending map updates\n");
		ip->ip_len   = htons(IP_HDR_SIZE + map_data_size);
		ip->udp_len = htons(UDP_HDR_SIZE + map_data_size);	
		psh.udp_length = htons(UDP_HDR_SIZE + map_data_size);
		total_length+= map_data_size;
			
		memcpy(et->et_dest, &player[player_id].station_id, MAC_SIZE);		
		player_ptr = (uchar *)&NetPingIP;
		player_ptr[3] = player[player_id].player_id;
		
		NetCopyIP((void*)&ip->ip_dst, &NetPingIP);
		psh.dest_address = NetPingIP;

		// need to break map information into smaller messages, map is to big for a single message.
		for (i = 0; i < MAZE_SIZE; i++)
		{
			ip->ip_id    = htons(NetIPID++);
			
			memcpy((void*)data_id, &map_msg_id, sizeof(map_msg_id));
			memcpy((void*)data, &maze[i], map_data_size);
										   
			ip->ip_sum   = 0;	
			ip->ip_sum   = csum_calc((unsigned short*)ip, IP_HDR_SIZE_NO_UDP);
					 
			ip->udp_xsum = 0; 
			memcpy(pseudogram_start , (char*) &psh , sizeof(udp_ph_t));
			memcpy(pseudogram_start + sizeof(udp_ph_t), &ip->udp_src, UDP_HDR_SIZE + map_data_size);
			ip->udp_xsum = csum_calc((unsigned short*)pseudogram_start , psize_start);
			
			eth_send((volatile void *)packet, total_length);	
		}
	}
	// In other cases, send player actions to all active players
	else
	{
		ip->ip_len   = htons(IP_HDR_SIZE + action_data_size);
		ip->udp_len = htons(UDP_HDR_SIZE + action_data_size);
		psh.udp_length = htons(UDP_HDR_SIZE + action_data_size);
		total_length+=action_data_size;
		
		
		Util_Printf("sending action updates\n");
		for (i = 0; i < num_of_players; i++)
		{
			//send updates only to active players 
			if (player[i].active==1)
			{			
				/* movement update data */
				
				memcpy((void*)data_id, &action_msg_id, sizeof(action_msg_id));
				memcpy((void*)data, &player[i], action_data_size);				
												
				for (j = i; j < num_of_players; j++)
				{	
					if (player[j].active == 1)
					{
						memcpy(et->et_dest, &player[j].station_id, MAC_SIZE);		
						ip->ip_id    = htons(NetIPID++);
						player_ptr = (uchar *)&NetPingIP;
						player_ptr[3] = player[j].player_id;

						NetCopyIP((void*)&ip->ip_dst, &NetPingIP);	   
						ip->ip_sum   = 0;	
						ip->ip_sum   = csum_calc((unsigned short*)ip, IP_HDR_SIZE_NO_UDP);								 
						psh.dest_address = NetPingIP;

						ip->udp_xsum = 0; 
						memcpy(pseudogram_game , (char*) &psh , sizeof(udp_ph_t));
						memcpy(pseudogram_game + sizeof(udp_ph_t), &ip->udp_src, UDP_HDR_SIZE +  action_data_size);

						ip->udp_xsum = csum_calc((unsigned short*)pseudogram_game , psize_game);
					
					
						
						eth_send((volatile void *)packet, total_length);

					}							
				}
			}
		}
		
	Util_Printf("sending action updates finished\n");
	}

	free(packet);
	free(pseudogram_start);
	free(pseudogram_game);
}




void TestPing (void)
{
	if (bDoActAsPingServer == 0)
	{
		Uart_Printf ("Please enter IP address....");
		Uart_GetString (cDestIP);
		
		NetPingIP = string_to_ip(cDestIP);
		
		Uart_Printf ("TestPing: NetPingIP = 0x%x\n", NetPingIP);
		
		if (NetPingIP == 0)
		{
			Uart_Printf ("TestPing: Zero or Invalid IP address\n");
			return;
		}
	}
	else
	{
		NetPingIP = string_to_ip("10.0.0.0");	
	}

	
	if (eth_probe () != 0)
	{
		Uart_Printf ("TestPing: Ethernet interface not found\n");
		return;

	}	
	
	if (NetLoop(PING) < 0) {
		if (bDoActAsPingServer == 0)
		{
			Uart_Printf("TestPing failed; host %s is not alive\n", cDestIP);
		}
	}
	else
		Uart_Printf("host %s is alive\n", cDestIP);
		
	bDoActAsPingServer = 0;	
}


int
NetSetEther(volatile uchar * xet, uchar * addr, uint prot)
{
	Ethernet_t *et = (Ethernet_t *)xet;
	ushort myvlanid;

	myvlanid = htons(NetOurVLAN);	
	if (myvlanid == (ushort)-1)
	{
		Uart_Printf ("NetSetEther VLAN_NONE\n");
		myvlanid = VLAN_NONE;
	}
	memcpy (et->et_dest, addr, 6);
	memcpy (et->et_src, NetOurEther, 6);
	if ((myvlanid & VLAN_IDMASK) == VLAN_NONE)
	{
		Uart_Printf ("NetSetEther VLAN_NONE and IDMASK\n");
		et->et_protlen = htons(prot);
		return ETHER_HDR_SIZE;
	}
}



int
NetCksumOk(uchar * ptr, int len)
{
	return !((NetCksum(ptr, len) + 1) & 0xfffe);
}



void
NetSetHandler(rxhand_f * f)
{
	packetHandler = f;
}

int PingSend(void)
{
	static uchar mac[6];
	volatile IP_t *ip;
	volatile ushort *s;
	uchar *pkt;

	/* XXX always send arp request */

	memcpy(mac, NetEtherNullAddr, 6);
	Uart_Printf  ("PingSend: Entering call mac = 0x%x\n", mac);	

#ifdef ET_DEBUG
	Uart_Printf("sending ARP for %08lx\n", NetPingIP);
#endif

	NetArpWaitPacketIP = NetPingIP;
	NetArpWaitPacketMAC = mac;

	pkt = NetArpWaitTxPacket;
	pkt += NetSetEther(pkt, mac, PROT_IP);

	ip = (volatile IP_t *)pkt;

	/*
	 *	Construct an IP and ICMP header.  (need to set no fragment bit - XXX)
	 */
	ip->ip_hl_v  = 0x45;		/* IP_HDR_SIZE / 4 (not including UDP) */
	Uart_Printf ("PingSend: ip->ip_hl_v = 0x%x\n", ip->ip_hl_v);	
	ip->ip_tos   = 0;
	Uart_Printf ("PingSend: ip->tos = 0x%x\n", ip->ip_tos);	
	ip->ip_len   = htons(IP_HDR_SIZE_NO_UDP + 8);
	Uart_Printf ("PingSend: ip->len = %d\n", ip->ip_len);	
	ip->ip_id    = htons(NetIPID++);
	Uart_Printf ("PingSend: ip->id = %d\n", ip->ip_id);	
	ip->ip_off   = htons(0x4000);	/* No fragmentation */
	Uart_Printf ("PingSend: ip->off = 0x%x\n", ip->ip_off);	
	ip->ip_ttl   = 255;
	Uart_Printf ("PingSend: ip->ttl = %d\n", ip->ip_ttl);	
	ip->ip_p     = 0x01;		/* ICMP */
	Uart_Printf ("PingSend: ip->protocol = 0x%x\n", ip->ip_p);	
	ip->ip_sum   = 0;
	NetCopyIP((void*)&ip->ip_src, &NetOurIP); /* already in network byte order */
	Uart_Printf ("PingSend: ip->src = 0x%x\n", NetOurIP);	
	NetCopyIP((void*)&ip->ip_dst, &NetPingIP);	   /* - "" - */
	Uart_Printf ("PingSend: ip->dst = 0x%x\n", NetPingIP);	
	ip->ip_sum   = ~NetCksum((uchar *)ip, IP_HDR_SIZE_NO_UDP / 2);
	Uart_Printf ("PingSend: ip->sum = 0x%x\n", ip->ip_sum);	

	s = &ip->udp_src;		/* XXX ICMP starts here */
	s[0] = htons(0x0800);		/* echo-request, code */
	s[1] = 0;					/* checksum */
	s[2] = 0; 					/* identifier */
	s[3] = htons(PingSeqNo++);	/* sequence number */
	
	Uart_Printf ("PingSend: s[0] = 0x%x\n", s[0]);
	Uart_Printf ("PingSend: s[1] = 0x%x\n", s[1]);
	Uart_Printf ("PingSend: s[2] = 0x%x\n", s[2]);
	Uart_Printf ("PingSend: s[3] = 0x%x\n", s[3]);
	
	s[1] = ~NetCksum((uchar *)s, 8/2);
	Uart_Printf ("PingSend: s[1] After reprocessing = 0x%x\n", s[1]);	
	
	

	/* size of the waiting packet */
	NetArpWaitTxPacketSize = (pkt - NetArpWaitTxPacket) + IP_HDR_SIZE_NO_UDP + 8;

	/* and do the ARP request */
	NetArpWaitTry = 1;

	Uart_Printf ("PingSend: Call ArpRequest\n");	
	ArpRequest();
	return 1;	/* waiting */
}

static void
PingHandler (uchar * pkt, unsigned dest, unsigned src, unsigned len)
{
	IPaddr_t tmp;
	volatile IP_t *ip = (volatile IP_t *)pkt;

	tmp = NetReadIP((void *)&ip->ip_src);
	Uart_Printf ("PingHandler: ip_src = 0x%x, NetPingIP = 0x%x\n", tmp, NetPingIP);
	if (tmp != NetPingIP)
		return;

	NetState = NETLOOP_SUCCESS;
}


static void PingStart(void)
{
	NetSetHandler (PingHandler);
	PingSend();
	return;	
}



static int net_check_prereq (proto_t protocol)
{
/*
	Original Code surpressed.
*/
	Uart_Printf ("net_check_prereq: protocol = %d\n", protocol);
	if (NetPingIP == 0)
	{
		Uart_Printf ("*** ERROR: ping address not given\n");
		return (1);
	}
	
	if (memcmp (NetOurEther, "\0\0\0\0\0\0", 6) == 0)
	{
		Uart_Printf ("*** ERROR: `ethaddr' not set\n");
		return (1);
	}
	
	return 0;
}	



/**********************************************************************/
/*
 *	Main network processing loop.
 */

int
NetLoop(proto_t protocol)
{
	bd_t *bd = &sBoardDesc;

	Uart_Printf ("NetLoop: Enter call - protocol ID = %d\n", protocol); 

	/* XXX problem with bss workaround */
	NetArpWaitPacketMAC = NULL;
	NetArpWaitTxPacket = NULL;
	NetArpWaitPacketIP = 0;
	NetArpWaitReplyIP = 0;
	NetArpWaitTxPacket = NULL;
	NetTxPacket = NULL;
	
	
	if (!NetTxPacket) { /* Used for ARP request */
		int	i;
		/*
		 *	Setup packet buffers, aligned correctly.
		 */
		NetTxPacket = &PktBuf[0] + (PKTALIGN - 1);
		NetTxPacket -= (ulong)NetTxPacket % PKTALIGN;
		for (i = 0; i < PKTBUFSRX; i++) {
			NetRxPackets[i] = NetTxPacket + (i+1)*PKTSIZE_ALIGN;
		}
		Uart_Printf ("NetLoop: allocate space for %d NetRxPackets aligned to %d at address 0x%x\n", PKTBUFSRX, PKTALIGN, (ulong)NetTxPacket);		
	}

	if (!NetArpWaitTxPacket) { /* Used for ICMP - contains IP header */
		NetArpWaitTxPacket = &NetArpWaitPacketBuf[0] + (PKTALIGN - 1);
		NetArpWaitTxPacket -= (ulong)NetArpWaitTxPacket % PKTALIGN;
		NetArpWaitTxPacketSize = 0;
		Uart_Printf ("NetLoop: Resetting NetArpWaitTxPacket to address 0x%x\n", (ulong)NetArpWaitTxPacket);		
	}

	
	eth_halt();
	if (eth_init(bd) < 0)
	{
		eth_halt();
		return;
	}
	

restart:
	memcpy (NetOurEther, bd->bi_enetaddr, 6);


	NetState = NETLOOP_CONTINUE;
	
	if (bHasBeenSet == 0)
	{
		NetOurIP = string_to_ip(CONFIG_IPADDR);		
		NetOurGatewayIP = string_to_ip (CONFIG_GATEWAY);
		NetOurSubnetMask= string_to_ip (CONFIG_NETMASK);
		
		bHasBeenSet = 1;
	}	
	
	NetOurVLAN = htons(CONFIG_VLAN_IDMASK); 
	NetOurNativeVLAN = htons(CONFIG_VLAN_IDMASK);
	Uart_Printf ("NetLoop: NetOurIP = 0x%x, NetOurGatewayIP=0x%x, NetOurSubnetMask=0x%x, NetOurVLAN=0x%x NetOurNativeVLAN=0x%x\n",
			NetOurIP, NetOurGatewayIP, NetOurSubnetMask, NetOurVLAN, NetOurNativeVLAN);		

	Uart_Printf ("NetLoop: calling PingStart\n");
	PingStart();
	NetBootFileXferSize = 0;					
	

 	while( Uart_Getkey() != ESC_KEY ) 
 	{
 		eth_rx ();
		Timer_Bdelay_Milli (2000); 		

		if (bDoActAsPingServer == 0)
		{
			if (!NetArpWaitPacketIP)
			{
				if (NetState == NETLOOP_SUCCESS)
				{
					break;
				}
			}
			else
				ArpRequest();
		}
 	}
 	
	if (NetState == NETLOOP_SUCCESS)
		return 0;
	else
		return -1;						

}


void
NetReceive(volatile uchar * inpkt, int len, unsigned char *player_id, unsigned char *action, unsigned char *station_id)
{
	Ethernet_t *et;
	IP_t	*ip;
	ARP_t	*arp;
	IPaddr_t tmp;
	int	x;
	uchar *pkt;
	IPaddr_t player_ip;
	ushort cti = 0, vlanid = VLAN_NONE, myvlanid, mynvlanid;


	NetRxPkt = inpkt;
	NetRxPktLen = len;
	et = (Ethernet_t *)inpkt;
	/*copy station_id info*/	
	memcpy((void*)station_id, et->et_src, 6); 
	
	Uart_Printf("packet received from src_mac = %02x:%02x:%02x:%02x:%02x:%02x\n", 
	station_id[0], station_id[1], station_id[2], station_id[3], station_id[4], station_id[5]);
	
	/* too small packet? */
	if (len < ETHER_HDR_SIZE)
		return;



	myvlanid = htons(NetOurVLAN);	
	if (myvlanid == (ushort)-1)
		myvlanid = VLAN_NONE;

	mynvlanid = htons(NetOurNativeVLAN);
	if (mynvlanid == (ushort)-1)
		mynvlanid = VLAN_NONE;


	x = htons(et->et_protlen);	

#ifdef ET_DEBUG
	Uart_Printf("packet received\n");
#endif

	if (x < 1514) {
		/*
		 *	Got a 802 packet.  Check the other protocol field.
		 */
		// x = htons /* ntohs */(et->et_prot);
		x = htons(et->et_prot);	

		ip = (IP_t *)(inpkt + E802_HDR_SIZE);
		len -= E802_HDR_SIZE;

	}
	else if (x != PROT_VLAN)
	{	/* normal packet */
		ip = (IP_t *)(inpkt + ETHER_HDR_SIZE);
		len -= ETHER_HDR_SIZE;
	}

#ifdef ET_DEBUG
	Uart_Printf("Receive from protocol 0x%x\n", x);
#endif



	switch (x) {
	case PROT_ARP:
		/*
		 * We have to deal with two types of ARP packets:
		 * - REQUEST packets will be answered by sending  our
		 *   IP address - if we know it.
		 * - REPLY packates are expected only after we asked
		 *   for the TFTP server's or the gateway's ethernet
		 *   address; so if we receive such a packet, we set
		 *   the server ethernet address
		 */
#ifdef ET_DEBUG
		Uart_Printf ("Got ARP\n");
#endif
		arp = (ARP_t *)ip;
		if (len < ARP_HDR_SIZE) {
			Uart_Printf("bad length %d < %d\n", len, ARP_HDR_SIZE);
			return;
		}

		if (NetReadIP(&arp->ar_data[16]) != htonl(NetOurIP)) {
			u8 *ip_ptr = (u8*)&NetOurIP;
			Uart_Printf ("dest IP data=%u.%u.%u.%u, out IP is 0x%08x or %u.%u.%u.%u\n", 
			arp->ar_data[16], arp->ar_data[17], arp->ar_data[18], arp->ar_data[19], NetOurIP, 
			*ip_ptr, *(ip_ptr + 1), *(ip_ptr + 2), *(ip_ptr + 3)); 
			return;
		}
		Uart_Printf ("Got correct ARP packet\n");
		switch (htons /* ntohs */(arp->ar_op)) {
		case ARPOP_REQUEST:		/* reply with our IP address	*/
#ifdef ET_DEBUG
			Uart_Printf ("Got ARP REQUEST, return our IP\n");
#endif
			et = (Ethernet_t *)inpkt;
			memcpy (et->et_dest, et->et_src, MAC_SIZE);
			memcpy (et->et_src, NetOurEther, MAC_SIZE);
			memcpy (&arp->ar_data[10], &arp->ar_data[0], 6);
			NetCopyIP(&arp->ar_data[16], &arp->ar_data[6]);
			memcpy   (&arp->ar_data[ 0], NetOurEther, 6);
			NetCopyIP(&arp->ar_data[ 6], &NetOurIP_network_order);
			arp->ar_op = htons(ARPOP_REPLY);
			(void) eth_send((uchar *)et, len);
			return;

		case ARPOP_REPLY:		/* arp reply */
			/* are we waiting for a reply */
			if (!NetArpWaitPacketIP || !NetArpWaitPacketMAC)
				break;
#ifdef ET_DEBUG
			Uart_Printf("Got ARP REPLY, set server/gtwy eth addr (%02x:%02x:%02x:%02x:%02x:%02x)\n",
				arp->ar_data[0], arp->ar_data[1],
				arp->ar_data[2], arp->ar_data[3],
				arp->ar_data[4], arp->ar_data[5]);
#endif

			tmp = NetReadIP(&arp->ar_data[6]);

			/* matched waiting packet's address */
			if (tmp == NetArpWaitReplyIP) {
#ifdef ET_DEBUG
				Uart_Printf ("Got it\n");
#endif
				/* save address for later use */
				memcpy(NetArpWaitPacketMAC, &arp->ar_data[0], 6);

				/* modify header, and transmit it */
				memcpy(((Ethernet_t *)NetArpWaitTxPacket)->et_dest, NetArpWaitPacketMAC, 6);
				(void) eth_send(NetArpWaitTxPacket, NetArpWaitTxPacketSize);

				/* no arp request pending now */
				NetArpWaitPacketIP = 0;
				NetArpWaitTxPacketSize = 0;
				NetArpWaitPacketMAC = NULL;

			}
			return;
		default:
#ifdef ET_DEBUG
			Uart_Printf("Unexpected ARP opcode 0x%x\n", htons (arp->ar_op));
#endif
			return;
		}
		break;

	case PROT_IP:
#ifdef ET_DEBUG
		Uart_Printf ("Got IP\n");
#endif
		if (len < IP_HDR_SIZE) {
			debug ("len bad %d < %d\n", len, IP_HDR_SIZE);
			return;
		}
		if (len < htons (ip->ip_len)) {
			Uart_Printf("len bad %d < %d\n", len, htons /* ntohs */(ip->ip_len));
			return;
		}
		len = htons (ip->ip_len);
#ifdef ET_DEBUG
		Uart_Printf("len=%d, v=%02x\n", len, ip->ip_hl_v>>4 & 0xf);
#endif

		/*
		 * watch for ICMP host redirects
		 *
		 * There is no real handler code (yet). We just watch
		 * for ICMP host redirect messages. In case anybody
		 * sees these messages: please contact me
		 * (wd@denx.de), or - even better - send me the
		 * necessary fixes :-)
		 *
		 * Note: in all cases where I have seen this so far
		 * it was a problem with the router configuration,
		 * for instance when a router was configured in the
		 * BOOTP reply, but the TFTP server was on the same
		 * subnet. So this is probably a warning that your
		 * configuration might be wrong. But I'm not really
		 * sure if there aren't any other situations.
		 */
		if (ip->ip_p == IPPROTO_ICMP)
		{
			ICMP_t *icmph = (ICMP_t *)&(ip->udp_src);

					 
			switch (icmph->type)
			{
				case ICMP_ECHO_REPLY:
					#ifdef ET_DEBUG
						Uart_Printf ("Got ICMP_ECHO_REPLY\n");
					#endif
				
					/*
					 *	IP header OK.  Pass the packet to the current handler.
					 */
					/* XXX point to ip packet */
					(*packetHandler)((uchar *)ip, 0, 0, 0);
					return;
				case ICMP_ECHO_REQUEST:
	#ifdef ET_DEBUG
					Uart_Printf ("Got ICMP ECHO REQUEST, return %d bytes \n",
						ETHER_HDR_SIZE + len);
	#endif
					memcpy (&et->et_dest[0], &et->et_src[0], 6);
					memcpy (&et->et_src[ 0], NetOurEther, 6);

					ip->ip_sum = 0;
					ip->ip_off = 0;
					NetCopyIP((void*)&ip->ip_dst, &ip->ip_src);
					NetCopyIP((void*)&ip->ip_src, &NetOurIP);
					ip->ip_sum = ~NetCksum((uchar *)ip, IP_HDR_SIZE_NO_UDP >> 1);

					icmph->type = ICMP_ECHO_REPLY;
					icmph->checksum = 0;
					icmph->checksum = ~NetCksum((uchar *)icmph,
							(len - IP_HDR_SIZE_NO_UDP) >> 1);
					(void) eth_send((uchar *)et, ETHER_HDR_SIZE + len);
					return;

				default:
					return;
			}
		}
		else if (ip->ip_p == IPPROTO_UDP)
		{	/* Only UDP packets */
			Uart_Printf ("Got a udp packet\n");
			char *ptr = (char*)ip;
			ptr += (ip->ip_hl_v & 0x0f)*4 + UDP_HDR_SIZE;
			player_event_handle(ptr, (u8*)&ip->ip_src, player_id, action);
			
			
			 
			return;
		}



		/*
		 *	IP header OK.  Pass the packet to the current handler.
		 */
		(*packetHandler)((uchar *)ip +IP_HDR_SIZE,
						htons /* ntohs */(ip->udp_dst),
						htons /* ntohs */(ip->udp_src),
						htons /* ntohs */(ip->udp_len) - 8);
		break;
	}
}
