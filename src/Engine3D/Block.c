#include "Block.h"
#include "Camera.h"
#include "World.h"
#include "../InterfaceSystem/Sprite.h"

BlockConfig* AllBlockConfig = NULL;

int MAX_ID_BLOCK = 0;


List* SpriteAnimationList = NULL;
PC_Rect* SpriteAnimationListOffset = NULL;//list of offset for each sprite in SpriteAnimationList
void AddSpriteAnimation(Sprite* sprite)
{
    if (SpriteAnimationList->size == 0){
        SpriteAnimationListOffset = (PC_Rect*)malloc(sizeof(PC_Rect));
    }
    else{
        SpriteAnimationListOffset = (PC_Rect*)realloc(SpriteAnimationListOffset, sizeof(PC_Rect)*(SpriteAnimationList->size+1));
    }
    List_add(SpriteAnimationList, sprite);
    SpriteAnimationListOffset[SpriteAnimationList->size-1] = sprite->rect;
}

void UpdateSpriteAnimation()
{
    int count = 0;
    for(List_node* cur = NULL; ForEach(SpriteAnimationList,&cur);){
        Sprite* sprite = (Sprite*)cur->data;
        sprite->rect.x += 2;
        if (sprite->rect.x >= SpriteAnimationListOffset[count].w + SpriteAnimationListOffset[count].x){
            sprite->rect.x = SpriteAnimationListOffset[count].x;
        }
        count++;
    }
}

void InitBlockConfig(int MAX_BLOCK)
{
    AllBlockConfig = (BlockConfig*)malloc(sizeof(BlockConfig)*MAX_BLOCK);
    MAX_ID_BLOCK = MAX_BLOCK;
    SpriteAnimationList = List_new();
    for (size_t i = 0; i < MAX_BLOCK; i++)
    {
        AllBlockConfig[i].sprites = NULL;
        AllBlockConfig[i].MaxVariation = 0;        
        AllBlockConfig[i].collider = NULL;
        AllBlockConfig[i].type = BLOCK_TYPE_SOLID;
        AllBlockConfig[i].RenderBlock = __DrawBlock_AllFaces;
        AllBlockConfig[i].data = NULL;
    }
}

void FreeBlockConfig()
{
    for (size_t i = 0; i < MAX_ID_BLOCK; i++)
    {
        for (size_t j = 0; j < AllBlockConfig[i].MaxVariation; j++)
        {
            free(AllBlockConfig[i].sprites[j]);
        }
        free(AllBlockConfig[i].sprites);        
    }
    free(AllBlockConfig);
    free(SpriteAnimationListOffset);
    List_clear(SpriteAnimationList);
    List_free(SpriteAnimationList);
}


Block* CreateBlock(int id,int variation, int x, int y, int z)
{
    Block* block = (Block*)malloc(sizeof(Block));
    block->id = id;
    block->x = x;
    block->y = y;
    block->z = z;
    ShowSideBlock(block, 1, 1, 1, 1, 1,0);
    for (size_t i = 0; i < 6; i++)
    {
        block->Shadows[i] = PC_WHITE;
    }
    block->variation = variation;
    block->Shadows[SIDE_LEFT] = ShadowColor;
    block->Shadows[SIDE_BOTTOM] = ShadowColor;
    
    return block;
}

void FreeBlock(Block* block)
{
    free(block);
}

void ShowSideBlock(Block* block, int front, int left, int right, int top, int bottom, int back)
{
    //lock values to 0 or 1
    front = front > 0 ? 1 : 0;
    left = left > 0 ? 1 : 0;
    right = right > 0 ? 1 : 0;
    top = top > 0 ? 1 : 0;
    bottom = bottom > 0 ? 1 : 0;
    back = back > 0 ? 1 : 0;
    //set ShowSide attribute with the given values (1 or 0 for each side in bit)
    block->ShowSide = front + left*2 + right*4 + top*8 + bottom*16 + back*32;
}

void SetShadow(Block* block, int topShadow)
{
    //if topShadow is 1, set ShadowColor else set PC_WHITE
    block->Shadows[SIDE_TOP] = topShadow ? ShadowColor : PC_WHITE;
}


