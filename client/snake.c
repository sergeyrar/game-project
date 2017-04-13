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
	int i,j;
	
	
	
	char map[40][101] =  {{"----------------------------------------------------------------------------------------------------"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"}, //10
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"}, //20
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"}, //30
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"|                                                                                                  |"},
						  {"----------------------------------------------------------------------------------------------------"}}; //40
						  

	for (i=0; i<40; ++i)
	{	
		for (j=0; j<101; ++j)
		{
		gotoxy(j+1,i+1);
		printf("%c",map[i][j]);
		}
	}
	
	
	for (i = 0; i < MAZE_SIZE; i++)
	{
		gotoxy(maze[i].x,maze[i].y);
		printf("X");
	}
}


/* GLOBAL FUNCTIONS */
int start_snake_game()
{
	player_t player[PLAYER_NUM] = {0};
	pos_t maze[MAZE_SIZE] = {0};
	
	pthread_t send_thread;
	pthread_t print_thread;
	//pthread_t receive_thread;
	
	
    int score = 0;
    snake_cell_t* snake_head = NULL;
    snake_cell_t* snake_tail = NULL;
    direction_t direction = RIGHT;


    init_snake(&snake_head, &snake_tail);
   	print_snake(snake_head);
   	
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
	
	

    while(1) {
		receive_state_update(player);
	}
	
	
	
    return score;
}




void init_snake(snake_cell_t** snake_head, snake_cell_t** snake_tail)
{
	int i;
	unsigned int init_snake_size=5;
	unsigned int init_snake_x=BOARD_WIDTH/2;
	unsigned int init_snake_y=BOARD_HEIGHT/2;
	
	snake_cell_t* snake_cell[init_snake_size];

	for (i=0 ; i<init_snake_size; ++i)
	{
		snake_cell[i]=(snake_cell_t*)malloc(sizeof(snake_cell_t));
		snake_cell[i]->pos.x=init_snake_x - i;
		snake_cell[i]->pos.y=init_snake_y;
	}
	
	for (i=init_snake_size-1 ; i>0; --i)
	{
		snake_cell[i-1]->next=snake_cell[i];
	}
	
	*snake_head=snake_cell[0];
	*snake_tail=snake_cell[init_snake_size-1];
	(*snake_tail)->next=NULL;
	
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


int is_snake_alive(snake_cell_t* snake_head)
{
	snake_cell_t* snake_cell=snake_head;
	
	if ((snake_head->pos.x==BOARD_WIDTH) || (snake_head->pos.y==BOARD_HEIGHT) || (snake_head->pos.x==1) || (snake_head->pos.y==1)){
		return 1;
	}
	
	while(snake_cell->next != NULL){
		if ((snake_head->pos.x==snake_cell->next->pos.x) && (snake_head->pos.y==snake_cell->next->pos.y)){
			return 1;
		}
		snake_cell=snake_cell->next;
	}
	
	return 0;
}



void print_snake(snake_cell_t* snake_head)
{
	
	gotoxy(snake_head->pos.x,snake_head->pos.y);
	printf("%c",SNAKE_BODY);
	

	
	while(snake_head->next != NULL){	
		snake_head=snake_head->next;
		gotoxy(snake_head->pos.x,snake_head->pos.y);
		printf("%c",SNAKE_BODY);
	}
}



void * send_direction_update(void * var)
{
	direction_t get_char = 0;
	
	while((get_char = getchar()) != ESC_KEY) 
	{
		//printf("thread is running1\n");
		send_message((void*)&get_char);	
	}
	
	return NULL;
}


snake_cell_t* advance_snake_head(snake_cell_t* snake_head, direction_t direction)
{
		snake_cell_t* snake_new_head;
		snake_new_head=(snake_cell_t*)malloc(sizeof(snake_cell_t));

		
		switch(direction){
		case UP:
			snake_new_head->pos.x=snake_head->pos.x;
			snake_new_head->pos.y=snake_head->pos.y-1;
			break;
			
		case DOWN:
			snake_new_head->pos.x=snake_head->pos.x;
			snake_new_head->pos.y=snake_head->pos.y+1;
			break;
			
		case LEFT:
			snake_new_head->pos.x=snake_head->pos.x-1;
			snake_new_head->pos.y=snake_head->pos.y;
			break;
			
			
		case RIGHT:
			snake_new_head->pos.x=snake_head->pos.x+1;
			snake_new_head->pos.y=snake_head->pos.y;
			break;
		}
		
		snake_new_head->next=snake_head;
		gotoxy(snake_new_head->pos.x,snake_new_head->pos.y);
		printf("%c",SNAKE_BODY);
		return snake_new_head;
}



int did_snake_eat(snake_cell_t* snake_head, pos_t food)
{
	if ((snake_head->pos.x==food.x)&&(snake_head->pos.y==food.y)){
		return 1;
	}	
	return 0;
}


 snake_cell_t* advance_snake_tail(snake_cell_t* snake_head, snake_cell_t* snake_tail)
{
	snake_cell_t* snake_new_tail;
	snake_cell_t* snake_cell=snake_head;
	
	while(snake_cell->next->next != NULL)
	{
		snake_cell=snake_cell->next;
	}
	
	snake_new_tail=snake_cell;
	snake_new_tail->next=NULL;
	gotoxy(snake_tail->pos.x,snake_tail->pos.y);
	printf(" ");
	free(snake_tail);
	return snake_new_tail;
}


 
void free_snake(snake_cell_t** snake_head)
{
	snake_cell_t* snake_cell;
	
	while(*snake_head){
		snake_cell=*snake_head;
		*snake_head=(*snake_head)->next;
		free(snake_cell);
	}
}


void end_game(int score)
{
	printf("game over, your score is %d",score);
}
