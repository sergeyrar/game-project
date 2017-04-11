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
#define SEND_PORT 8888
#define LISTEN_PORT 9999
#define SEND_BUFLEN 1
#define RECV_BUFLEN 100

struct sockaddr_in my_addr, send_addr, recv_addr;
static int s;
static socklen_t slen=sizeof(struct sockaddr_in);


void udp_init()
{
	if ((s=socket(AF_INET, SOCK_DGRAM, 0))==-1)
	{
		die("socket failed\n");
	}
	
	/* my address information */
	memset((char *) &my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(LISTEN_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	
	// bind socket to port, listen for incoming messages on port LISTEN_PORT
	if( bind(s , (struct sockaddr*)&my_addr, sizeof(struct sockaddr_in) ) == -1)
    {
        die("bind");
    }

	/* server's address information */
	memset((char *) &send_addr, 0, sizeof(struct sockaddr_in));
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(SEND_PORT);
	send_addr.sin_addr.s_addr = inet_addr(SRV_IP);

	
}

void register_in_server()
{
	int i;
	char reg = START;


	for (i = 0; i < 3; i++)
	{
		if (sendto(s, (void*)&reg, SEND_BUFLEN, 0, (struct sockaddr *)&send_addr, slen)==-1)
		{
			die("sendto() failed");
		}
		
		printf("A 0x%02x message was sent to server\n", reg);
		usleep(100000);
	}
}

void send_message(void* key_stroke)
{

	if (sendto(s, key_stroke, SEND_BUFLEN, 0, (struct sockaddr *)&send_addr, slen)==-1)
	{
		die("sendto() failed");
	}
	printf("A message was sent for key stroke %s\n", (char*)key_stroke);
}




void receive_state_update()
{
	unsigned char buf[RECV_BUFLEN];
	
	printf("Receiveing packet\n");
	
	if ((recvfrom(s, buf, RECV_BUFLEN, 0, (struct sockaddr *) &recv_addr, &slen)) == -1)
	{
		die("recvfrom()");
	}
	printf("Received packet from %s:%d\n", inet_ntoa(recv_addr.sin_addr), ntohs(recv_addr.sin_port));
	printf("Data: %s\n" , buf);
}
