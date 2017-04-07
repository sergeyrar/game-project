/*
	General definitions and structures Ethernet communication protocols.
*/

#ifndef NET_H
#define NET_H


/*
	General ported definitions.
*/

typedef enum { BOOTP, RARP, ARP, TFTP, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP } proto_t;

//#define htonl(lVal) (((lVal & 0xFF) << 24) | (((lVal >> 8) & 0xFF) << 16) | (((lVal >> 16) & 0xFF) << 8) | (lVal >> 24))   
#define htonl(lVal) (long)((((long)lVal & 0xFF) << 24) | ((((long)lVal) & 0xFF00) << 8) | ((((long)lVal) & 0xFF0000) >> 8) | (((long)lVal & 0xFF000000) >> 24))   
#define htons(lVal) (long)((((long)lVal & 0xFF) << 8) | ((((short)lVal) & 0xFF00) >> 8))   


#define DM9000_DMP_PACKET(func,packet,length)  \
	do { \
		int i; 							\
		Uart_Printf(func ": length: %d\n", length);			\
		for (i = 0; i < length; i++) {				\
			if (i % 8 == 0)					\
				Uart_Printf("\n%s: %02x: ", func, i);	\
			Uart_Printf("%02x ", ((unsigned char *) packet)[i]);	\
		} Uart_Printf("\n");						\
	} while(0)



#define __constant_cpu_to_le64(x) ((__u64)(x))
#define __constant_le64_to_cpu(x) ((__u64)(x))
#define __constant_cpu_to_le32(x) ((__u32)(x))
#define __constant_le32_to_cpu(x) ((__u32)(x))
#define __constant_cpu_to_le16(x) ((__u16)(x))
#define __constant_le16_to_cpu(x) ((__u16)(x))
#define __cpu_to_le64(x) ((__u64)(x))
#define __le64_to_cpu(x) ((__u64)(x))
#define __cpu_to_le32(x) ((__u32)(x))
#define __le32_to_cpu(x) ((__u32)(x))
#define __cpu_to_le16(x) ((__u16)(x))
#define __le16_to_cpu(x) ((__u16)(x))





#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

/*
	Ethernet protocols definitions
*/
/*
 * Maximum packet size; used to allocate packet storage.
 * TFTP packets can be 524 bytes + IP header + ethernet header.
 * Lets be conservative, and go for 38 * 16.  (Must also be
 * a multiple of 32 bytes).
 */
/*
 * AS.HARNOIS : Better to set PKTSIZE to maximum size because
 * traffic type is not always controlled
 * maximum packet size =  1518
 * maximum packet size and multiple of 32 bytes =  1536
 */
#define PKTSIZE			1518
#define PKTSIZE_ALIGN	1536
#define PKTALIGN		32
# define PKTBUFSRX		4
/*#define PKTSIZE		608*/


#define NETLOOP_CONTINUE	1
#define NETLOOP_RESTART		2
#define NETLOOP_SUCCESS		3
#define NETLOOP_FAIL		4


#define CONFIG_ETHADDR	    "08:08:11:18:12:27"
#define CONFIG_NETMASK      "255.0.0.0"
#define CONFIG_GATEWAY		"192.168.0.2"
#define CONFIG_IPADDR		"192.168.0.3"
#define CONFIG_SERVERIP		"192.168.0.3"
#define CONFIG_VLAN_IDMASK	0x0FFF			/* mask of valid vlan id 	*/


#define PROT_IP				0x0800		/* IP protocol			*/
#define PROT_ARP			0x0806		/* IP ARP protocol		*/
#define PROT_RARP			0x8035		/* IP ARP protocol		*/
#define PROT_VLAN			0x8100		/* IEEE 802.1q protocol		*/


#define IPPROTO_ICMP	 	1	/* Internet Control Message Protocol	*/
#define IPPROTO_UDP			17	/* User Datagram Protocol		*/


#define IP_HDR_SIZE_NO_UDP	(sizeof (IP_t) - 8)
#define IP_HDR_SIZE			(sizeof (IP_t))
#define ETHER_HDR_SIZE		14		/* Ethernet header size		*/
#define ARP_HDR_SIZE		(8+20)		/* Size assuming ethernet	*/
#define E802_HDR_SIZE		22		/* 802 ethernet header size	*/