BlockConfig* ConfigBlock(int id, Collider* collider, BlockType type, void (*RenderBlock)(Block* block), void* data)
{
    AllBlockConfig[id].collider = collider;
    AllBlockConfig[id].type = type;
    AllBlockConfig[id].RenderBlock = RenderBlock;
    AllBlockConfig[id].data = data;
    return &AllBlockConfig[id];
}

void AddSpriteVariation(BlockConfig* blockConfig,BlockSprites* blockSprites){
    if (blockConfig->sprites == NULL)
    {
        blockConfig->sprites = (BlockSprites**)malloc(sizeof(BlockSprites*));
        blockConfig->MaxVariation = 1;
    }
    else
    {
        blockConfig->MaxVariation++;
        blockConfig->sprites = (BlockSprites**)realloc(blockConfig->sprites, sizeof(BlockSprites*)*blockConfig->MaxVariation);
    }
    blockConfig->sprites[blockConfig->MaxVariation-1] = blockSprites;
}

BlockSprites* CreateSpriteVariation(Sprite* front, Sprite* left, Sprite* right, Sprite* top, Sprite* bottom, Sprite* back)
{
    BlockSprites* blockSprites = (BlockSprites*)malloc(sizeof(BlockSprites));
    blockSprites->Front = front;
    blockSprites->Left = left;
    blockSprites->Right = right;
    blockSprites->Top = top;
    blockSprites->Bottom = bottom;
    blockSprites->Back = back;
    return blockSprites;
}


