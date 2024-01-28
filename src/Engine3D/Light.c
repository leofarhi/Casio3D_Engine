#include "Light.h"
#include "World.h"


void Entity_UpdateLight(Entity* entity)
{
    LightInfo* info = entity->data;
    if (entity->x!=info->lastPosition.x
        || entity->y!=info->lastPosition.y
        || entity->z!=info->lastPosition.z)
    {
        info->lastPosition = (Vector3){entity->x, entity->y, entity->z};
        //Restituate Light on blocks
        for (List_node* cur = NULL;ForEach(info->blockAffected,&cur);)
        {
            Block* block = (Block*)cur->data;
            DynamicCullingBlock(block->x/BLOCK_SIZE, block->y/BLOCK_SIZE, block->z/BLOCK_SIZE);
        }
        List_clear(info->blockAffected);
        for (int i = 0; i < world->width; i++)
        {
            for (int j = 0; j < world->height; j++)
            {
                for (int k = 0; k < world->depth; k++)
                {
                    if (world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                    {
                        //Calculate distance
                        float distance = sqrt(pow(i*BLOCK_SIZE-entity->x,2)+pow(j*BLOCK_SIZE-entity->y,2)+pow(k*BLOCK_SIZE-entity->z,2))/(float)BLOCK_SIZE;
                        if (distance <= info->range)
                        {
                            //Calculate light intensity
                            float intensity = info->intensity*(1-distance/info->range);
                            //Calculate light color
                            PC_Color color = PC_ColorCreate(
                                min(max(info->color.r*intensity+world->ambientLight.r,0),255), 
                                min(max(info->color.g*intensity+world->ambientLight.g,0),255),
                                min(max(info->color.b*intensity+world->ambientLight.b,0),255),
                                info->color.a);
                            //Apply light
                            Block* block = world->map[i][j][k];
                            for (size_t s = 0; s < 6; s++)
                            {
                                block->Shadows[s] = color;
                            }
                            
                            List_add(info->blockAffected, block);
                        }
                    }
                }
            }
        }
        
    }
}

void Entity_FreeLight(Entity* entity)
{
    LightInfo* info = entity->data;
    if (info != NULL){
        List_clear(info->blockAffected);
        List_free(info->blockAffected);
        free(info);
    }
}


Entity* SpawnLight(int x, int y, int z, PC_Color color, float intensity, int range)
{
    Entity* e = CreateEntity(NULL, 0, 0, 0);
    PlaceEntity(e, x, y, z);
    LightInfo* info = malloc(sizeof(LightInfo));
    info->color = color;
    info->intensity = intensity;
    info->range = range;
    info->lastPosition = (Vector3){e->x-1, e->y, e->z};
    info->blockAffected = List_new();
    e->data = info;
    e->Update = Entity_UpdateLight;
    e->Free = Entity_FreeLight;
}