#ifndef GAME_RENDERER_H

#define GAME_RENDERER_H

#include "sprite_sheet_manager.h"
#include "game_state.h"

typedef struct {
  SpriteSheetManager ssm;
  int player_idle;
  int player_walk;
} GameRenderer;

int game_renderer_init(GameRenderer *gr);
void game_renderer_render_state(GameRenderer *gr, GameState *gs);
void game_renderer_deinit(GameRenderer *gr);

#endif