void __DrawBlock_AllFaces(Block* block)
{
    RenderVars renderVars = GetRenderVars((Vector3) {block->x,block->y,block->z});
    Sprite* sprite;
    //draw front of the block
    if (block->ShowSide & 1)
    {
        sprite = AllBlockConfig[block->id].sprites[block->variation]->Front;
        DrawZtexture(renderVars.z,
        block->Shadows[SIDE_FRONT],
        sprite->texture,
        renderVars.x,
        renderVars.y,
        sprite->rect.x,
        sprite->rect.y,
        BLOCK_SIZE,
        BLOCK_SIZE,
        renderVars.BaseScale+1,renderVars.BaseScale+1);
    }
    //draw back of the block
    if (block->ShowSide & 32)
    {
        sprite = AllBlockConfig[block->id].sprites[block->variation]->Back;
        DrawZtexture(
            renderVars.z+BLOCK_SIZE,
            block->Shadows[SIDE_BACK],
            sprite->texture,
            renderVars.x+(BLOCK_SIZE*(-renderVars.spaceX)>>16),
            renderVars.y-(BLOCK_SIZE*renderVars.spaceY>>16),
            sprite->rect.x,
            sprite->rect.y,
            BLOCK_SIZE,
            BLOCK_SIZE,
            renderVars.BaseScale,renderVars.BaseScale);
    }
    if (renderVars.spaceY!=0)
    {
        //draw top of the block
        if (!MySignbit(renderVars.spaceY))
        {
            if (block->ShowSide & 8)
            {
                sprite = AllBlockConfig[block->id].sprites[block->variation]->Top;
                const long long invSpaceY = ((1 << 31) / (renderVars.spaceY));
                const long long count_line = (BLOCK_SIZE << 16) / -invSpaceY;
                const long long ratio2 = (count_line / BLOCK_SIZE)+1;
                long long len = BLOCK_SIZE*ratio2;
                long long count = 0;
                long long j = 0;
                int scale;
                if (ratio2>0)
                {
                    //OPTIMISATION
                    while(count<len && renderVars.y - ((j * renderVars.spaceY) >> 16)>SCREEN_HEIGHT)
                    {
                        count+=ratio2;
                        j++;
                    }
                    while(len>0 && renderVars.y - (((len/ratio2) * renderVars.spaceY) >> 16)<0)
                    {
                        len-=ratio2;
                    }
                    //DRAW
                    while(count <= len)
                    {
                        scale = (BLOCK_SIZE-(((renderVars.z+j)*renderVars.ratio) >> 16)+1);
                        DrawZtexture(renderVars.z+j,//Z
                            block->Shadows[SIDE_TOP],//teinte
                            sprite->texture,//texture
                            renderVars.x + ((j * (-renderVars.spaceX)) >> 16),//x
                            renderVars.y - ((j * renderVars.spaceY) >> 16),//y
                            sprite->rect.x,//sx
                            sprite->rect.y+ (j%BLOCK_SIZE),//sy
                            BLOCK_SIZE,//sw
                            1,//sh
                            scale+1, ratio2);
                        count+=ratio2;
                        j++;
                    }
                }
            }
        }
        else //draw bottom of the block
        {
            sprite = AllBlockConfig[block->id].sprites[block->variation]->Bottom;
            if (block->ShowSide & 16)
            {
                sprite = AllBlockConfig[block->id].sprites[block->variation]->Bottom;
                long long invSpaceY = ((1 << 31) / (-renderVars.spaceY));
                long long count_line = (BLOCK_SIZE << 16) / -invSpaceY;
                long long ratio2 = (count_line / BLOCK_SIZE)+1;
                long long len = BLOCK_SIZE*ratio2;
                long long count = 0;
                long long j = 0;
                const long long y0 = renderVars.y;
                int scale;
                int y;
                if (ratio2>0)
                {
                    //OPTIMISATION
                    while(count <= len && y - ((j * renderVars.spaceY) >> 16) + BLOCK_SIZE - ratio2 >SCREEN_HEIGHT)
                    {
                        count+=ratio2;
                        j++;
                        scale = (BLOCK_SIZE-(((renderVars.z+j)*renderVars.ratio) >> 16)+1);
                        y = y0 - (BLOCK_SIZE-scale);
                    }
                    scale = (BLOCK_SIZE-(((renderVars.z+len)*renderVars.ratio) >> 16)+1);
                    y = y0 - (BLOCK_SIZE-scale);
                    while(len>0 && y - (((len/ratio2) * renderVars.spaceY) >> 16) + BLOCK_SIZE - ratio2 <0)
                    {
                        len-=ratio2;
                        scale = (BLOCK_SIZE-(((renderVars.z+len)*renderVars.ratio) >> 16)+1);
                        y = y0 - (BLOCK_SIZE-scale);
                    }
                    //DRAW
                    while(count <= len)
                    {
                        scale = (BLOCK_SIZE-(((renderVars.z+j)*renderVars.ratio) >> 16)+1);
                        y = y0 - (BLOCK_SIZE-scale);
                        DrawZtexture(renderVars.z+j,
                            block->Shadows[SIDE_BOTTOM],
                            sprite->texture, 
                            renderVars.x + ((j * (-renderVars.spaceX)) >> 16), 
                            y - ((j * renderVars.spaceY) >> 16) + BLOCK_SIZE - ratio2,
                            sprite->rect.x,
                            sprite->rect.y+ (j%BLOCK_SIZE),
                            BLOCK_SIZE,
                            1, 
                            scale+1, ratio2);
                        count+=ratio2;
                        j++;
                    }
                }
            }
        }
    }
    if (renderVars.spaceX != 0)
    {
        //draw left of the block
        if (!MySignbit(renderVars.spaceX))
        {
            if (block->ShowSide & 2)
            {
                sprite = AllBlockConfig[block->id].sprites[block->variation]->Left;
                long long invSpaceX = ((1 << 31) / (renderVars.spaceX));
                long long count_line = (BLOCK_SIZE << 16) / -invSpaceX;
                long long ratio2 = (count_line / BLOCK_SIZE)+1;
                long long len = BLOCK_SIZE*ratio2;
                long long count = 0;
                long long i = 0;
                int scale;
                if (ratio2>0)
                {
                    //OPTIMISATION
                    while(count<len && renderVars.x + ((i * (-renderVars.spaceX)) >> 16) - ratio2>SCREEN_WIDTH)
                    {
                        count+=ratio2;
                        i++;
                    }
                    //DRAW
                    while(count <= len)
                    {
                        scale = BLOCK_SIZE-(((renderVars.z+i)*renderVars.ratio) >> 16);
                        DrawZtexture(renderVars.z+i,
                            block->Shadows[SIDE_LEFT],
                            sprite->texture, 
                            renderVars.x + ((i * (-renderVars.spaceX)) >> 16)- ratio2, 
                            renderVars.y - ((i * renderVars.spaceY) >> 16), 
                            (i%BLOCK_SIZE) + sprite->rect.x, 
                            sprite->rect.y,
                            1,
                            BLOCK_SIZE, 
                            ratio2, scale);
                        count+=ratio2;
                        i++;
                    }
                }
            }
        }
        else//draw right of the block
        {
            if (block->ShowSide & 4)
            {
                sprite = AllBlockConfig[block->id].sprites[block->variation]->Right;
                long long invSpaceX = ((1 << 31) / (-renderVars.spaceX));
                long long count_line = (BLOCK_SIZE << 16) / -invSpaceX;
                long long ratio2 = (count_line / BLOCK_SIZE)+1;
                long long len = BLOCK_SIZE*ratio2;
                long long count = 0;
                long long i = 0;
                const long long x0 = renderVars.x;
                int scale;
                if (ratio2>0)
                {
                    //OPTIMISATION
                    while(len>0 && renderVars.x + (((len/ratio2) * (-renderVars.spaceX)) >> 16) + BLOCK_SIZE<0)
                    {
                        len-=ratio2;
                    }
                    //DRAW
                    while(count <= len)
                    {
                        scale = BLOCK_SIZE-(((renderVars.z+i)*renderVars.ratio) >> 16);
                        int x = x0-(BLOCK_SIZE-scale);
                        DrawZtexture(renderVars.z+i,
                            block->Shadows[SIDE_RIGHT],
                            sprite->texture,
                            x+(i*(-renderVars.spaceX)>>16)+BLOCK_SIZE - ratio2,
                            renderVars.y-(i*renderVars.spaceY>>16), 
                            (i%BLOCK_SIZE) + sprite->rect.x, 
                            sprite->rect.y,
                            1,
                            BLOCK_SIZE, 
                            ratio2+1, scale);
                        count+=ratio2;
                        i++;
                    }
                }
            }
        }
    }

}

