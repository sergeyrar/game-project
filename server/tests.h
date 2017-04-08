#ifndef _TESTS_H
#define _TESTS_H

#include "types.h"


void TestReadByte ();
void TestWriteByte ();

void TestReadWord ();
void TestWriteWord ();

void TestReadDM9000_ID ();

void TestGPIO ();

void TestBdelayMilli ();
void TestBdelayMicro ();
void TestPWMBuzzer ();
void TestTimerIsr();

void SetIP();
void TestPing();
void TestPingServer();











/*----game data types----*/
typedef enum direction_e {
    LEFT = 97, RIGHT = 100, UP = 119, DOWN = 115
} direction_t;


typedef enum game_status_s {
    START = 40, END = 41
} game_status_t;


typedef struct position_s {
	unsigned int x;
	unsigned int y;
}position_t;


typedef struct player_s {
	unsigned char active;
	position_t pos;
	unsigned char size;
	direction_t current_direction;
	unsigned char station_id[6];
} player_t;
/*----------------------*/




/*----game functions----*/
void receive_players_actions(unsigned char *player_id, unsigned char *action, unsigned char *station_id);
void start_eth_device();
void send_updates(player_t* player, unsigned int num_of_players);
/*---------------------*/


#endif
