#ifndef _UDP_COMMS_H_
#define _UDP_COMMS_H_

void udp_init();
void send_message(void* key_stroke);
void register_in_server();
void receive_state_update(player_t *player);
void receive_maze_info(pos_t *maze);

#endif
