#include "game_renderer.h"
#include "assets.h"
#include "shared.h"
#include "def.h"
#include <stdio.h>

int game_renderer_init(GameRenderer *gr) {
  if(!gr) return 1;

  if(ssm_init(&gr->ssm)) {
    PERROR("Failed to initialize sprite sheet manager.\n");
    return 1;
  }

  gr->player_idle = ssm_load(&gr->ssm, ".png", idle_png, idle_png_len, 1, 4);
  if(gr->player_idle == -1) {
    PERROR("Failed to load a spritesheet.\n");
    ssm_deinit(&gr->ssm);
    return 1;
  }

  return 0;
}

void game_renderer_render_state(GameRenderer *gr, GameState *gs) {
  if (!gr || !gs) return;
  if (WindowShouldClose()) return;

  int screen_w = GetScreenWidth();
  int screen_h = GetScreenHeight();

  float w_aspect = (float)WORLD_W / (float) WORLD_H;
  float s_aspect = (float)screen_w / (float)screen_h;

  float scale_x, scale_y, offset_x, offset_y;
  offset_x = 0.f;
  offset_y = 0.f;

  float w_screen_w, w_screen_h;

  if(s_aspect > w_aspect) {
    // World is wider than window
    w_screen_h = screen_h;
    w_screen_w = screen_h * w_aspect;
    offset_x = (screen_w - w_screen_w) * 0.5f; 
  } else {
    w_screen_w = screen_w;
    w_screen_h = screen_w / w_aspect;
    offset_y = (screen_h - w_screen_h) * 0.5f;
  }
  scale_x = w_screen_w / (float) WORLD_W;
  scale_y = w_screen_h / (float) WORLD_H;

  // BeginDrawing();
  ClearBackground(GRAY);

  DrawRectangle((int)offset_x, (int)offset_y, (int)w_screen_w, (int)w_screen_h, BLACK);

  for (unsigned int i = 0; i < MAX_PLAYERS; i++) {
    Player *p = &gs->players[i];
    float rx, ry, rw, rh;
    rx = p->x * scale_x + offset_x;
    ry = p->y * scale_y + offset_y;
    rw = (float)PLAYER_W * scale_x;
    rh = (float)PLAYER_H * scale_y;
    ssm_render(&gr->ssm, gr->player_idle, rx, ry, rw, rh, 0);
  }

  // EndDrawing();
}


void game_renderer_deinit(GameRenderer *gr) {
  if(!gr) return;
  ssm_deinit(&gr->ssm);
}
