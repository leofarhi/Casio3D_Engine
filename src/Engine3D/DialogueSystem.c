#include "DialogueSystem.h"
#include "World.h"
#include "Camera.h"

void Background_DrawWorld(void* data)
{
    DrawWorld();
}

void Background_DrawTexture(void* data)
{
    PC_Texture* texture = (PC_Texture*)data;
    PC_DrawSubTextureSize(texture,0,0,0,0,GetWidth(texture),GetHeight(texture),SCREEN_WIDTH,SCREEN_HEIGHT);
}

void Background_DrawSprite(void* data)
{
    Sprite* sprite = (Sprite*)data;
    PC_DrawSubTextureSize(sprite->texture,0,0,sprite->rect.x,sprite->rect.y,sprite->rect.w,sprite->rect.h,SCREEN_WIDTH,SCREEN_HEIGHT);
}

void FadeIn(lambdaFunction background){
    int i;
    PC_Color black = PC_ColorCreate(0,0,0,255);
    for (i = SCREEN_HEIGHT/2; i > 0; i-=12)
    {
        ClearScreen();
        UpdateInputs();
        if (background.function != NULL)
            background.function(background.data);
        //vertical fermeture
        PC_DrawFillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT/2-i,black);
        PC_DrawFillRect(0,SCREEN_HEIGHT/2+i,SCREEN_WIDTH,SCREEN_HEIGHT-i,black);
        UpdateScreen();
    }
}

void FadeOut(lambdaFunction background){
    int i;
    PC_Color black = PC_ColorCreate(0,0,0,255);
    for (i = 0; i < SCREEN_HEIGHT/2; i+=12)
    {
        ClearScreen();
        UpdateInputs();
        if (background.function != NULL)
            background.function(background.data);
        //vertical ouverture
        PC_DrawFillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT/2-i,black);
        PC_DrawFillRect(0,SCREEN_HEIGHT/2+i,SCREEN_WIDTH,SCREEN_HEIGHT-i,black);
        UpdateScreen();
    }
}

void FadeInAlpha(lambdaFunction background){
    int i;
    PC_Color black = PC_ColorCreate(0,0,0,255);
    for (i = 0; i < 255; i+=40)
    {
        ClearScreen();
        UpdateInputs();
        if (background.function != NULL)
            background.function(background.data);
        #if defined(WIN_MODE)  || defined(LINUX_MODE)
        PC_DrawFillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,PC_ColorCreate(0,0,0,i));
        #elif defined(CG_MODE) || defined(FX_MODE)
        for (size_t x = 0; x < SCREEN_WIDTH*SCREEN_HEIGHT; x++)
        {

            unsigned int pixel = gint_vram[x];
            pixel = multi_color(pixel,(PC_Color){255-i,255-i,255-i,255});
            gint_vram[x] = pixel;
        }
        #endif
        UpdateScreen();
    }
    ClearScreen();
    UpdateScreen();

}

void FadeOutAlpha(lambdaFunction background){
    int i;
    PC_Color black = PC_ColorCreate(0,0,0,255);
    for (i = 0; i < 255; i+=40)
    {
        ClearScreen();
        UpdateInputs();
        if (background.function != NULL)
            background.function(background.data);
        #if defined(WIN_MODE)  || defined(LINUX_MODE)
        PC_DrawFillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,PC_ColorCreate(0,0,0,255-i));
        #elif defined(CG_MODE) || defined(FX_MODE)
        for (size_t x = 0; x < SCREEN_WIDTH*SCREEN_HEIGHT; x++)
        {

            unsigned int pixel = gint_vram[x];
            pixel = multi_color(pixel,(PC_Color){i,i,i,255});
            gint_vram[x] = pixel;
        }
        #endif
        
        UpdateScreen();
    }
}


Dialogue* NewDialogue(char*text_lines[4],char* name,int sideName,Sprite* face,Sprite* Body,lambdaFunction background)
{
    Dialogue* dialogue = (Dialogue*)malloc(sizeof(Dialogue));
    dialogue->text_lines[0] = text_lines[0];
    dialogue->text_lines[1] = text_lines[1];
    dialogue->text_lines[2] = text_lines[2];
    dialogue->text_lines[3] = text_lines[3];
    dialogue->name = name;
    dialogue->sideName = sideName;
    dialogue->face = face;
    dialogue->Body = Body;
    dialogue->background = background;
    return dialogue;
}

