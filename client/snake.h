#include <stdio.h>
  


#define SNAKE_BODY 'o'
#define FOOD '#'
#define UP_DOWN_BORDER '-'
#define LEFT_RIGHT_BORDER '|'

#define BOARD_WIDTH 100
#define BOARD_HEIGHT 40
#define BUFFER 2
#define SPEED 4


typedef struct pos_s {
    unsigned int x;
    unsigned int y;
}pos_t;


typedef struct snake_cell_s {
    pos_t pos;
    struct snake_cell_s* next;
//    struct snake_cell_s* prev;
} snake_cell_t;
    
typedef enum direction_e {
    LEFT = 97, RIGHT = 100, UP = 119, DOWN = 115
} direction_t;

typedef enum game_status_s {
    START = 40, END = 41
} game_status_s;


int start_snake_game();
void init_snake(snake_cell_t**,snake_cell_t**); 
pos_t update_food(snake_cell_t*);
int is_snake_alive(snake_cell_t*);
void print_food(pos_t);
void print_snake(snake_cell_t*);
void update_direction(direction_t*,direction_t*);
snake_cell_t* advance_snake_head(snake_cell_t*, direction_t);
int did_snake_eat(snake_cell_t*, pos_t);
snake_cell_t* advance_snake_tail(snake_cell_t*, snake_cell_t*);
void free_snake(snake_cell_t**);
void end_game(int);