#define VLAN_NONE			4095			/* untagged 			*/
#define VLAN_ETHER_HDR_SIZE	18	/* VLAN Ethernet header size	*/
#define VLAN_IDMASK			CONFIG_VLAN_IDMASK


/*
 * ICMP stuff (just enough to handle (host) redirect messages)
 */
#define ICMP_ECHO_REPLY		0	/* Echo reply 			*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO_REQUEST	8	/* Echo request			*/

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/

/*
	General ported types.
*/
typedef unsigned long 	ulong;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned char	uchar;

typedef unsigned char	u8, __8;
typedef unsigned short	u16, __u16;
typedef unsigned long	u32, __u32;

/*
 * The current receive packet handler.  Called with a pointer to the
 * application packet, and a protocol type (PORT_BOOTPC or PORT_TFTP).
 * All other packets are dealt with without calling the handler.
 */
typedef void	rxhand_f(uchar *, unsigned, unsigned, unsigned);

/*
 *	A timeout handler.  Called after time interval has expired.
 */
typedef void	thand_f(void);


typedef struct bd_info {
	unsigned long	bi_memstart;	/* start of DRAM memory */
	unsigned long	bi_memsize;	/* size	 of DRAM memory in bytes */
	unsigned long	bi_flashstart;	/* start of FLASH memory */
	unsigned long	bi_flashsize;	/* size	 of FLASH memory */
	unsigned long	bi_flashoffset; /* reserved area for startup monitor */
	unsigned long	bi_sramstart;	/* start of SRAM memory */
	unsigned long	bi_sramsize;	/* size	 of SRAM memory */
#if defined(CONFIG_5xx) || defined(CONFIG_8xx) || defined(CONFIG_8260) \
	|| defined(CONFIG_E500) || defined(CONFIG_MPC86xx)
	unsigned long	bi_immr_base;	/* base of IMMR register */
#endif
#if defined(CONFIG_MPC5xxx)
	unsigned long	bi_mbar_base;	/* base of internal registers */
#endif
#if defined(CONFIG_MPC83XX)
	unsigned long	bi_immrbar;
#endif
#if defined(CONFIG_MPC8220)
	unsigned long	bi_mbar_base;	/* base of internal registers */
	unsigned long   bi_inpfreq;     /* Input Freq, In MHz */
	unsigned long   bi_pcifreq;     /* PCI Freq, in MHz */
	unsigned long   bi_pevfreq;     /* PEV Freq, in MHz */
	unsigned long   bi_flbfreq;     /* Flexbus Freq, in MHz */
	unsigned long   bi_vcofreq;     /* VCO Freq, in MHz */
#endif
	unsigned long	bi_bootflags;	/* boot / reboot flag (for LynxOS) */
	unsigned long	bi_ip_addr;	/* IP Address */
	unsigned char	bi_enetaddr[6];	/* Ethernet adress */
	unsigned short	bi_ethspeed;	/* Ethernet speed in Mbps */
	unsigned long	bi_intfreq;	/* Internal Freq, in MHz */
	unsigned long	bi_busfreq;	/* Bus Freq, in MHz */
#if defined(CONFIG_CPM2)
	unsigned long	bi_cpmfreq;	/* CPM_CLK Freq, in MHz */
	unsigned long	bi_brgfreq;	/* BRG_CLK Freq, in MHz */
	unsigned long	bi_sccfreq;	/* SCC_CLK Freq, in MHz */
	unsigned long	bi_vco;		/* VCO Out from PLL, in MHz */
#endif
#if defined(CONFIG_MPC512X)
	unsigned long	bi_ipsfreq;	/* IPS Bus Freq, in MHz */
#endif /* CONFIG_MPC512X */
#if defined(CONFIG_MPC5xxx)
	unsigned long	bi_ipbfreq;	/* IPB Bus Freq, in MHz */
	unsigned long	bi_pcifreq;	/* PCI Bus Freq, in MHz */
#endif
	unsigned long	bi_baudrate;	/* Console Baudrate */
#if defined(CONFIG_405)   || \
    defined(CONFIG_405GP) || \
    defined(CONFIG_405CR) || \
    defined(CONFIG_405EP) || \
    defined(CONFIG_405EZ) || \
    defined(CONFIG_405EX) || \
    defined(CONFIG_440)
	unsigned char	bi_s_version[4];	/* Version of this structure */
	unsigned char	bi_r_version[32];	/* Version of the ROM (AMCC) */
	unsigned int	bi_procfreq;	/* CPU (Internal) Freq, in Hz */
	unsigned int	bi_plb_busfreq;	/* PLB Bus speed, in Hz */
	unsigned int	bi_pci_busfreq;	/* PCI Bus speed, in Hz */
	unsigned char	bi_pci_enetaddr[6];	/* PCI Ethernet MAC address */
#endif
#if defined(CONFIG_HYMOD)
	hymod_conf_t	bi_hymod_conf;	/* hymod configuration information */
#endif

#ifdef CONFIG_HAS_ETH1
	/* second onboard ethernet port */
	unsigned char   bi_enet1addr[6];
#endif
#ifdef CONFIG_HAS_ETH2
	/* third onboard ethernet port */
	unsigned char	bi_enet2addr[6];
#endif
#ifdef CONFIG_HAS_ETH3
	unsigned char   bi_enet3addr[6];
#endif

#if defined(CONFIG_405GP) || defined(CONFIG_405EP) || \
    defined(CONFIG_405EZ) || defined(CONFIG_440GX) || \
    defined(CONFIG_440EP) || defined(CONFIG_440GR) || \
    defined(CONFIG_440EPX) || defined(CONFIG_440GRX) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT)
	unsigned int	bi_opbfreq;		/* OPB clock in Hz */
	int		bi_iic_fast[2];		/* Use fast i2c mode */
