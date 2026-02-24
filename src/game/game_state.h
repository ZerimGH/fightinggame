#ifndef GAME_STATE_H

#define GAME_STATE_H

#define WORLD_W 512 
#define WORLD_H 288

#define PLAYER_W 96 
#define PLAYER_H 63

#include <stdint.h>
#include "shared.h"

typedef enum {
  PSIdle,
  PSWalk
} PlayerState;

typedef struct {
  uint16_t x, y;

  PlayerState state;
  uint8_t frame;
  int8_t facing;
} Player;

typedef struct {
  Player players[MAX_PLAYERS];
  uint64_t inputs[MAX_PLAYERS];
  uint64_t last_inputs[MAX_PLAYERS];
  uint64_t frame_no;
} GameState;

void game_state_init(GameState *gs);
void game_state_set_inputs(GameState *gs, uint8_t player_id, uint64_t inputs);
void game_state_update(GameState *gs);

#endif
