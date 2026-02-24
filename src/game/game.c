#include "game.h"

#include <stdio.h>
#include <string.h>
#include "game_renderer.h"
#include "game_state.h"
#include "def.h"
#include "inputs.h"
#include "shared.h"

int game_init(Game *game) {
  if(!game) return 1;
  memset(game, 0, sizeof(Game));

  if(game_renderer_init(&game->renderer)) {
    PERROR("Failed to initialize game renderer.\n");
    return 1;
  }

  if(game_client_init(&game->client, "127.0.0.1", PORT)) {
    PERROR("Failed to initialize game client.\n");
    game_renderer_deinit(&game->renderer);
    return 1;
  }

  while(!game->client.started && game->client.connected) {
    game_client_process(&game->client);
  }

  if(!game->client.connected) {
    PERROR("Client disconnected before the game started.\n");
    game_renderer_deinit(&game->renderer);
    game_client_close(&game->client);
    return 1;
  }

  game_state_init(&game->current);
  game_state_init(&game->last_clean);

  return 0;
}


static void set_bit(uint64_t *bits, unsigned int idx, int state) {
  if(state) *bits |= 0b1ULL << idx;
  else *bits &= ~(0b1ULL << idx);
}


static void game_send_inputs(Game *game) {
  static const int keys[] = { INPUTS_KEY };
  unsigned int count = sizeof(keys)/sizeof(keys[0]);
  uint64_t inputs = 0;

  for(unsigned int i = 0; i < count; i++) {
    int state = IsKeyDown(keys[i]);
    set_bit(&inputs, i, state);
  }

  uint64_t frame = game->current.frame_no;
  game_client_send_input(&game->client, frame, inputs);
}

static void advance_clean(Game *game) {
  GameState state = game->last_clean;

  while(1) {
    GameState local = state;
    int stat = 0;

    for(int i = 0; i < MAX_PLAYERS; i++) {
      uint64_t inputs;
      if(game_client_get_input(&game->client, i, local.frame_no, &inputs)) {
        stat = 1;
        break;
      }
      game_state_set_inputs(&local, i, inputs);
    }

    if(stat) break;

    game_state_update(&local);
    state = local;
  }

  game->last_clean = state;
}

static void advance_current(Game *game) {
  GameState state = game->last_clean;

  while(state.frame_no <= game->current.frame_no) {
    GameState local = state;

    for(int i = 0; i < MAX_PLAYERS; i++) {
      uint64_t inputs;
      if(game_client_get_input(&game->client, i, local.frame_no, &inputs)) {
        inputs = state.inputs[i];
      }
      game_state_set_inputs(&local, i, inputs);
    }

    game_state_update(&local);
    state = local;
  }

  game->current = state;
}

void game_update(Game *game) {
  if(!game || !game->client.connected || !game->client.started) return;

  game_send_inputs(game);
  game_client_process(&game->client);
  if(!game->client.connected) return;

  advance_clean(game);
  advance_current(game);
}


void game_render(Game *game) {
  if(!game) return;
  game_renderer_render_state(&game->renderer, &game->current);
}


void game_close(Game *game) {
  if(!game) return;
  game_renderer_deinit(&game->renderer);
  game_client_close(&game->client);
}
