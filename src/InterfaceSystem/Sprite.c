#include "Sprite.h"

Sprite* CreateSprite(PC_Texture *texture, PC_Rect rect)
{
    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->texture = texture;
    sprite->rect = rect;
    return sprite;
}

void DrawSprite(Sprite* sprite, int x, int y)
{
    PC_DrawSubTexture(sprite->texture, x, y, sprite->rect.x, sprite->rect.y, sprite->rect.w, sprite->rect.h);
}