void StartDialogue(Dialogue* dialogue){
    //sideName = 0 -> left, 1 -> right
    PC_Color c1 = PC_ColorCreate(242,179,128,255);
    PC_Color c2 = PC_ColorCreate(118,57,41,255);
    PC_Color white = PC_ColorCreate(255,255,255,255);
    PC_Color black = PC_ColorCreate(0,0,0,255);
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    IsKeyDownWait(SDLK_a);
    #elif defined(CG_MODE) || defined(FX_MODE)
    IsKeyDownWait(KEY_EXE);
    #endif
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    while (!IsKeyDownWait(SDLK_a))
    #elif defined(CG_MODE) || defined(FX_MODE)
    while (!IsKeyDownWait(KEY_EXE))
    #endif
    {
        ClearScreen();
        UpdateInputs();
        if (dialogue->background.function != NULL)
            dialogue->background.function(dialogue->background.data);

        if (dialogue->Body != NULL)
        {
            if (dialogue->sideName == 0)
            {
                PC_DrawSubTexture(dialogue->Body->texture,0,0,dialogue->Body->rect.x,dialogue->Body->rect.y,dialogue->Body->rect.w,dialogue->Body->rect.h);
            }else
            {
                PC_DrawSubTexture(dialogue->Body->texture,SCREEN_WIDTH-dialogue->Body->rect.w,0,dialogue->Body->rect.x,dialogue->Body->rect.y,dialogue->Body->rect.w,dialogue->Body->rect.h);
            }
        }
        if (dialogue->sideName == 0)
        {
            PC_DrawFillRect(0,(SCREEN_HEIGHT/3)*2-20,SCREEN_WIDTH/2,20,c1);
            PC_DrawRect(0,(SCREEN_HEIGHT/3)*2-20,SCREEN_WIDTH/2,20,c2);
            PC_DrawText(dialogue->name,3,(SCREEN_HEIGHT/3)*2-20+3,black,MainFont);
        }
        else
        {
            PC_DrawFillRect(SCREEN_WIDTH/2,(SCREEN_HEIGHT/3)*2-20,SCREEN_WIDTH/2,20,c1);
            PC_DrawRect(SCREEN_WIDTH/2,(SCREEN_HEIGHT/3)*2-20,SCREEN_WIDTH/2,20,c2);
            PC_DrawText(dialogue->name,SCREEN_WIDTH/2+3,(SCREEN_HEIGHT/3)*2-20+3,black,MainFont);
        }

        PC_DrawFillRect(0,(SCREEN_HEIGHT/3)*2,SCREEN_WIDTH,(SCREEN_HEIGHT/3)+20,c1);
        PC_DrawRect(0,(SCREEN_HEIGHT/3)*2,SCREEN_WIDTH,(SCREEN_HEIGHT/3)+20,c2);
        if (dialogue->face != NULL)
        {
            if (dialogue->sideName == 0)
            {
                PC_DrawSubTexture(dialogue->face->texture,0,0,dialogue->face->rect.x,dialogue->face->rect.y,dialogue->face->rect.w,dialogue->face->rect.h);
            }else
            {
                PC_DrawSubTexture(dialogue->face->texture,SCREEN_WIDTH-dialogue->face->rect.w,0,dialogue->face->rect.x,dialogue->face->rect.y,dialogue->face->rect.w,dialogue->face->rect.h);
            }
        }
        if (dialogue->text_lines[0] != NULL)
            PC_DrawText(dialogue->text_lines[0],3,(SCREEN_HEIGHT/3)*2+3,black,MainFont);
        if (dialogue->text_lines[1] != NULL)
            PC_DrawText(dialogue->text_lines[1],3,(SCREEN_HEIGHT/3)*2+20+3,black,MainFont);
        if (dialogue->text_lines[2] != NULL)
            PC_DrawText(dialogue->text_lines[2],3,(SCREEN_HEIGHT/3)*2+40+3,black,MainFont);
        if (dialogue->text_lines[3] != NULL)
            PC_DrawText(dialogue->text_lines[3],3,(SCREEN_HEIGHT/3)*2+60,black,MainFont);

        UpdateScreen();
    }
    ClearScreen();
}

DialoguePack* NewDialoguePack(Dialogue* dialogue){
    DialoguePack* pack = (DialoguePack*)malloc(sizeof(DialoguePack));
    pack->dialogues = malloc(sizeof(Dialogue*));
    pack->dialogues[0] = dialogue;
    pack->size = 1;
    return pack;
}
void AddDialogue(DialoguePack* pack,Dialogue* dialogue){
    pack->size++;
    pack->dialogues = (Dialogue**)realloc(pack->dialogues,sizeof(Dialogue*)*pack->size);
    pack->dialogues[pack->size-1] = dialogue;
}

void FreeDialoguePack(DialoguePack* pack){
    int i;
    for (i = 0; i < pack->size; i++)
    {
        free(pack->dialogues[i]);
    }
    free(pack->dialogues);
    free(pack);
}

void StartDialoguePack(DialoguePack* pack){
    int i;
    for (i = 0; i < pack->size; i++)
    {
        StartDialogue(pack->dialogues[i]);
    }
}