#include "Ressources.h"
#include "../Engine3D/Entity.h"

PC_Texture* AllTextures[AllTexturesSize];
Sprite* AllSprites[AllSpritesSize];

int CountAddSprite = 0;

int Sprite_Block;
int Sprite_UI;
int Sprite_Player;
int Sprite_Ennemy;

void AddSprite(Sprite* sprite)
{
    AllSprites[CountAddSprite] = sprite;
    CountAddSprite++;
}

void __InitSpriteSystem()
{
    for (size_t i = 0; i < AllTexturesSize; i++)
    {
        AllTextures[i] = NULL;
    }
    for (size_t i = 0; i < AllSpritesSize; i++)
    {
        AllSprites[i] = NULL;
    }
    
    AllTextures[0] = PC_LoadTexture("assets/Images/Block.png");
    AddSprite(CreateSprite(AllTextures[0], (PC_Rect){0, 0, 0, 0}));
    Sprite_Block = CountAddSprite;
    int NbCol = 3;
    for (int i = 0; i < 25; i++)
    {
        int x = (i%NbCol)*BLOCK_SIZE;
        int y = (i/NbCol)*BLOCK_SIZE;
        AddSprite(CreateSprite(AllTextures[0], (PC_Rect){x, y, BLOCK_SIZE, BLOCK_SIZE}));
    }

    AllTextures[1] = PC_LoadTexture("assets/Images/UI.png");
    Sprite_UI = CountAddSprite;
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){0, 0, 396, 224})); //Background Main Menu
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){0, 224, 396, 72})); //Hornement Main Menu Horizontal
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){713, 0, 72, 296})); //Hornement Main Menu Vertical
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){397, 0, 316, 197})); //Logo Main Menu
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){399, 202, 34, 39})); //Touche F1
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){434, 202, 34, 39})); //Touche F6
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){471, 202, 78, 38})); //Touche SHIFT
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){551, 202, 78, 38})); //Touche ALPHA
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){632, 208, 39, 24})); //Touche EXE
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){673, 203, 39, 39})); //Touche CROSS
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){399, 245, 29, 34})); //Touche INTEROGATION
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){434, 245, 34, 30})); //Touche HEART
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){398, 280, 43, 16})); //Touche Select Button

    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){605, 242, 54, 54})); //Link Face
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){659, 242, 54, 54})); //Zelda Face

    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){468, 245, 137, 26})); //Game Over

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            AddSprite(CreateSprite(AllTextures[1], (PC_Rect){785 + i*40, j*40, 40, 40}));//Powers
        }
        
    }
    //printf("CountAddSprite : %d, Sprite_UI : %d\n", CountAddSprite, Sprite_UI);
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){865, 243, 40, 35}));//Arrow Up = 42
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){845, 246, 18, 33}));//Money = 43
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){907, 1, 33, 33}));//MapUI Sky = 44
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){907, 34, 33, 33}));//MapUI Overworld = 45
    AddSprite(CreateSprite(AllTextures[1], (PC_Rect){907, 67, 33, 33}));//MapUI Underworld = 46
    

    AllTextures[2] = PC_LoadTexture("assets/Images/Shadow.png");
    shadowSprite = CreateSprite(AllTextures[2], (PC_Rect){0, 0, 40, 40});

    AllTextures[3] = PC_LoadTexture("assets/Images/Players.png");
    Sprite_Player = CountAddSprite;
    AddSprite(CreateSprite(AllTextures[3], (PC_Rect){0, 80*3, 80*20, 80*3})); //Player 1
    AddSprite(CreateSprite(AllTextures[3], (PC_Rect){0, 0, 80*20, 80*3})); //Player 2
}



