#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "types.h"

#include "Utils.h"
#include "tests.h"


//#define DEBUG_MODE
//#define DEBUG_MODE_MAIN

#define	ESC_KEY		0x1b
#define PLAYER_NUM 256
#define BOARD_WIDTH 100
#define BOARD_HEIGHT 40
#define INITIAL_SIZE 1



/*
pos_t update_food(snake_cell_t* snake_head)
{
	snake_cell_t* snake_cell=snake_head;
	pos_t food;
	food.x=(rand()%(BOARD_WIDTH-BUFFER)) + BUFFER;
	food.y=(rand()%(BOARD_HEIGHT-BUFFER)) + BUFFER;		
	
	while(snake_cell != NULL){
		if ((snake_cell->pos.x==food.x) && (snake_cell->pos.y==food.y)){
			food.x=(rand()%(BOARD_WIDTH-BUFFER)) + BUFFER;
			food.y=(rand()%(BOARD_HEIGHT-BUFFER)) + BUFFER;	
		}
		snake_cell=snake_cell->next;
	}
	return food;
}

*/


static void register_new_player(player_t *player, unsigned char player_id, unsigned char *station_id)
{
	if (player[player_id].active == 0)
	{
			player[player_id].active = 1;
			player[player_id].pos.x = rand() % BOARD_WIDTH;
			player[player_id].pos.y = rand() % BOARD_HEIGHT;
			player[player_id].old_pos.x = player[player_id].pos.x;
			player[player_id].old_pos.y = player[player_id].pos.y;
			player[player_id].size = INITIAL_SIZE;
			player[player_id].player_id = player_id;
			memcpy((void*)&player[player_id].station_id, station_id, 6);
		
#ifdef DEBUG_MODE
		Util_Printf("changed player[%d] to active = %d, old.pos.x=%u, old.pos.y=%u, size=%u\n", 
		player_id, player[player_id].active, player[player_id].old_pos.x, player[player_id].old_pos.y, player[player_id].size);
		
		Util_Printf ("inside functions new station id = %02x:%02x:%02x:%02x:%02x:%02x\n",
		player[player_id].station_id[0], player[player_id].station_id[1], player[player_id].station_id[2], 
		player[player_id].station_id[3], player[player_id].station_id[4], player[player_id].station_id[5]);
#endif
	}
			
}


static void update_players_status(player_t *player, unsigned char *player_id, unsigned char *action, unsigned char *station_id)
{

	if (*player_id ==0 && *action == 0)
	{
		//Util_Printf ("no packet received this round, exit\n");
		return;
	}
	/* save old position */
	player[*player_id].old_pos.x = player[*player_id].pos.x;
	player[*player_id].old_pos.y = player[*player_id].pos.y;
	
	switch(*action)
	{
		case UP:
				//Util_Printf ("GAME_RUN - player-id = %u , action = UP\n", player_id);
				player[*player_id].pos.y -= 1;
				break;

		case DOWN:
				//Util_Printf ("GAME_RUN - player-id = %u , action = DOWN\n", player_id);
				player[*player_id].pos.y += 1;
				break;

		case LEFT:
				//Util_Printf ("GAME_RUN - player-id = %u , action = LEFT\n", player_id);
				player[*player_id].pos.x -= 1;
				break;

		case RIGHT:
				//Util_Printf ("GAME_RUN - player-id = %u , action = RIGHT\n", player_id);
				player[*player_id].pos.x += 1;
				break;

		case START:	
				Util_Printf ("GAME_RUN - player-id = %u , action = START\n", player_id);
				register_new_player(player, *player_id, station_id);
				break;

		case END:
				//Util_Printf ("GAME_RUN - player-id = %u , action = END\n", player_id);
				break;
	}
	
	send_updates(player, PLAYER_NUM);	
}

static void check_players_collisions(unsigned char *player)
{
	//Util_Printf ("checking for collisions\n");
	/* how to take into account size of object */	
}



static void game_run(player_t *player)
{	

	unsigned char player_id = 0;
	unsigned char action = 0;
	unsigned char station_id[6] = {0};
	
	//check_players_collisions(player_id);
	receive_players_actions(&player_id, &action, station_id);	
	update_players_status(player, &player_id, &action, station_id);
	
#ifdef DEBUG_MODE_MAIN
		Util_Printf ("GAME_RUN new station id = %02x:%02x:%02x:%02x:%02x:%02x\n",
		player[player_id].station_id[0], player[player_id].station_id[1], player[player_id].station_id[2], 
		player[player_id].station_id[3], player[player_id].station_id[4], player[player_id].station_id[5]);
#endif
}


int main ()
{
	player_t player[PLAYER_NUM] = {0};
	
	MMU_Table_Init_MMU ();	
	start_eth_device();
		
	while( Uart_Getkey() != ESC_KEY ) 
 	{
		game_run(player);
	}
}




