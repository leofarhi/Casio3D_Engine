#ifndef DIALOGUE_SYSTEM_H
#define DIALOGUE_SYSTEM_H

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

#include "../InterfaceSystem/Sprite.h"

void Background_DrawWorld(void* data);

void Background_DrawTexture(void* data);

void Background_DrawSprite(void* data);


void FadeIn(lambdaFunction background);

void FadeOut(lambdaFunction background);

void FadeInAlpha(lambdaFunction background);

void FadeOutAlpha(lambdaFunction background);

typedef struct Dialogue{
    char* text_lines[4];
    char* name;
    int sideName;
    Sprite* face;
    Sprite* Body;
    lambdaFunction background;
}Dialogue;

Dialogue* NewDialogue(char*text_lines[4],char* name,int sideName,Sprite* face,Sprite* Body,lambdaFunction background);

void StartDialogue(Dialogue* dialogue);

typedef struct DialoguePack{
    Dialogue** dialogues;
    int size;
}DialoguePack;

DialoguePack* NewDialoguePack(Dialogue* dialogue);
void AddDialogue(DialoguePack* pack,Dialogue* dialogue);

void FreeDialoguePack(DialoguePack* pack);

void StartDialoguePack(DialoguePack* pack);



#endif