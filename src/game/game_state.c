#include "game_state.h"
#include "inputs.h"

#define PLAYER_SPEED 2 

enum {
  INPUTS_IDX
};

static int get_bit(uint64_t inputs, unsigned int bit) {
  if(bit >= 64) return 0;
  return (inputs >> bit) & 0b1;
}

static void player_idle_enter(Player *p);
static void player_idle_exit(Player *p);
static void player_idle_update(Player *p);
static void player_idle_apply_inputs(Player *p, uint64_t inputs);

static void player_walk_enter(Player *p);
static void player_walk_exit(Player *p);
static void player_walk_update(Player *p);
static void player_walk_apply_inputs(Player *p, uint64_t inputs);

static void player_state_enter(Player *p, PlayerState new) {
  p->state = new;
  p->frames = 0;

  switch(new) {
    case PSIdle: player_idle_enter(p); break;
    case PSWalk: player_walk_enter(p); break;
  }
}

static void player_state_exit(Player *p) {
  switch(p->state) {
    case PSIdle: player_idle_exit(p); break;
    case PSWalk: player_walk_exit(p); break;
  }
}

static void player_state_update(Player *p) {
  switch(p->state) {
    case PSIdle: player_idle_update(p); break;
    case PSWalk: player_walk_update(p); break;
  }

  p->frames++;
  p->anim_frame = p->frames / ANIM_FRAMES;
}

static void player_state_apply_inputs(Player *p, uint64_t inputs) {
  switch(p->state) {
    case PSIdle: player_idle_apply_inputs(p, inputs); break;
    case PSWalk: player_walk_apply_inputs(p, inputs); break;
  }
}

static void player_state_change(Player *p, PlayerState new) {
  if(p->state == new) return;
  player_state_exit(p);
  player_state_enter(p, new);
}

void game_state_init(GameState *gs) {
  if(!gs) return;

  gs->frame_no = 0;

  uint16_t spacing = (WORLD_W - PLAYER_W) / (MAX_PLAYERS - 1);

  for(unsigned int i = 0; i < MAX_PLAYERS; i++) {
    Player *p = &gs->players[i];

    p->x = spacing * i;
    p->y = WORLD_H - PLAYER_H;
    p->state = PSIdle;
    p->frames = 0;
    p->facing = 1;

    gs->inputs[i] = 0;
    gs->last_inputs[i] = 0;
  }
}

void game_state_set_inputs(GameState *gs, uint8_t player_id, uint64_t inputs) {
  if(!gs) return;
  if(player_id >= MAX_PLAYERS) return;
  gs->inputs[player_id] = inputs;
}

void game_state_update(GameState *gs) {
  if(!gs) return;

  for(uint8_t i = 0; i < MAX_PLAYERS; i++) {
    player_state_apply_inputs(&gs->players[i], gs->inputs[i]);
    player_state_update(&gs->players[i]);
  }

  gs->frame_no++;
}

static void player_idle_enter(Player *p) {
}

static void player_idle_exit(Player *p) {
}

static void player_idle_update(Player *p) {
}

static void player_idle_apply_inputs(Player *p, uint64_t inputs) {
  int left = get_bit(inputs, InputLeft);
  int right = get_bit(inputs, InputRight);

  if((left || right) && !(left && right)) {
    player_state_change(p, PSWalk);
    player_walk_apply_inputs(p, inputs);
  }
}

static void player_walk_enter(Player *p) {
}

static void player_walk_exit(Player *p) {
}

static void player_walk_apply_inputs(Player *p, uint64_t inputs) {
  int left = get_bit(inputs, InputLeft);
  int right = get_bit(inputs, InputRight);

  if((left && right) || !(left || right)) {
    player_state_change(p, PSIdle);
    return;
  }

  if(left && !right) p->facing = -1;
  if(right && !left) p->facing = 1;
}

static void player_walk_update(Player *p) {
  if(p->facing < 0) {
    if(p->x >= PLAYER_SPEED) p->x -= PLAYER_SPEED;
  } else if(p->facing > 0) {
    if(p->x + PLAYER_W + PLAYER_SPEED <= WORLD_W) p->x += PLAYER_SPEED;
  }
}
