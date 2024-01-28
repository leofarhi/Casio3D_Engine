#ifndef BLOCK_H
#define BLOCK_H

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

#include "../InterfaceSystem/Sprite.h"

#include "Camera.h"
#include "Collider.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <List.h>

#if defined(LINUX_MODE)
#define MySignbit(x) ((x) < 0)
#else
#define MySignbit(x) signbit(x)
#endif

#define BLOCK_SIZE 40
#define ShadowColor PC_ColorCreate(255/2,255/2,255/2,255)

typedef enum BlockType
{
    BLOCK_TYPE_SOLID,
    BLOCK_TYPE_TRANSPARENT,
    BLOCK_TYPE_SEMI_SOLID,

    BLOCK_TYPE_SIZE
}BlockType;

typedef struct Block Block;



extern List* SpriteAnimationList;
extern PC_Rect* SpriteAnimationListOffset;
void AddSpriteAnimation(Sprite* sprite);
void UpdateSpriteAnimation();

typedef struct BlockSprites
{
    Sprite* Top;
    Sprite* Bottom;
    Sprite* Back;
    Sprite* Front;
    Sprite* Left;
    Sprite* Right;
} BlockSprites;


typedef struct BlockConfig
{
    BlockSprites** sprites; //list of sprites variation
    int MaxVariation;

    Collider* collider;
    BlockType type;

    void (*RenderBlock)(Block*);
    void* data;
} BlockConfig;

extern BlockConfig* AllBlockConfig;

extern int MAX_ID_BLOCK;

BlockConfig* ConfigBlock(int id, Collider* collider, BlockType type, void (*RenderBlock)(Block* block), void* data);

void AddSpriteVariation(BlockConfig* blockConfig,BlockSprites* blockSprites);

BlockSprites* CreateSpriteVariation(Sprite* front, Sprite* left, Sprite* right, Sprite* top, Sprite* bottom, Sprite* back);

void InitBlockConfig(int MAX_BLOCK);

void FreeBlockConfig();

typedef enum SideBlock
{
    SIDE_TOP,
    SIDE_BOTTOM,
    SIDE_BACK,
    SIDE_FRONT,
    SIDE_LEFT,
    SIDE_RIGHT

} SideBlock;


typedef struct Block
{
    int id;
    int x;
    int y;
    int z;
    int ShowSide; //use bit to show side
    PC_Color Shadows[6];
    int variation;
    //enum TYPE_BLOCK;
} Block;


void DrawBlock(Block* block);

Block* CreateBlock(int id,int variation, int x, int y, int z);

void FreeBlock(Block* block);

void ShowSideBlock(Block* block, int front, int left, int right, int top, int bottom, int back);

void SetShadow(Block* block, int topShadow);






void __DrawBlock_AllFaces(Block* block);

void __DrawBlock_ZdepthRepeat(Block* block);

void __DrawBlock_SlopeBack(Block* block);

void __DrawBlock_SlopeFront(Block* block);


#endif