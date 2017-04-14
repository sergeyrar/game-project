#include <stdio.h>
  



#define PLAYER_NUM 256
#define MAZE_SIZE 1000



typedef enum direction_e {
    LEFT = 97, RIGHT = 100, UP = 119, DOWN = 115
} direction_t;

typedef enum game_status_s {
    START = 40, END = 41
} game_status_s;


typedef struct pos_s {
    unsigned int x;
    unsigned int y;
}pos_t;


typedef struct player_s {
	unsigned char active;
	pos_t pos;
	pos_t old_pos;
	unsigned char size;
	direction_t current_direction;
	unsigned char player_id;
} player_t;
   


int start_snake_game();
void * print_new_state(void * player);
void * send_direction_update(void * var);
void end_game(int);
