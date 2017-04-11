#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error_hndl.h"
#include "snake.h"

#define SRV_IP "10.0.0.100"
#define PORT 8888
#define SEND_BUFLEN 1
#define RECV_BUFLEN 1000

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
	int i;
	char reg = START;
	socklen_t slen=sizeof(si_other);

	for (i = 0; i < 3; i++)
	{
		if (sendto(s, (void*)&reg, SEND_BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
		{
			die("sendto() failed");
		}
		
		printf("A 0x%02x message was sent to server\n", reg);
		usleep(100000);
	}
}

void send_message(void* key_stroke)
{
	socklen_t slen=sizeof(si_other);

	if (sendto(s, key_stroke, SEND_BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
	{
		die("sendto() failed");
	}
	printf("A message was sent for key stroke %s\n", (char*)key_stroke);
}




void receive_state_update()
{
	socklen_t slen=sizeof(si_other);
	int recv_len;
	unsigned char buf[RECV_BUFLEN];
	
	printf("Receiveing packet\n");
	if ((recv_len = recvfrom(s, buf, RECV_BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
	{
		die("recvfrom()");
	}
	printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
	printf("Data: %s\n" , buf);
}
