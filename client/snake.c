#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
//#include <ncurses.h>
#include "io_utils.h"
#include "snake.h"
#include "udp_comms.h"
#include "error_hndl.h"


#define	ESC_KEY		0x1b


/* LOCAL FUNCTIONS */
static void print_board_borders(pos_t *maze)
{
	int i;
	
	for (i = 0; i < MAZE_SIZE; i++)
	{
		gotoxy(maze[i].x,maze[i].y);
		printf("X");
	}
}


/* GLOBAL FUNCTIONS */
int start_snake_game()
{
	int win = 0;
	player_t player[PLAYER_NUM] = {0};
	pos_t maze[MAZE_SIZE] = {0};
	
	pthread_t send_thread;
	pthread_t print_thread;
	//pthread_t receive_thread;

	/* set communication with server*/
   	udp_init();
   	register_in_server();

	/* receive maze information from server and print it */
	receive_maze_info(maze);
    print_board_borders(maze);    

	if (pthread_create(&send_thread, NULL, send_direction_update, NULL) != 0)
	{
		die("pthread creation failed\n");
	}
		
	if (pthread_create(&print_thread, NULL, print_new_state, player) != 0)
	{
		die("pthread creation failed\n");
	}
	
    while(!win) 
    {
		receive_state_update(player, &win);
	}
		
    return win;
}







void * print_new_state(void * player)
{
	while (1)
	{
		player_t *player_ptr = (player_t *)player;
		int i;
		for (i = 0; i < PLAYER_NUM; i++)
		{
			if (player_ptr[i].active)
			{
				gotoxy(player_ptr[i].old_pos.x, player_ptr[i].old_pos.y);
				printf(" ");
				gotoxy(player_ptr[i].pos.x, player_ptr[i].pos.y);
				printf("o");
			}
		}
		
	}	
	return NULL;

}


void * send_direction_update(void * var)
{
	direction_t get_char = 0;
	
	while(1) 
	{
		get_char = getchar();
		send_message((void*)&get_char);	
	}
	
	return NULL;
}



void end_game(int winner)
{
	int i;
	
	for (i = 0 ; i < 1000000 ; i++)
	{
		printf("game over the winning player is %d\n", winner);
	}
}
