#include "def.h"
#include "game.h"
#include <stdio.h>
#include "raylib/raylib.h"

int main(void) {
  InitWindow(512, 288, "fightinggame");
  SetTargetFPS(FRAMERATE);

  Game game;
  if(game_init(&game)) {
    PERROR("Failed to initialize game.\n");
    return 1;
  }

  while(!WindowShouldClose() && game.client.connected) {
    game_update(&game);
    game_render(&game);
  }

  game_close(&game);

  return 0;
}
