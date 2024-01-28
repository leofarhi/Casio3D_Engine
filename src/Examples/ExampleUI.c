#include "examples.h"
#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/ParticuleEngineTexture.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"


#include "../Engine3D/Camera.h"
#include "../Engine3D/World.h"
#include "../Engine3D/DialogueSystem.h"
#include "../InterfaceSystem/Sprite.h"
#include "Ressources.h"

int MainMenu()
{
    ClearScreen();
    ClearZdepth();
    int w = AllSprites[Sprite_UI+3]->rect.w/1.5f;
    int h = AllSprites[Sprite_UI+3]->rect.h/1.5f;
    float anim = 0;
    int sprite = Sprite_UI+8;
    ClearZdepth();
    while (!(IsKeyDown(eventDataSystem->submitButton) || IsKeyDown(eventDataSystem->submitButton2)))
    {
        ClearScreen();
        UpdateInputs();

        ClearZdepth();
        DrawSprite(AllSprites[Sprite_UI],0,0);
        DrawZtexture(0,PC_WHITE,AllSprites[Sprite_UI+3]->texture,396/2-w/2,224/4-h/2,
        AllSprites[Sprite_UI+3]->rect.x,AllSprites[Sprite_UI+3]->rect.y,AllSprites[Sprite_UI+3]->rect.w,AllSprites[Sprite_UI+3]->rect.h,w,h);

        DrawSprite(AllSprites[sprite], SCREEN_WIDTH/2 - AllSprites[sprite]->rect.w/2, SCREEN_HEIGHT-AllSprites[sprite]->rect.h-10 - sin(anim)*10);
        anim+=0.2f;
        anim = fmod(anim,360);
        UpdateScreen();
    }
    while ((IsKeyDown(eventDataSystem->submitButton) || IsKeyDown(eventDataSystem->submitButton2)))
    {
        UpdateInputs();
    }

    Vector2 decalage[4]= {PC_GetTextSize("Continuer",MainFont),
                        PC_GetTextSize("Nouvelle Partie",MainFont),
                        PC_GetTextSize("Options",MainFont),
                        PC_GetTextSize("Quitter",MainFont)};

    int heights[4] = {20,40,60,80};
    int selected = 0;
    while (!(IsKeyDown(eventDataSystem->submitButton) || IsKeyDown(eventDataSystem->submitButton2)))
    {
        ClearScreen();
        UpdateInputs();
        ClearZdepth();

        DrawSprite(AllSprites[Sprite_UI],0,0);
        DrawZtexture(0,PC_WHITE,AllSprites[Sprite_UI+3]->texture,396/2-w/2,224/4-h/2,
        AllSprites[Sprite_UI+3]->rect.x,AllSprites[Sprite_UI+3]->rect.y,AllSprites[Sprite_UI+3]->rect.w,AllSprites[Sprite_UI+3]->rect.h,w,h);

        PC_DrawText("Continuer",SCREEN_WIDTH/2 - decalage[0].x/2, SCREEN_HEIGHT/2 + heights[0],PC_WHITE,MainFont);
        PC_DrawText("Nouvelle Partie",SCREEN_WIDTH/2 - decalage[1].x/2, SCREEN_HEIGHT/2 + heights[1],PC_WHITE,MainFont);
        PC_DrawText("Options",SCREEN_WIDTH/2 - decalage[2].x/2, SCREEN_HEIGHT/2 + heights[2],PC_WHITE,MainFont);
        PC_DrawText("Quitter",SCREEN_WIDTH/2 - decalage[3].x/2, SCREEN_HEIGHT/2 + heights[3],PC_WHITE,MainFont);
        PC_DrawSubTextureSize(AllSprites[Sprite_UI+12]->texture,
            SCREEN_WIDTH/2 - AllSprites[Sprite_UI+12]->rect.w/2 - decalage[selected].x/2 -10,      SCREEN_HEIGHT/2 + heights[selected],
            AllSprites[Sprite_UI+12]->rect.x,                       AllSprites[Sprite_UI+12]->rect.y,
            AllSprites[Sprite_UI+12]->rect.w,                       AllSprites[Sprite_UI+12]->rect.h,
            AllSprites[Sprite_UI+12]->rect.w/2,                      AllSprites[Sprite_UI+12]->rect.h/2);
        PC_DrawSubTextureSize(AllSprites[Sprite_UI+12]->texture,
            SCREEN_WIDTH/2 + decalage[selected].x/2 + 10,                                    SCREEN_HEIGHT/2 + heights[selected],
            AllSprites[Sprite_UI+12]->rect.x,                       AllSprites[Sprite_UI+12]->rect.y,
            AllSprites[Sprite_UI+12]->rect.w,                       AllSprites[Sprite_UI+12]->rect.h,
            -AllSprites[Sprite_UI+12]->rect.w/2,                      AllSprites[Sprite_UI+12]->rect.h/2);
        UpdateScreen();
        if (IsKeyDownWait(eventDataSystem->upButton) && selected > 0)
        {
            selected -= 1;
        }
        if (IsKeyDownWait(eventDataSystem->downButton) && selected < 3)
        {
            selected += 1;
        }
    }

    
    return selected;
}