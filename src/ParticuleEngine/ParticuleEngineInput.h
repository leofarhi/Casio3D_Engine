#ifndef PARTICULE_ENGINE_INPUT_H
#define PARTICULE_ENGINE_INPUT_H
#include "ParticuleEngine.h"
#include <List.h>

#if defined(WIN_MODE)  || defined(LINUX_MODE)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

extern List* InputEvents;
extern List* InputEventsHeld;

#elif defined(PSP_MODE)
#include "Libs/PSP/MyKeyboard/MyKeyboard.h"
static struct SceCtrlData padData;
static struct SceCtrlData oldData;

float InputAnalogX();
float InputAnalogY();
#elif defined(NSD_MODE)
#include <nds.h>
#include <stdio.h>
#elif defined(CG_MODE) ||defined(FX_MODE)
#include <gint/keyboard.h>
#include <MyKeyboard.h>
#include <stdarg.h>
#include <gint/rtc.h>
typedef struct 
{
    int key;
    int type;
} PC_GINT_keyinfo;

extern List* InputEvents;
#endif

extern Vector2 MousePosition;

void UpdateInputs();
//Updates the inputs

int GetInputEvent();
//Returns the first input event in the list

bool IsKeyPressed(int key);
//Checks if a key is pressed (Holding down)
//key is the key to check

bool IsKeyDown(int key);
//Checks if a key is down (Pressed once)

bool IsKeyDownWait(int key);
//Checks if a key is down (Pressed once) and waits until the key is released

bool IsKeyUp(int key);
//Checks if a key is up (Released)

bool IsMouseButtonPressed(int button);
//Checks if a mouse button is pressed (Holding down)

bool IsMouseButtonDown(int button);
//Checks if a mouse button is down (Pressed once)

bool IsMouseButtonUp(int button);
//Checks if a mouse button is up (Released)

bool PC_MouseOver(int x, int y, int w, int h);

char* PC_GetText();

#endif