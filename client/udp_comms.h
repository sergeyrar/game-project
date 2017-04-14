#ifndef _UDP_COMMS_H_
#define _UDP_COMMS_H_


#define MAP_MSG_ID 0xaaaa
#define ACTION_MSG_ID 0xbbbb
#define WIN_MSG_ID 0xcccc
#define QUIT_MSG_ID 0xdddd


void udp_init();
void send_message(void* key_stroke);
void register_in_server();
void receive_state_update(player_t *player, int *win);
void receive_maze_info(pos_t *maze);

#endif
