#include "Entity.h"
#include "Block.h"
#include "World.h"

Sprite* shadowSprite = NULL;


Entity* CreateEntity(Sprite* sprite, int x, int y, int z)
{
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    entity->ID = 0;
    entity->data = NULL;
    entity->Update = NULL;
    entity->Free = NULL;
    entity->Draw = NULL;
    entity->FixedDraw = NULL;
    entity->collider = NULL;
    entity->sprite = sprite;
    entity->x = x;
    entity->y = y;
    entity->z = z;
    entity->ScaleSpriteWidth = 1;
    entity->ScaleSpriteHeight = 1;
    entity->color = PC_WHITE;
    entity->OnTriggerEnter = NULL;
    entity->drawShadow = false;
    return entity;
}

Vector2 GetEntitySize(Entity* entity)
{
    if (entity->sprite == NULL)
    {
        return (Vector2){0,0};
    }
    return (Vector2){entity->sprite->rect.w*entity->ScaleSpriteWidth,entity->sprite->rect.h*entity->ScaleSpriteHeight};
}

void FreeEntity(Entity* entity)
{
    if (entity->collider != NULL)
    {
        Collider_delete(entity->collider);
    }
    if (entity->Free != NULL)
    {
        entity->Free(entity);
    }
    free(entity);
}

void DrawEntity(Entity* entity)
{
    if (entity->FixedDraw !=NULL)
    {
        entity->FixedDraw(entity);
    }
    if (entity->sprite != NULL)
    {

        RenderVars renderVars = GetRenderVars((Vector3) {entity->x,entity->y-BLOCK_SIZE,entity->z});
        long x = renderVars.x;
        long y = renderVars.y;
        long z = renderVars.z;
        const long long spaceX = renderVars.spaceX;
        const long long spaceY = renderVars.spaceY;

        int flipX = 1-(MySignbit(entity->sprite->rect.w)*2);
        int flipY = 1-(MySignbit(entity->sprite->rect.h)*2);

        y-=entity->sprite->rect.h;
        int scaleW = entity->sprite->rect.w-((z*(entity->sprite->rect.w<< 16)/SCREEN_WIDTH) >> 16);
        int scaleH = entity->sprite->rect.h-((z*(entity->sprite->rect.h<< 16)/SCREEN_WIDTH) >> 16);
        scaleW = scaleW*entity->ScaleSpriteWidth;
        scaleH = scaleH*entity->ScaleSpriteHeight;
        if (scaleW == 0 || scaleH == 0)
        {
            return;
        }
        DrawZtexture(z,entity->color,entity->sprite->texture, x, y + (entity->sprite->rect.h-scaleH), entity->sprite->rect.x, entity->sprite->rect.y, entity->sprite->rect.w*flipX, entity->sprite->rect.h*flipY, scaleW,scaleH);
    }
    if (entity->Draw != NULL)
    {
        entity->Draw(entity);
    }
    if (entity->drawShadow)
    {
        Entity_DrawShadow(entity);
    }
}


void Entity_DrawShadow(Entity* entity){   
    long temp_z = GetNewPositionWithRotate((Vector3){entity->x,entity->y,entity->z}).z;
    const int scaleSP = abs(entity->sprite->rect.w*entity->ScaleSpriteWidth)-((temp_z*(abs(entity->sprite->rect.w*entity->ScaleSpriteWidth)<< 16)/SCREEN_WIDTH) >> 16);
    if (scaleSP == 0)
    {
        return;
    }
    const int ind_x = min(max((entity->x+(scaleSP/2)) / BLOCK_SIZE,0),world->width-1);
    int ind_y = min(max((entity->y+5) / BLOCK_SIZE,0),world->height-1);
    const int ind_z = min(max(entity->z / BLOCK_SIZE,0),world->depth-1);
    int y =-ind_y*BLOCK_SIZE;
    while (ind_y >= 0 && world->map[ind_x][ind_y][ind_z] == NULL)
    {
        ind_y--;
    }
    if (ind_y < 0 || AllBlockConfig[world->map[ind_x][ind_y][ind_z]->id].type != BLOCK_TYPE_SOLID)
    {
        return;
    }
    y= -(ind_y+1)*BLOCK_SIZE;

    RenderVars renderVars = GetRenderVars((Vector3) {entity->x,-y-BLOCK_SIZE,entity->z});
    long x = renderVars.x;
    y = renderVars.y;
    y-=BLOCK_SIZE;
    long z = renderVars.z;
    const long long spaceX = renderVars.spaceX;
    const long long spaceY = renderVars.spaceY;
    const int ratio = renderVars.ratio;
   

    const int invSpaceY = (spaceY != 0) ? ((1 << 31) / (spaceY)) : 0;
    int i = 0;
    const int step = max(1, (invSpaceY >> 16));
    
    const int scaleW = (shadowSprite->rect.w-((z*(shadowSprite->rect.w<< 16)/SCREEN_WIDTH) >> 16))*entity->ScaleSpriteWidth;
    
    const int scaleH = (shadowSprite->rect.h-((z*(shadowSprite->rect.h<< 16)/SCREEN_WIDTH) >> 16))*entity->ScaleSpriteHeight;
    
    x += (scaleSP/2)-scaleW/2;
    DrawZtexture(z,PC_WHITE,shadowSprite->texture, x, y + (shadowSprite->rect.h-scaleH), shadowSprite->rect.x, shadowSprite->rect.y, shadowSprite->rect.w, shadowSprite->rect.h, scaleW,scaleH);
}


