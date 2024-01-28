#ifndef ENTITY_H
#define ENTITY_H

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

#include "Camera.h"
#include "Collider.h"
#include "Block.h"

#include <stdio.h>
#include <math.h>
#include <float.h>

#include "../InterfaceSystem/Sprite.h"
#include <List.h>

typedef struct Entity Entity;

extern Sprite* shadowSprite;

typedef enum Direction
{
    DOWN,
    LEFT,
    RIGHT,
    UP
} Direction;

typedef struct Entity
{
    Sprite* sprite;
    int ID;//information facultative, sert à identifier l'entite
    int x;
    int y;
    int z;
    float ScaleSpriteWidth;
    float ScaleSpriteHeight;
    void* data;
    //pointer to function
    void (*FixedDraw)(Entity* entity);//draw before entity
    void (*Draw)(Entity* entity);//draw after entity
    void (*Update)(Entity* entity);
    void (*Free)(Entity* entity);
    Collider* collider;
    PC_Color color;
    void (*OnTriggerEnter)(Entity* this, Block* otherBlock, Entity* otherEntity);

    bool drawShadow;
} Entity;

Vector2 GetEntitySize(Entity* entity);

void Entity_DrawShadow(Entity* Entity);

Entity* CreateEntity(Sprite* sprite, int x, int y, int z);

void FreeEntity(Entity* entity);

void DrawEntity(Entity* entity);

Block* Entity_CheckCollisionWithWorld(Entity* entity);

Entity* Entity_CheckCollisionWithEntity(Entity* entity);

List* Entity_CheckAllCollisionsWithWorld(Entity* entity);

List* Entity_CheckAllCollisionsWithEntity(Entity* entity);

Block* Entity_GetNearestBlockCollider(Entity* entity);

Entity* Entity_GetNearestEntityCollider(Entity* entity);


#endif