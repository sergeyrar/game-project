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




#define	ESC_KEY		0x1b
#define PLAYER_NUM 256
#define INITIAL_SIZE 1

#define MAZE_SIZE 1000
#define MAP_HEIGHT 40
#define MAP_WIDTH 100





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
	position_t old_pos;
	unsigned char size;
	direction_t current_direction;
	unsigned char player_id;	
	unsigned char station_id[6];
} player_t;
/*----------------------*/




/*----game functions----*/
void receive_players_actions(unsigned char *player_id, unsigned char *action, unsigned char *station_id);
void start_eth_device();
void send_updates(player_t *player, unsigned char player_id, unsigned int num_of_players , position_t *maze, unsigned char action);
/*---------------------*/


#endif
