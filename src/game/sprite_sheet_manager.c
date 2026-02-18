#include "sprite_sheet_manager.h"
#include "def.h"
#include <stdio.h>

int ssm_init(SpriteSheetManager *ssm) {
  if(!ssm) return 1;
  ssm->count = 0;
  return 0;
}

int ssm_load(SpriteSheetManager *ssm, const char *ext, unsigned char *data, unsigned int len, unsigned int rows, unsigned int cols) {
  if(!ssm) return -1;
  if(ssm->count >= MAX_SPRITE_SHEETS) return -1;
  if(sprite_sheet_init(&ssm->sheets[ssm->count], ext, data, len, rows, cols)) {
    PERROR("Failed to initialize sprite sheet.\n");
    return -1;
  }
  return ssm->count++;
}

static SpriteSheet *ssm_lookup(SpriteSheetManager *ssm, int idx) {
  if(!ssm) return NULL;
  if((unsigned int) idx >= ssm->count || idx < 0) return NULL;
  return &ssm->sheets[idx];
}

void ssm_render(SpriteSheetManager *ssm, int idx, float x, float y, float w, float h, unsigned int frame) {
  if(!ssm) return;
  SpriteSheet *ss = ssm_lookup(ssm, idx);
  if(!ss) return;
  sprite_sheet_render(ss, x, y, w, h, frame);
}

void ssm_deinit(SpriteSheetManager *ssm) {
  if(!ssm) return;
  for(unsigned int i = 0; i < ssm->count; i++) {
    sprite_sheet_deinit(&ssm->sheets[i]);
  }
  ssm->count = 0;
}

