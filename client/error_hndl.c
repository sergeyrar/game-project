#include <stdlib.h>
#include <stdio.h>
#include "io_utils.h"



void die(char *s)
{
   perror(s);
   restore_input();
   exit(1);
}