#endif
#if defined(CONFIG_NX823)
	unsigned char	bi_sernum[8];
#endif
#if defined(CONFIG_4xx)
#if defined(CONFIG_440GX) || \
    defined(CONFIG_460EX) || defined(CONFIG_460GT)
	int 		bi_phynum[4];           /* Determines phy mapping */
	int 		bi_phymode[4];          /* Determines phy mode */
#elif defined(CONFIG_405EP) || defined(CONFIG_440)
	int 		bi_phynum[2];           /* Determines phy mapping */
	int 		bi_phymode[2];          /* Determines phy mode */
#else
	int 		bi_phynum[1];           /* Determines phy mapping */
	int 		bi_phymode[1];          /* Determines phy mode */
#endif
#endif /* defined(CONFIG_4xx) */
} bd_t;


/*
	Ethernet types.
*/	

typedef ulong		IPaddr_t;

/*
 *	Address Resolution Protocol (ARP) header.
 */
typedef struct
{
	ushort		ar_hrd;		/* Format of hardware address	*/
#   define ARP_ETHER	    1		/* Ethernet  hardware address	*/
	ushort		ar_pro;		/* Format of protocol address	*/
	uchar		ar_hln;		/* Length of hardware address	*/
	uchar		ar_pln;		/* Length of protocol address	*/
	ushort		ar_op;		/* Operation			*/
#   define ARPOP_REQUEST    1		/* Request  to resolve  address	*/
#   define ARPOP_REPLY	    2		/* Response to previous request	*/

#   define RARPOP_REQUEST   3		/* Request  to resolve  address	*/
#   define RARPOP_REPLY	    4		/* Response to previous request */

	/*
	 * The remaining fields are variable in size, according to
	 * the sizes above, and are defined as appropriate for
	 * specific hardware/protocol combinations.
	 */
	uchar		ar_data[20];
#if 0
	uchar		ar_sha[];	/* Sender hardware address	*/
	uchar		ar_spa[];	/* Sender protocol address	*/
	uchar		ar_tha[];	/* Target hardware address	*/
	uchar		ar_tpa[];	/* Target protocol address	*/
#endif /* 0 */
} ARP_t;


/*
 *	Internet Protocol (IP) header.
 */
