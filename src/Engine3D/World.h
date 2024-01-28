#ifndef WORLD_H
#define WORLD_H
#include <stdio.h>
#include <stdlib.h>
#include "Block.h"
#include "Entity.h"

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

#include "Camera.h"

#include "../InterfaceSystem/Sprite.h"
#include <math.h>
#include <float.h>
#include <limits.h>
#include <List.h>
#include <time.h>

#if defined(DEBUG_MODE)
#define EDITOR_MODE
#endif

typedef struct StackPlaceBlock
{
    Vector3 blockPos;
    Vector3 SizeStack;
    int ID;
    int variation;
}StackPlaceBlock;

double GetTime();

double DeltaTime();

double SpeedMultiplier();

int GetFPS();

void ResetDeltaClock();

typedef struct World {
    int ID;

    int width;
    int height;
    int depth;
    Block* ***map;//[width][height][DEPTH];
    List* Entities;
    Sprite* background;
    void (*DrawWorldUI)();
    int quit;

    //General Light
    PC_Color ambientLight;
    PC_Color backgroundColor;

    //entite a retirer
    List* EntitiesToRemove;
} World;

extern World* world;


void CreateWorld(int ID,int width, int height, int depth);

void DestroyWorld();

void DrawWorld();

void PlaceBlock(Block *block, int x, int y, int z);

void RemoveBlock(int x, int y, int z);

void PlaceEntity(Entity *entity, int x, int y, int z);

void RemoveEntity(Entity* entity);

void CullingShadowWorld();

void DynamicCullingBlock(int x, int y, int z);

int SideOfBlockHit();

Block* GetBlockHit();

Entity* GetEntityHit();

int MainWorld();

void UpdateWorld();

void SaveWorldFILE(FILE* file);

void SaveWorld(char* path);

void LoadWorldFILE(FILE* file);

void LoadWorld(char* path);


#endif