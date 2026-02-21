#ifndef GAME_CLIENT_H

#define GAME_CLIENT_H

#include "shared.h"
#include "packet.h"
#include <stddef.h>

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

typedef struct {
  uint64_t inputs[MAX_ROLLBACK];
  uint64_t frames[MAX_ROLLBACK];
} InputHistory;

typedef struct {
  ENetHost *host;
  ENetPeer *peer;  

  InputHistory inputs[MAX_PLAYERS];
  int started;
  int connected;
  uint8_t num_players;
  uint8_t player_id;   
} GameClient;

int game_client_init(GameClient *gc, char *address, unsigned long port);
void game_client_process(GameClient *gc);
void game_client_close(GameClient *gc);
void game_client_send_input(GameClient *gc, uint64_t frame, uint64_t inputs);
int game_client_get_input(GameClient *gc, uint8_t player_id, uint64_t frame_no, uint64_t *res);

#endif