typedef struct {
	uchar		ip_hl_v;	/* header length and version	*/
	uchar		ip_tos;		/* type of service		*/
	ushort		ip_len;		/* total length			*/
	ushort		ip_id;		/* identification		*/
	ushort		ip_off;		/* fragment offset field	*/
	uchar		ip_ttl;		/* time to live			*/
	uchar		ip_p;		/* protocol			*/
	ushort		ip_sum;		/* checksum			*/
	IPaddr_t	ip_src;		/* Source IP address		*/
	IPaddr_t	ip_dst;		/* Destination IP address	*/
	ushort		udp_src;	/* UDP source port		*/
	ushort		udp_dst;	/* UDP destination port		*/
	ushort		udp_len;	/* Length of UDP packet		*/
	ushort		udp_xsum;	/* Checksum			*/
} IP_t;

/*
 *	Ethernet header
 */
typedef struct {
	uchar		et_dest[6];	/* Destination node		*/
	uchar		et_src[6];	/* Source node			*/
	ushort		et_protlen;	/* Protocol or length		*/
	uchar		et_dsap;	/* 802 DSAP			*/
	uchar		et_ssap;	/* 802 SSAP			*/
	uchar		et_ctl;		/* 802 control			*/
	uchar		et_snap1;	/* SNAP				*/
	uchar		et_snap2;
	uchar		et_snap3;
	ushort		et_prot;	/* 802 protocol			*/
} Ethernet_t;

typedef struct {
	uchar		vet_dest[6];	/* Destination node		*/
	uchar		vet_src[6];	/* Source node			*/
	ushort		vet_vlan_type;	/* PROT_VLAN			*/
	ushort		vet_tag;	/* TAG of VLAN			*/
	ushort		vet_type;	/* protocol type		*/
} VLAN_Ethernet_t;

typedef struct icmphdr {
	uchar		type;
	uchar		code;
	ushort		checksum;
	union {
		struct {
			ushort	id;
			ushort	sequence;
		} echo;
		ulong	gateway;
		struct {
			ushort	__unused;
			ushort	mtu;
		} frag;
	} un;
} ICMP_t;



/*
	General and Ethernet control global variables.
*/	 
#ifdef NET_MAIN
unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,			/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,		/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,			/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,			/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,			/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,			/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,			/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,	/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,			/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,			/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,	/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,			/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,			/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

IPaddr_t				NetArpWaitPacketIP;
IPaddr_t				NetArpWaitReplyIP;
uchar	    			*NetArpWaitPacketMAC;	/* MAC address of waiting packet's destination	*/
uchar	    			*NetArpWaitTxPacket;	/* THE transmit packet			*/
int						NetArpWaitTxPacketSize;
uchar 					NetArpWaitPacketBuf[PKTSIZE_ALIGN + PKTALIGN];
ulong					NetArpWaitTimerStart;
int						NetArpWaitTry;
volatile uchar*			NetTxPacket;
volatile uchar			PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
volatile uchar 			*NetRxPackets[PKTBUFSRX]; /* Receive packets			*/
bd_t	 				sBoardDesc;

#else
extern unsigned char 	_ctype[];
extern IPaddr_t			NetArpWaitPacketIP;
extern IPaddr_t			NetArpWaitReplyIP;
extern uchar	    	*NetArpWaitPacketMAC;	/* MAC address of waiting packet's destination	*/
extern uchar	    	*NetArpWaitTxPacket;	/* THE transmit packet			*/
extern int				NetArpWaitTxPacketSize;
extern uchar 			NetArpWaitPacketBuf[];
extern ulong			NetArpWaitTimerStart;
extern int				NetArpWaitTry;
extern volatile uchar*	NetTxPacket;
extern volatile uchar	PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
extern volatile uchar 	*NetRxPackets[PKTBUFSRX]; /* Receive packets			*/
extern bd_t 			sBoardDesc;	
#endif


#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)


/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
static int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static int is_multicast_ether_addr(const u8 *addr)
{
	return (0x01 & addr[0]);
}

static void NetCopyIP(void *to, void *from)
{
	memcpy(to, from, sizeof(IPaddr_t));
}

/* return IP *in network byteorder* */
static IPaddr_t NetReadIP(void *from)
{
	IPaddr_t ip;
	memcpy((void*)&ip, from, sizeof(ip));
	return ip;
}


void
eth_halt(void);







#endif
