#ifndef SPRITE_SHEET_MANAGER_H

#define SPRITE_SHEET_MANAGER_H

#define MAX_SPRITE_SHEETS 1024 

#include "sprite_sheet.h"

typedef struct {
  SpriteSheet sheets[MAX_SPRITE_SHEETS];
  unsigned int count;
} SpriteSheetManager;

int ssm_init(SpriteSheetManager *ssm);
int ssm_load(SpriteSheetManager *ssm, const char *ext, unsigned char *data, unsigned int len, unsigned int rows, unsigned int cols);
void ssm_render(SpriteSheetManager *ssm, int idx, float x, float y, float w, float h, unsigned int frame, int flip);
void ssm_deinit(SpriteSheetManager *ssm);

#endif // SPRITE_SHEET_MANAGER_H