void __DrawBlock_ZdepthRepeat(Block* block)
{

    RenderVars renderVars = GetRenderVars((Vector3) {block->x,block->y,block->z});
    long x = renderVars.firstPosition.x;
    long y = renderVars.firstPosition.y;
    long z = renderVars.z;
    const long long spaceX = renderVars.spaceX;
    const long long spaceY = renderVars.spaceY;
    const int ratio = renderVars.ratio;

    Vector3 BasePos = {x,y,z};
    Sprite* sprite = AllBlockConfig[block->id].sprites[block->variation]->Front;
    //Z Depth Drawing
    int step = (int)AllBlockConfig[block->id].data;
    for (; z <= BasePos.z + BLOCK_SIZE; z+=step)
    {
        renderVars.z = z;
        renderVars = ApplyRenderVarsZ(renderVars);
        x = renderVars.x;
        y = renderVars.y;

        const int ratio = (BLOCK_SIZE<< 16)/SCREEN_WIDTH;
        if (BLOCK_SIZE-((z*ratio) >> 16) < 0)
            return;

        const int scale = BLOCK_SIZE-((z*ratio) >> 16);
        if (scale < BLOCK_SIZE/4)
            return;
        DrawZtexture(z,block->Shadows[SIDE_FRONT],sprite->texture,x,y,sprite->rect.x + BLOCK_SIZE,sprite->rect.y,BLOCK_SIZE,BLOCK_SIZE,scale,scale+1);
    }
    
    

}

