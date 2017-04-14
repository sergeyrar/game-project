//#include <ncurses.h>

#include "main.h"
#include "snake.h" 
#include "io_utils.h"


int main(int argc, char **argv)
{
    unsigned int winning_player = 0;

    init_input();
    winning_player = start_snake_game();
    end_game(winning_player);
    restore_input();
}



