#ifndef COLLIDER_H
#define COLLIDER_H

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

typedef struct Collider {
    int x;
    int y;
    int z;
    int w;
    int h;
    int depth;
    bool isTrigger;
} Collider;

Collider *Collider_new(int x, int y, int z, int w, int h, int depth, int isTrigger);

void Collider_delete(Collider *c);

int Collider_collides(Collider *c1, Vector3 Offset1, Collider *c2, Vector3 Offset2);



#endif