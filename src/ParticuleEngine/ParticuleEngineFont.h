#ifndef PARTICULE_ENGINE_FONT_H
#define PARTICULE_ENGINE_FONT_H
#include "ParticuleEngine.h"
#include "ParticuleEngineDraw.h"
#include "ParticuleEngineTexture.h"
#include <string.h>
#include <math.h>

#if defined(WIN_MODE)  || defined(LINUX_MODE)
#elif defined(PSP_MODE)
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include <string.h>
#elif defined(NDS_MODE)
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "gl2d_Patched.h"
//#include "font_si.h"
//#include "uvcoord_font_si.h"
//glImage FontImages[FONT_SI_NUM_IMAGES];
#elif defined(CG_MODE)
#elif defined(FX_MODE)
#endif

typedef struct PC_Font
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    TTF_Font* font;
    #elif defined(PSP_MODE) || defined(NDS_MODE)
    PC_Texture* texture;
    int w, h;
    int row, col;
    int size;
    #elif defined(CG_MODE) || defined(FX_MODE)
    font_t* font;
    #endif
} PC_Font;

extern PC_Font* MainFont;

#if defined(PSP_MODE) || defined(NDS_MODE)
PC_Font* PC_LoadFont(const char* path, int size, int w, int h,int row, int col);
#elif defined(CG_MODE) || defined(FX_MODE)
PC_Font* PC_LoadFont(const char* path);
PC_Font* PC_Font_new(font_t* font);
#else
PC_Font* PC_LoadFont(const char* path, int size);
#endif

void PC_FreeFont(PC_Font* font);

void PC_DrawText(const unsigned char *text, int x, int y, PC_Color color, PC_Font* font);

void SetMainFont(PC_Font* font);

Vector2 PC_GetTextSize(const unsigned char *text, PC_Font* font); //tres couteux

#endif