Block* Entity_CheckCollisionWithWorld(Entity* entity)
{
    if (entity->collider == NULL)
    {
        return NULL;
    }
    int ind_x = entity->x / BLOCK_SIZE;
    int ind_y = entity->y / BLOCK_SIZE;
    int ind_z = entity->z / BLOCK_SIZE;
    //check collision with world around 3x3x3
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            for (int k = -1; k <= 1; k++)
            {
                if (ind_x + i < 0 || ind_x + i >= world->width || ind_y + j < 0 || ind_y + j >= world->height || ind_z + k < 0 || ind_z + k >= world->depth)
                {
                    continue;
                }
                Block* block = world->map[ind_x + i][ind_y + j][ind_z + k];
                if (block != NULL)
                {
                    if (AllBlockConfig[block->id].collider!=NULL &&
                        Collider_collides(entity->collider,(Vector3){entity->x, entity->y, entity->z},
                            AllBlockConfig[block->id].collider, (Vector3){block->x, block->y, block->z}))
                    {
                        return block;
                    }
                }
            }
        }
    }
    return NULL;
}

Entity* Entity_CheckCollisionWithEntity(Entity* entity)
{
    if (entity->collider == NULL)
    {
        return NULL;
    }
    for (List_node* cur=NULL;ForEach(world->Entities,&cur);)
    {
        Entity* otherEntity = (Entity*)cur->data;
        if (otherEntity == entity)
        {
            continue;
        }
        if (otherEntity->collider != NULL)
        {
            if (Collider_collides(entity->collider,(Vector3){entity->x, entity->y, entity->z},
                otherEntity->collider, (Vector3){otherEntity->x, otherEntity->y, otherEntity->z}))
            {
                return otherEntity;
            }
        }
    }
    return NULL;
}

List* Entity_CheckAllCollisionsWithWorld(Entity* entity){
    List* collisions = List_new();
    if (entity->collider == NULL)
    {
        return collisions;
    }
    int ind_x = entity->x / BLOCK_SIZE;
    int ind_y = entity->y / BLOCK_SIZE;
    int ind_z = entity->z / BLOCK_SIZE;
    //check collision with world around 3x3x3
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            for (int k = -2; k <= 2; k++)
            {
                if (ind_x + i < 0 || ind_x + i >= world->width || ind_y + j < 0 || ind_y + j >= world->height || ind_z + k < 0 || ind_z + k >= world->depth)
                {
                    continue;
                }
                Block* block = world->map[ind_x + i][ind_y + j][ind_z + k];
                if (block != NULL)
                {
                    if (AllBlockConfig[block->id].collider!=NULL &&
                        Collider_collides(entity->collider,(Vector3){entity->x, entity->y, entity->z},
                            AllBlockConfig[block->id].collider, (Vector3){block->x, block->y, block->z}))
                    {
                        List_add(collisions,block);
                    }
                }
            }
        }
    }
    return collisions;
}

List* Entity_CheckAllCollisionsWithEntity(Entity* entity){
    List* collisions = List_new();
    if (entity->collider == NULL)
    {
        return collisions;
    }
    for (List_node* cur=NULL;ForEach(world->Entities,&cur);)
    {
        Entity* otherEntity = (Entity*)cur->data;
        if (otherEntity == entity)
        {
            continue;
        }
        if (otherEntity->collider != NULL)
        {
            if (Collider_collides(entity->collider,(Vector3){entity->x, entity->y, entity->z},
                otherEntity->collider, (Vector3){otherEntity->x, otherEntity->y, otherEntity->z}))
            {
                List_add(collisions,otherEntity);
            }
        }
    }
    return collisions;
}

Block* Entity_GetNearestBlockCollider(Entity* entity)
{
    List* Blocks = Entity_CheckAllCollisionsWithWorld(entity);
    Vector3 pos;
    if (Blocks->size>0)
    {
        Block* block = Blocks->head->data;
        pos = (Vector3){block->x,block->y,block->z};
        float distance = vector3_distance(pos,(Vector3){entity->x,entity->y,entity->z});
        for (List_node* node = NULL; ForEach(Blocks, &node);)
        {
            Block* block2 = node->data;
            //Check distance between entity and block2
            float temp = vector3_distance((Vector3){block2->x,block2->y,block2->z},(Vector3){entity->x,entity->y,entity->z});
            if (temp < distance)
            {
                distance = temp;
                pos = (Vector3){block2->x,block2->y,block2->z};
                block = block2;
            }
        }
        List_clear(Blocks);
        List_free(Blocks);
        return block;
    }
    else
    {
        return NULL;
    }
}

Entity* Entity_GetNearestEntityCollider(Entity* entity)
{
    List* Entities = Entity_CheckAllCollisionsWithEntity(entity);
    Vector3 pos;
    if (Entities->size>0)
    {
        Entity* entity2 = Entities->head->data;
        pos = (Vector3){entity2->x,entity2->y,entity2->z};
        float distance = vector3_distance(pos,(Vector3){entity->x,entity->y,entity->z});
        for (List_node* node = NULL; ForEach(Entities, &node);)
        {
            Entity* entity3 = node->data;
            //Check distance between entity and entity3
            float temp = vector3_distance((Vector3){entity3->x,entity3->y,entity3->z},(Vector3){entity->x,entity->y,entity->z});
            if (temp < distance)
            {
                distance = temp;
                pos = (Vector3){entity3->x,entity3->y,entity3->z};
                entity2 = entity3;
            }
        }
        List_clear(Entities);
        List_free(Entities);
        return entity2;
    }
    else
    {
        return NULL;
    }
}