#ifndef SPRITE_H
#define SPRITE_H
#include <stdio.h>
#include <stdlib.h>

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "InterfaceSystem.h"
#include "EventDataSystem.h"



typedef struct Sprite {
    PC_Texture *texture;
    PC_Rect rect;
} Sprite;

void InitSpriteSystem();

Sprite* CreateSprite(PC_Texture *texture, PC_Rect rect);

void DrawSprite(Sprite* sprite, int x, int y);

#endif