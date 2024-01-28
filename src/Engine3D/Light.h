#ifndef LIGHT_ENGINE_3D_H
#define LIGHT_ENGINE_3D_H
#include "Entity.h"
#include <List.h>

typedef struct LightInfo
{
    PC_Color color;
    float intensity;
    int range;
    Vector3 lastPosition;

    List* blockAffected;//list of blocks affected by the light
} LightInfo;

Entity* SpawnLight(int x, int y, int z, PC_Color color, float intensity, int range);


#endif