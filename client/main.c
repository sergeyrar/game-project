//#include <ncurses.h>

#include "main.h"
#include "snake.h" 
#include "io_utils.h"


int main(int argc, char **argv)
{
    unsigned int score = 0;

    init_input();
    score = start_snake_game();
    end_game(score);
    restore_input();
}



