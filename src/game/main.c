#include "def.h"
#include "game.h"
#include <stdio.h>
#include <stdint.h>
#include "raylib/raylib.h"

#define DT (1.0 / FRAMERATE)

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(512, 288, "fightinggame");

  Game game;
  if(game_init(&game)) {
    PERROR("Failed to initialize game.\n");
    return 1;
  }

  double start_time = GetTime();
  double next_time = start_time;

  int c = 0;
  int lc = c;

  char debug[128];
  snprintf(debug, sizeof(debug), "Waiting...");

  while(!WindowShouldClose() && game.client.connected) {
    double cur_time = GetTime();

    if(cur_time >= next_time) {
      game_update(&game);
      next_time += DT;
      c++;
    } else {
      BeginDrawing();
      game_render(&game);
      DrawText(debug, 10, 10, 16, WHITE);
      EndDrawing();
    }

    if(c % FRAMERATE == 0 && c != lc) {
      uint64_t expected = (cur_time - start_time) * FRAMERATE;
      uint64_t cur = game.current.frame_no;
      uint64_t clean = game.last_clean.frame_no;

      snprintf(debug, sizeof(debug), "EXPECTED: %lu  CURRENT: %lu  CLEAN: %lu", expected, cur, clean);

      lc = c;
    }
  }

  game_close(&game);
  CloseWindow();
  return 0;
}
