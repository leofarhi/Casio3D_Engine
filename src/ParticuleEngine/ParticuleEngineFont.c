#include "ParticuleEngineFont.h"
#include "Resources.h"
#include <math.h>
#include <string.h>

PC_Font* MainFont = NULL;

#if defined(PSP_MODE) || defined(NDS_MODE)
PC_Font* PC_LoadFont(const char* path,int size, int w, int h,int row, int col)
#elif defined(CG_MODE) || defined(FX_MODE)
PC_Font* PC_LoadFont(const char* path)
#else
PC_Font* PC_LoadFont(const char* path, int size)
#endif
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    PC_Font* font = malloc(sizeof(PC_Font));
    font->font = TTF_OpenFont(path, size);
    if (font->font == NULL)
    {
        printf("Error loading font: %s\n", TTF_GetError());
        exit(1);
    }
    return font;
    #elif defined(PSP_MODE) || defined(NDS_MODE)
    PC_Texture* texture = PC_LoadTexture(path);
    if (texture == NULL)
        return NULL;
    PC_Font* font = malloc(sizeof(PC_Font));
    font->texture = texture;
    font->w = w;
    font->h = h;
    font->row = row;
    font->col = col;
    font->size = size;
    return font;
    #elif defined(CG_MODE) || defined(FX_MODE)
    return (PC_Font*)GetResource((unsigned char*)path);
    #endif
}

#if defined(CG_MODE) || defined(FX_MODE)
PC_Font* PC_Font_new(font_t* font)
{
    PC_Font* f = malloc(sizeof(PC_Font));
    f->font = font;
    return f;
}
#endif

void PC_FreeFont(PC_Font* font)
{
    if (font == NULL)
        return;
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    TTF_CloseFont(font->font);
    free(font);
    #elif defined(PSP_MODE) || defined(NDS_MODE)
    PC_FreeTexture(font->texture);
    free(font);
    #elif defined(CG_MODE) || defined(FX_MODE)
    free(font);
    #endif
}

void SetMainFont(PC_Font* font)
{
    MainFont = font;
    #if defined(CG_MODE) || defined(FX_MODE)
    dfont(font);
    #endif
}

void PC_DrawText(const unsigned char *text, int x, int y, PC_Color color, PC_Font* font)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
     if (font == NULL)
        return;
    SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
    //SDL_Surface* textSurface = TTF_RenderText_Blended(font->font, text, sdlColor);
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font->font, text, sdlColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect rect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &rect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    #elif defined(PSP_MODE) || defined(NDS_MODE)
     if (font == NULL)
        return;
    int t = fmax(font->h, font->w);
    int scale = font->size / t;
    int len = strlen(text);
    for(int i = 0; i < len; i++){
        char c = text[i];
        int row = c / font->col;
        int col = c % font->col;
        int u = col * font->w;
        int v = row * font->h;
        PC_DrawSubTextureSizeColored(font->texture, x + i * font->w * scale, y, u, v, font->w, font->h, font->w * scale, font->h * scale, color);
    }
    #elif defined(CG_MODE) || defined(FX_MODE)
    if (font != NULL)
        dfont(font->font);
    else
        dfont(dfont_default());
    dtext(x, y, color.color, (const char*)text);
    #endif
}

Vector2 PC_GetTextSize(const unsigned char *text, PC_Font* font)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    if (font == NULL)
        return (Vector2){0,0};
    SDL_Color sdlColor = {255, 255, 255, 255};
    //SDL_Surface* textSurface = TTF_RenderText_Blended(font->font, text, sdlColor);
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font->font, text, sdlColor);
    Vector2 size = {textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);
    return size;
    #elif defined(PSP_MODE) || defined(NDS_MODE)
    if (font == NULL)
        return (Vector2){0,0};
    int t = fmax(font->h, font->w);
    int scale = font->size / t;
    int len = strlen(text);
    return (Vector2){len * font->w * scale, font->h * scale};
    #elif defined(CG_MODE) || defined(FX_MODE)
    Vector2 size = {0,0};
    if (font == NULL)
        dsize(text,dfont_default(), &size.x, &size.y);
    else
        dsize(text,font->font, &size.x, &size.y);
    return size;
    return (Vector2){0,0};
    #endif
}