#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "types.h"

#include "Utils.h"
#include "tests.h"


//#define DEBUG_MODE
//#define DEBUG_MODE_MAIN





static void maze_generate(position_t *maze)
{
	int i;
	
	for (i = 0; i < MAZE_SIZE; i++)
	{
		do
		{	
			
			maze[i].x =  rand() % (MAP_WIDTH);
			maze[i].y =  rand() % (MAP_HEIGHT);
			
			// leave space for maze center
		} while (  ((maze[i].x == MAP_WIDTH/2 - 1) && (maze[i].y == MAP_HEIGHT/2 - 1)) || \
				   ((maze[i].x == MAP_WIDTH/2 - 1) && (maze[i].y == MAP_HEIGHT/2    )) || \
				   ((maze[i].x == MAP_WIDTH/2 - 1) && (maze[i].y == MAP_HEIGHT/2 + 1)) || \ 
				   ((maze[i].x == MAP_WIDTH/2    ) && (maze[i].y == MAP_HEIGHT/2 - 1)) || \
				   ((maze[i].x == MAP_WIDTH/2    ) && (maze[i].y == MAP_HEIGHT/2    )) || \
				   ((maze[i].x == MAP_WIDTH/2    ) && (maze[i].y == MAP_HEIGHT/2 + 1)) || \
				   ((maze[i].x == MAP_WIDTH/2 + 1) && (maze[i].y == MAP_HEIGHT/2 - 1)) || \
				   ((maze[i].x == MAP_WIDTH/2 + 1) && (maze[i].y == MAP_HEIGHT/2    )) || \
				   ((maze[i].x == MAP_WIDTH/2 + 1) && (maze[i].y == MAP_HEIGHT/2 + 1))    );
	}
}


static void register_new_player(player_t *player, unsigned char player_id, unsigned char *station_id)
{
	if (player[player_id].active == 0)
	{
			player[player_id].active = 1;
			player[player_id].pos.x = MAP_WIDTH + (rand() % 10);
			player[player_id].pos.y = MAP_HEIGHT + (rand() % 10);
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

static void check_step_on_maze(player_t *player, unsigned char player_id, position_t *maze)
{
	int i;
	
	for (i = 0; i < MAZE_SIZE; i++)
	{
		if ( (player[player_id].pos.x == maze[i].x) &&  (player[player_id].pos.y == maze[i].y) )
		{
			player[player_id].pos.x = player[player_id].old_pos.x;
			player[player_id].pos.y = player[player_id].old_pos.y;
		}
	}
}

static int did_player_win(player_t *player, unsigned char player_id)
{
	/* If player's x,y position is in the center of the maze he wins */
	int win = 0;
			if (   ((player[player_id].pos.x == MAP_WIDTH/2 - 1) && (player[player_id].pos.y == MAP_HEIGHT/2 - 1)) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2 - 1) && (player[player_id].pos.y == MAP_HEIGHT/2    )) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2 - 1) && (player[player_id].pos.y == MAP_HEIGHT/2 + 1)) || \ 
				   ((player[player_id].pos.x == MAP_WIDTH/2    ) && (player[player_id].pos.y == MAP_HEIGHT/2 - 1)) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2    ) && (player[player_id].pos.y == MAP_HEIGHT/2    )) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2    ) && (player[player_id].pos.y == MAP_HEIGHT/2 + 1)) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2 + 1) && (player[player_id].pos.y == MAP_HEIGHT/2 - 1)) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2 + 1) && (player[player_id].pos.y == MAP_HEIGHT/2    )) || \
				   ((player[player_id].pos.x == MAP_WIDTH/2 + 1) && (player[player_id].pos.y == MAP_HEIGHT/2 + 1))    ) 
				   				   
				   {
					   win = 1;
				   }
	
	return win;
}

static void update_players_status(player_t *player, position_t *maze, unsigned char *player_id, unsigned char *action, unsigned char *station_id)
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
				player[*player_id].pos.y -= 1;
				break;

		case DOWN:
				player[*player_id].pos.y += 1;
				break;

		case LEFT:
				player[*player_id].pos.x -= 1;
				break;

		case RIGHT:
				player[*player_id].pos.x += 1;
				break;

		case START:	
				register_new_player(player, *player_id, station_id);
				send_maze_data(maze, player, *player_id);
				break;

		case END:
				//clear_player();
				//send_end_data();
				break;
	}
	
	check_step_on_maze(player, *player_id, maze);
	send_new_positions(player);
	
	if (did_player_win(player, *player_id))
	{	
		send_player_win(player, *player_id);
		maze_generate(maze);
		memset(player, 0, sizeof(player_t) * PLAYER_NUM);
	}
}

static void check_players_collisions(unsigned char *player)
{
	//Util_Printf ("checking for collisions\n");
	/* how to take into account size of object */	
}



static void game_run(player_t *player, position_t *maze)
{	

	unsigned char player_id = 0;
	unsigned char action = 0;
	unsigned char station_id[6] = {0};
	
	//check_players_collisions(player_id);
	receive_players_actions(&player_id, &action, station_id);	
	update_players_status(player, maze, &player_id, &action, station_id);
	
#ifdef DEBUG_MODE_MAIN
		Util_Printf ("GAME_RUN new station id = %02x:%02x:%02x:%02x:%02x:%02x\n",
		player[player_id].station_id[0], player[player_id].station_id[1], player[player_id].station_id[2], 
		player[player_id].station_id[3], player[player_id].station_id[4], player[player_id].station_id[5]);
#endif
}




int main ()
{
	player_t player[PLAYER_NUM] = {0};
	position_t maze[MAZE_SIZE] = {0};
						  
	maze_generate(maze);					  
						  	
	MMU_Table_Init_MMU ();	
	start_eth_device();
		
	while( Uart_Getkey() != ESC_KEY ) 
 	{
		game_run(player, maze);
	}
}




