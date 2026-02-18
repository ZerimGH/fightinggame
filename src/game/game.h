#ifndef GAME_H

#define GAME_H

#include "game_renderer.h"
#include "game_client.h"
#include "game_state.h"

typedef struct {
  GameRenderer renderer;
  GameClient client;

  GameState current;
  GameState last_clean;
} Game;

int game_init(Game *game);
void game_update(Game *game);
void game_render(Game *game);
void game_close(Game *game);

#endif // GAME_H
