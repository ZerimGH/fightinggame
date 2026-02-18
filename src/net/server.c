#include "game_server.h"
#include "def.h"
#include <stdio.h>

int main(void) {
  while(1) { 
    GameServer gs;
    if(game_server_init(&gs)) {
      PERROR("Failed to create server.\n");
      return 1;
    }

    while(gs.running) {
      game_server_process(&gs);
    }
    game_server_close(&gs);
  }

  return 0;
}

