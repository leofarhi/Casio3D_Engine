#ifndef GAME_RESSOURCES_H
#define GAME_RESSOURCES_H

#include "../Engine3D/Block.h"

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

#include "../InterfaceSystem/Sprite.h"

#include "../Engine3D/Camera.h"
#include "../Engine3D/Collider.h"

#include <stdio.h>
#include <math.h>
#include <float.h>

#if defined(CG_MODE) || defined(FX_MODE)
#include <gint/display.h>
extern font_t uf8x9;
#endif

typedef enum BlockID
{
    GRASS_1,
    DIRT,
    BRICK_1,
    LAVA,
    WATER,
    CLIFF,
    GRASS_2,
    MIASME,
    TREE_TOP,
    TREE_TRUNK,
    STONE,
    WOOD_1,
    WOOD_2,



    BLOCK_ID_SIZE
}BlockID;

#define AllTexturesSize 10
#define AllSpritesSize 90

extern PC_Texture* AllTextures[AllTexturesSize];
extern Sprite* AllSprites[AllSpritesSize];

extern int Sprite_Block;
extern int Sprite_UI;
extern int Sprite_Player;
extern int Sprite_Ennemy;


void InitGameConfig();

void CleanGameConfig();

#endif //GAME_RESSOURCES_H