void InitGameConfig()
{
    #if defined(WIN_MODE) || defined(LINUX_MODE)
    SetMainFont(PC_LoadFont("assets/Fonts/Calibri.ttf", 16));
    #elif defined(CG_MODE) || defined(FX_MODE)
    SetMainFont(PC_Font_new(&uf8x9));
    #endif
    __InitSpriteSystem();

    //InitController(); //from Original Game -> removed for this demo

    InitBlockConfig(BLOCK_ID_SIZE);

    AddSpriteAnimation(AllSprites[Sprite_Block+4]);//Lava
    AddSpriteAnimation(AllSprites[Sprite_Block+7]);//Water

    //Collider_new(int x, int y, int z, int w, int h, int depth);
    Collider* fillBlockCollider = Collider_new(0, 0, 0, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE,false);

    BlockConfig* grassBlock = ConfigBlock(GRASS_1, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(grassBlock, CreateSpriteVariation(
        AllSprites[Sprite_Block+1],//FRONT
        AllSprites[Sprite_Block+1],//LEFT
        AllSprites[Sprite_Block+1],//RIGHT
        AllSprites[Sprite_Block+0],//TOP
        AllSprites[Sprite_Block+2],//BOTTOM
        AllSprites[Sprite_Block+1]//BACK
        ));
    BlockConfig* dirtBlock = ConfigBlock(DIRT, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(dirtBlock, CreateSpriteVariation(
        AllSprites[Sprite_Block+2],//FRONT
        AllSprites[Sprite_Block+2],//LEFT
        AllSprites[Sprite_Block+2],//RIGHT
        AllSprites[Sprite_Block+2],//TOP
        AllSprites[Sprite_Block+2],//BOTTOM
        AllSprites[Sprite_Block+2]//BACK
        ));
    BlockConfig* brick_1_Block = ConfigBlock(BRICK_1, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(brick_1_Block, CreateSpriteVariation(
        AllSprites[Sprite_Block+6],//FRONT
        AllSprites[Sprite_Block+6],//LEFT
        AllSprites[Sprite_Block+6],//RIGHT
        AllSprites[Sprite_Block+6],//TOP
        AllSprites[Sprite_Block+6],//BOTTOM
        AllSprites[Sprite_Block+6]//BACK
        ));
    BlockConfig* LavaBlock = ConfigBlock(LAVA, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(LavaBlock, CreateSpriteVariation(
        AllSprites[Sprite_Block+4],//FRONT
        AllSprites[Sprite_Block+4],//LEFT
        AllSprites[Sprite_Block+4],//RIGHT
        AllSprites[Sprite_Block+4],//TOP
        AllSprites[Sprite_Block+4],//BOTTOM
        AllSprites[Sprite_Block+4]//BACK
        ));
    BlockConfig* WaterBlock = ConfigBlock(WATER, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(WaterBlock, CreateSpriteVariation(
        AllSprites[Sprite_Block+7],//FRONT
        AllSprites[Sprite_Block+7],//LEFT
        AllSprites[Sprite_Block+7],//RIGHT
        AllSprites[Sprite_Block+7],//TOP
        AllSprites[Sprite_Block+7],//BOTTOM
        AllSprites[Sprite_Block+7]//BACK
        ));
    BlockConfig* CliffBlock = ConfigBlock(CLIFF, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(CliffBlock, CreateSpriteVariation(
        AllSprites[Sprite_Block+13],//FRONT
        AllSprites[Sprite_Block+13],//LEFT
        AllSprites[Sprite_Block+13],//RIGHT
        AllSprites[Sprite_Block+12],//TOP
        AllSprites[Sprite_Block+13],//BOTTOM
        AllSprites[Sprite_Block+13]//BACK
        ));
    BlockConfig* grass2 = ConfigBlock(GRASS_2, NULL, BLOCK_TYPE_SEMI_SOLID, __DrawBlock_ZdepthRepeat, (void*)12);
    AddSpriteVariation(grass2, CreateSpriteVariation(
        AllSprites[Sprite_Block+18],//FRONT
        AllSprites[Sprite_Block+18],//LEFT
        AllSprites[Sprite_Block+18],//RIGHT
        AllSprites[Sprite_Block+18],//TOP
        AllSprites[Sprite_Block+18],//BOTTOM
        AllSprites[Sprite_Block+18]//BACK
        ));
    BlockConfig* miasme = ConfigBlock(MIASME, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(miasme, CreateSpriteVariation(
        AllSprites[Sprite_Block+10],//FRONT
        AllSprites[Sprite_Block+10],//LEFT
        AllSprites[Sprite_Block+10],//RIGHT
        AllSprites[Sprite_Block+10],//TOP
        AllSprites[Sprite_Block+10],//BOTTOM
        AllSprites[Sprite_Block+10]//BACK
        ));
    BlockConfig* tree_top = ConfigBlock(TREE_TOP, fillBlockCollider, BLOCK_TYPE_SEMI_SOLID, __DrawBlock_ZdepthRepeat, (void*)3);
    AddSpriteVariation(tree_top, CreateSpriteVariation(
        AllSprites[Sprite_Block+19],//FRONT
        AllSprites[Sprite_Block+19],//LEFT
        AllSprites[Sprite_Block+19],//RIGHT
        AllSprites[Sprite_Block+19],//TOP
        AllSprites[Sprite_Block+19],//BOTTOM
        AllSprites[Sprite_Block+19]//BACK
        ));
    BlockConfig* tree_trunk = ConfigBlock(TREE_TRUNK, fillBlockCollider, BLOCK_TYPE_SEMI_SOLID, __DrawBlock_ZdepthRepeat, (void*)3);
    AddSpriteVariation(tree_trunk, CreateSpriteVariation(
        AllSprites[Sprite_Block+22],//FRONT
        AllSprites[Sprite_Block+22],//LEFT
        AllSprites[Sprite_Block+22],//RIGHT
        AllSprites[Sprite_Block+22],//TOP
        AllSprites[Sprite_Block+22],//BOTTOM
        AllSprites[Sprite_Block+22]//BACK
        ));
    BlockConfig* stone = ConfigBlock(STONE, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(stone, CreateSpriteVariation(
        AllSprites[Sprite_Block+9],//FRONT
        AllSprites[Sprite_Block+9],//LEFT
        AllSprites[Sprite_Block+9],//RIGHT
        AllSprites[Sprite_Block+9],//TOP
        AllSprites[Sprite_Block+9],//BOTTOM
        AllSprites[Sprite_Block+9]//BACK
        ));
    BlockConfig* wood_1 = ConfigBlock(WOOD_1, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(wood_1, CreateSpriteVariation(
        AllSprites[Sprite_Block+16],//FRONT
        AllSprites[Sprite_Block+16],//LEFT
        AllSprites[Sprite_Block+16],//RIGHT
        AllSprites[Sprite_Block+16],//TOP
        AllSprites[Sprite_Block+16],//BOTTOM
        AllSprites[Sprite_Block+16]//BACK
        ));
    BlockConfig* wood_2 = ConfigBlock(WOOD_2, fillBlockCollider, BLOCK_TYPE_SOLID, __DrawBlock_AllFaces, NULL);
    AddSpriteVariation(wood_2, CreateSpriteVariation(
        AllSprites[Sprite_Block+21],//FRONT
        AllSprites[Sprite_Block+21],//LEFT
        AllSprites[Sprite_Block+21],//RIGHT
        AllSprites[Sprite_Block+21],//TOP
        AllSprites[Sprite_Block+21],//BOTTOM
        AllSprites[Sprite_Block+21]//BACK
        ));
}

void CleanGameConfig()
{
    FreeBlockConfig();
    for (size_t i = 0; i < AllSpritesSize; i++)
    {
        if (AllSprites[i] != NULL)
        {
            free(AllSprites[i]);
        }
    }
    for (size_t i = 0; i < AllTexturesSize; i++)
    {
        if (AllTextures[i] != NULL)
        {
            PC_FreeTexture(AllTextures[i]);
        }
    }
}