void __DrawBlock_SlopeBack(Block* block)
{
    const int cameraX = MainCamera->x;
    const int cameraY = MainCamera->y;

    int x = block->x - cameraX;
    int y = -block->y + cameraY - MainCamera->z;
    const int z = block->z + MainCamera->z;
    const int midScreenW = SCREEN_WIDTH/2;
    const int midScreenH = SCREEN_HEIGHT/2;

    const int spaceX = ((x-midScreenW) << 16) / SCREEN_WIDTH;
    const int spaceY = ((y-midScreenH) << 16) / SCREEN_WIDTH;
    x -= (z*spaceX) >> 16;
    y -= ((z*spaceY) >> 16);
    y += MainCamera->angle;

    const int ratio = (BLOCK_SIZE<< 16)/SCREEN_WIDTH;
    int scale = BLOCK_SIZE-((z*ratio) >> 16);
    if (scale < BLOCK_SIZE/4)
        return;

    int y0 = y;
    int invSpaceY = (spaceY != 0) ? ((1 << 31) / (spaceY)) : 0;
    const int shadow = BLOCK_SIZE*4*(block->ShowSide & 64? 1 : 0);

    int count_line = (BLOCK_SIZE << 16) / -invSpaceY;
    int j = 0;
    int ratio2 = count_line / BLOCK_SIZE +1;
    Sprite* sprite = AllBlockConfig[block->id].sprites[block->variation]->Front;
    while (j < BLOCK_SIZE)
    {
        scale = BLOCK_SIZE-(((z+j)*ratio) >> 16);
        for (int k = 0; k < ratio2; k++)
        {
            y = y0-(BLOCK_SIZE-j)+BLOCK_SIZE;
            DrawZtexture(z+j,block->Shadows[SIDE_TOP],sprite->texture, x + ((j * (-spaceX)) >> 16), y - ((j * spaceY) >> 16)-k, sprite->rect.x+ shadow,sprite->rect.y+ j,BLOCK_SIZE,1, scale, 1);
        }
        j += 1;
    }
    //DrawZtexture(z+BLOCK_SIZE,block->Shadows[SIDE_TOP],sprite->texture, x + (((i-step) * (-spaceX)) >> 16), y - ((i * spaceY) >> 16), sprite->rect.x+ shadow,sprite->rect.y+ scale-1,BLOCK_SIZE,1, scale, 1);
}

void __DrawBlock_SlopeFront(Block* block)
{
    const int cameraX = MainCamera->x;
    const int cameraY = MainCamera->y;

    int x = block->x - cameraX;
    int y = -block->y + cameraY - MainCamera->z;
    const int z = block->z + MainCamera->z;
    const int midScreenW = SCREEN_WIDTH/2;
    const int midScreenH = SCREEN_HEIGHT/2;


    const int spaceX = ((x-midScreenW) << 16) / SCREEN_WIDTH;
    const int spaceY = ((y-midScreenH) << 16) / SCREEN_WIDTH;
    x -= (z*spaceX) >> 16;
    y -= ((z*spaceY) >> 16);
    y += MainCamera->angle;

    const int ratio = (BLOCK_SIZE<< 16)/SCREEN_WIDTH;
    int scale = BLOCK_SIZE-((z*ratio) >> 16);
    if (scale < BLOCK_SIZE/4)
        return;

    //slope front
    int y0 = y;
    int invSpaceY = (spaceY != 0) ? ((1 << 31) / (spaceY)) : 0;
    const int shadow = BLOCK_SIZE*4*(block->ShowSide & 64? 1 : 0);

    int count_line = (BLOCK_SIZE << 16) / -invSpaceY;
    int j = 0;
    int ratio2 = count_line / BLOCK_SIZE +1;
    Sprite* sprite = AllBlockConfig[block->id].sprites[block->variation]->Front;
    while (j < BLOCK_SIZE)
    {
        scale = BLOCK_SIZE-(((z+j)*ratio) >> 16);
        for (int k = 0; k < ratio2; k++)
        {
            y = y0+(BLOCK_SIZE-j);
            DrawZtexture(z+j,block->Shadows[SIDE_TOP],sprite->texture, x + ((j * (-spaceX)) >> 16), y - ((j * spaceY) >> 16)-k, sprite->rect.x+ shadow,sprite->rect.y+ j,BLOCK_SIZE,1, scale, 1);
        }
        j += 1;
    }
    //DrawZtexture(z+BLOCK_SIZE,block->Shadows[SIDE_TOP],sprite->texture, x + (((i-step) * (-spaceX)) >> 16), y - ((i * spaceY) >> 16), sprite->rect.x+ shadow,sprite->rect.y+ scale-1,BLOCK_SIZE,1, scale, 1);

}


void DrawBlock(Block* block){
    AllBlockConfig[block->id].RenderBlock(block);
}