#ifndef GAME_SERVER_H

#define GAME_SERVER_H

#include <stdint.h>
#include <stddef.h>
#include "shared.h"

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;

typedef struct {
  int connected;
  uint8_t player_id;
  uint64_t last_frame_no;
  uint64_t sent[MAX_PLAYERS];
  ENetPeer*peer;
} Player;

typedef struct {
  ENetHost *host;
  Player players[MAX_PLAYERS];
  size_t player_count;
  int running;
} GameServer;

int game_server_init(GameServer *gs);
void game_server_process(GameServer *gs);
void game_server_close(GameServer *gs);

#endif // GAME_SERVER_H
