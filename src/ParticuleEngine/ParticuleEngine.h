#ifndef PARTICULE_ENGINE_H
#define PARTICULE_ENGINE_H
#include <List.h>
#include <stddef.h>

#if defined(FX9860G)
#define FX_MODE
#elif defined(FXCG50)
#define CG_MODE
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if defined(WIN_MODE)  || defined(LINUX_MODE)
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern int delay_update;

void errx(int exitcode, const char *format, const char * error);

#elif defined(PSP_MODE)
#include "Libs/PSP/common/common-sce.h"
#include "Libs/PSP/common/callbacks.h"
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <psputility.h>

// Define PSP Width / Height
#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)


static unsigned int __attribute__((aligned(16))) PSP_list[262144];

#define SCREEN_WIDTH PSP_SCR_WIDTH // 480
#define SCREEN_HEIGHT PSP_SCR_HEIGHT // 272

#elif defined(NDS_MODE)
#include <NEMain.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gl2d_Patched.h"
#include <nds/touch.h>
#include <nds/input.h>
#include <filesystem.h>
#include <dirent.h>
extern int slcScreen;
#elif defined(N3DS_MODE)
#include <citro2d.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#elif defined(CG_MODE)
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/rtc.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/clock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libprof.h>
extern prof_t FPS;
#define SCREEN_WIDTH 396
#define SCREEN_HEIGHT 224
#elif defined(FX_MODE)
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/rtc.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/clock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libprof.h>
extern prof_t FPS;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#else
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#endif

//if not c++ compiler
#if !defined(__cplusplus)
#if !defined(bool)
typedef int bool;
#endif
#if !defined(true)
#define true 1
#define false 0
#endif
#endif

typedef struct Vector2{ //vecteur 2D
    int x;
    int y;
}Vector2;

typedef struct Vector3{ //vecteur 3D
    int x;
    int y;
    int z;
}Vector3;

typedef struct PC_Rect{
    int x;
    int y;
    int w;
    int h;
}PC_Rect;

Vector2 vector2_create(int x, int y);
Vector2* vector2_create_ptr(int x, int y);

Vector3 vector3_create(int x, int y, int z);
Vector3* vector3_create_ptr(int x, int y, int z);

float vector3_distance(Vector3 a, Vector3 b);

void PC_Init();
//Initializes the engine

void PC_Quit();
//Quits the engine

void ClearScreen();
//Clears the screen

typedef struct PC_Color PC_Color;
void ClearScreenColor(PC_Color color);
//Clears the screen with a color

void UpdateScreen();
//Updates the screen

void SelectScreen(int screen);
//Selects the screen to draw on

int GetTicks();

typedef struct lambdaFunction{
    void (*function)(void* data);
    void* data;
}lambdaFunction;

lambdaFunction lambdaFunction_create(void *function, void* data);
void lambdaFunction_call(lambdaFunction function);

void lambdaFunction_clear(lambdaFunction function);

#endif