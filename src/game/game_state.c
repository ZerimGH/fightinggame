#include "game_state.h"
#include "inputs.h"

#define PLAYER_SPEED 2 

void game_state_init(GameState *gs) {
  if(!gs) return;
  gs->frame_no = 0;
  uint16_t spacing = (WORLD_W - PLAYER_W) / (MAX_PLAYERS - 1);
  for(unsigned int i = 0; i < MAX_PLAYERS; i++) {
    gs->players[i].x = spacing * i;
    gs->players[i].y = WORLD_H - PLAYER_H;
  }
}

static int get_bit(uint64_t inputs, unsigned int bit) {
  if(bit >= 64) return 0;
  return (inputs >> bit) & 0b1;
}

enum {
  INPUTS_IDX
};

void game_state_apply_inputs(GameState *gs, uint8_t player_id, uint64_t inputs) {
  if(!gs) return;
  gs->inputs[player_id] = inputs;
  Player *p = &gs->players[player_id];
  if(get_bit(inputs, InputLeft)) {
    if(p->x >= PLAYER_SPEED) p->x -= PLAYER_SPEED;
  }

  if(get_bit(inputs, InputRight)) {
    if(p->x <= WORLD_W - PLAYER_W) p->x += PLAYER_SPEED;
  }
}
