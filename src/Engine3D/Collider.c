#include "Collider.h"


Collider *Collider_new(int x, int y, int z, int w, int h, int depth, int isTrigger)
{
    Collider *c = (Collider *)malloc(sizeof(Collider));
    c->x = x;
    c->y = y;
    c->z = z;
    c->w = w;
    c->h = h;
    c->depth = depth;
    c->isTrigger = isTrigger;
    return c;
}

void Collider_delete(Collider *c)
{
    free(c);
}

int Collider_collides(Collider *c1, Vector3 Offset1, Collider *c2, Vector3 Offset2)
{
    if (c1->x + Offset1.x < c2->x + Offset2.x + c2->w &&
        c1->x + Offset1.x + c1->w > c2->x + Offset2.x &&
        c1->y + Offset1.y < c2->y + Offset2.y + c2->h &&
        c1->y + Offset1.y + c1->h > c2->y + Offset2.y &&
        c1->z + Offset1.z < c2->z + Offset2.z + c2->depth &&
        c1->z + Offset1.z + c1->depth > c2->z + Offset2.z)
    {
        return 1;
    }
    return 0;
}