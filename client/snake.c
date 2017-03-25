#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
//#include <ncurses.h>
#include "io_utils.h"
#include "snake.h"
#include "udp_comms.h"



/* LOCAL FUNCTIONS */
static void print_board_borders()
{
	int i;
	for (i=0; i<BOARD_HEIGHT; ++i)
	{
		gotoxy(0,i);
		printf("%c",LEFT_RIGHT_BORDER);
		gotoxy(BOARD_WIDTH,i);
		printf("%c",LEFT_RIGHT_BORDER);
	}

	for (i=0; i<BOARD_WIDTH; ++i)
		{
			gotoxy(i,0);
			printf("%c",UP_DOWN_BORDER);
			gotoxy(i,BOARD_HEIGHT);
			printf("%c",UP_DOWN_BORDER);
		}
}


/* GLOBAL FUNCTIONS */
int start_snake_game()
{
    int score = 0;
    snake_cell_t* snake_head = NULL;
    snake_cell_t* snake_tail = NULL;
    pos_t food = {0};
    direction_t direction = RIGHT;
    direction_t prev_direction = DOWN;

    print_board_borders();
    init_snake(&snake_head, &snake_tail);
    food = update_food(snake_head);
    print_food(food);
   	print_snake(snake_head);
   	/*new code for game*/
   	udp_init();
   	register_in_server();
    /*--------------------------*/
    while(!is_snake_alive(snake_head)) {
        usleep(1000000/SPEED);
        if (kbhit()) {
            update_direction(&direction,&prev_direction);
        }
        snake_head=advance_snake_head(snake_head, direction);
        if (did_snake_eat(snake_head, food)) {
            food = update_food(snake_head);
            print_food(food);
            score++;
        } else {
        	snake_tail=advance_snake_tail(snake_head, snake_tail);
        }
    }
  
    snake_tail = NULL;
    free_snake(&snake_head);
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


void print_food(pos_t food)
{
	gotoxy(food.x,food.y);
	printf("%c",FOOD);
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



void update_direction(direction_t* direction, direction_t* prev_direction)
{
	*direction=getchar();
	send_message((void*)direction);	
	/*
	switch(*direction){
		case UP:
			if (*prev_direction==DOWN){
				*direction=DOWN;
			}else{
				*prev_direction=*direction;
				*direction=UP;
			}
			break;
			
		case DOWN:
			if (*prev_direction==UP){
				*direction=UP;
			}else{
				*prev_direction=*direction;
				*direction=DOWN;
			}
			break;
			
		case LEFT:
			if (*prev_direction==RIGHT){
				*direction=RIGHT;
			}else{
				*prev_direction=*direction;
				*direction=LEFT;
			}
			break;
			
			
		case RIGHT:
			if (*prev_direction==LEFT){
				*direction=LEFT;
			}else{
				*prev_direction=*direction;
				*direction=RIGHT;
			}
			break;
			
		default:
			break;
	}
	*/ 	
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
