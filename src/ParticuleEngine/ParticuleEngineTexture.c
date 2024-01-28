#include "ParticuleEngineTexture.h"
#include "ParticuleEngine.h"
#include "ParticuleEngineDraw.h"
#include "ParticuleEngineFont.h"
#include "Resources.h"
#include <stdio.h>
#include <stdlib.h>
#include <Tuple.h>

#if defined(WIN_MODE)  || defined(LINUX_MODE)
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
void free_texture(PC_Texture* tex) {
    if (tex) {
        if (tex->texture) {
            if(!tex->vram){
                free(tex->texture->data);
            }
        }
        free(tex->texture);
    }
}

#elif defined(NDS_MODE)
#include <List.h>
#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "gl2d_Patched.h"
#include <nds/touch.h>
#include <nds/input.h>
#include <filesystem.h>
#include <dirent.h>

int GetGlTextureSize(int size) {
	if (size <= 8) {
		return TEXTURE_SIZE_8;
	}
	else if (size <= 16) {
		return TEXTURE_SIZE_16;
	}
	else if (size <= 32) {
		return TEXTURE_SIZE_32;
	}
	else if (size <= 64) {
		return TEXTURE_SIZE_64;
	}
	else if (size <= 128) {
		return TEXTURE_SIZE_128;
	}
	else if (size <= 256) {
		return TEXTURE_SIZE_256;
	}
	else if (size <= 512) {
		return TEXTURE_SIZE_512;
	}
	else {
		return TEXTURE_SIZE_1024;
	}
};
#elif defined(CG_MODE) || defined(FX_MODE)
#include <gint/image.h>
#endif

PC_Color TextureColorMod;

PC_Texture* PC_LoadTexture(const char* path)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    PC_Texture* texture = malloc(sizeof(PC_Texture));
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "error IMG_Load %s", IMG_GetError());
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture->texture == NULL)
        errx(EXIT_FAILURE, "error SDL_CreateTextureFromSurface %s", SDL_GetError());
    texture->width = surface->w;
    texture->height = surface->h;
    //convert to RGBA
    texture->surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0); 
    SDL_FreeSurface(surface);
    return texture;
    #elif defined(PSP_MODE)
    Texture* texture = load_texture(path,0,GU_TRUE);
    if (texture == NULL)
        return NULL;
    PC_Texture* pc_texture = malloc(sizeof(PC_Texture));
    pc_texture->texture = texture;
    pc_texture->vram = GU_TRUE;
    pc_texture->width = texture->width;
    pc_texture->height = texture->height;
    Vector2* size = (Vector2*)GetResource((unsigned char*)path);
    if (size!= NULL)
    {
        pc_texture->width = size->x;
        pc_texture->height = size->y;
    }
    return pc_texture;
    #elif defined(NDS_MODE)
    PC_Texture* texture = (PC_Texture*)GetResource((unsigned char*)path);
    if (texture == NULL)
        return NULL;
    const unsigned int* Bitmap = texture->Bitmap;
    const unsigned short* Pal = texture->Pal;
    int width = texture->width;
    int height = texture->height;
    texture->imageData = malloc(sizeof(glImage));
    texture->TextureID = glLoadTileSet(texture->imageData, // pointer to glImage array
        width, // sprite width
        height, // sprite height
        width, // bitmap width
        height, // bitmap height
        GL_RGB256,			// texture type for glTexImage2D() in videoGL.h
        GetGlTextureSize(width), // sizeX for glTexImage2D() in videoGL.h
        GetGlTextureSize(height), // sizeY for glTexImage2D() in videoGL.h
        GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
        256,				// Length of the palette to use (16 colors)
        (u16*)Pal,		// Load our 256 color tiles palette
        (u8*)Bitmap // image data generated by GRIT
    );
    return texture;
    #elif defined(CG_MODE) || defined(FX_MODE)
    PC_Texture* texture = (PC_Texture*)GetResource((unsigned char*)path);
    if (texture->IsExternalPath!=NULL && texture->texture == NULL)
    {
        texture->texture = (bopti_image_t*)gint_world_switch(GINT_CALL(LoadExternalImage,(void*)texture->IsExternalPath));
    }
    SetupFormatTexture(texture);
    return texture;
    #endif

    return NULL;
}

#if defined(CG_MODE) || defined(FX_MODE)
void SetupFormatTexture(PC_Texture* tex){
    bopti_image_t* texture = tex->texture;
    if(IMAGE_IS_RGB16(texture->format)) {
        tex->GetPixel =GetPixel_CG_RGB16;
        tex->DecodePixel =DecodePixel_CG_RGB16;
    }
    else if(IMAGE_IS_P8(texture->format)) {
        tex->GetPixel =GetPixel_CG_P8;
        tex->DecodePixel =DecodePixel_CG_P8;
    }
    else if(IMAGE_IS_P4(texture->format)) {
        tex->GetPixel =GetPixel_CG_P4;
        tex->DecodePixel =DecodePixel_CG_P4;
    }

    tex->alpha = image_alpha(texture->format);
}

bopti_image_t* __LoadImageSprite(Tuple* args)
{
    const char* path = (const char*)Tuple_get(args,0);
    int index = (int)Tuple_get(args,1);
    return LoadImageSprite(path,index);
}

PC_Texture* PC_LoadTextureSprite(const char* path,int index)
{
    PC_Texture* ref = (PC_Texture*)GetResource((unsigned char*)path);
    PC_Texture* texture = malloc(sizeof(PC_Texture));
    texture->IsExternalPath = ref->IsExternalPath;
    texture->texture = NULL;
    Tuple* args = Tuple_create(2);
    Tuple_set(args,0,(void*)texture->IsExternalPath);
    Tuple_set(args,1,(void*)index);
    texture->texture = (bopti_image_t*)gint_world_switch(GINT_CALL(__LoadImageSprite,(void*)args));
    Tuple_free(args);
    SetupFormatTexture(texture);
    return texture;
}


bopti_image_t* __LoadExternalSubImageSize(List* args)
{
    const char* path = (const char*)List_get(args,0);
    PC_Rect* rect = (PC_Rect*)List_get(args,1);
    return LoadExternalSubImageSize(path, rect->x, rect->y, rect->w, rect->h);
}
#endif

PC_Texture* PC_LoadSubTextureSize(const char* path, int sx, int sy, int sw, int sh)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    PC_Texture* texture = PC_LoadTexture(path);
    if (texture == NULL)
        return NULL;
    SDL_Surface* surface = texture->surface;
    SDL_Surface* subSurface = SDL_CreateRGBSurfaceWithFormat(0, sw, sh, surface->format->BitsPerPixel, surface->format->format);
    if (subSurface == NULL) {
        printf("Erreur lors de la création de la sous-surface : %s\n", SDL_GetError());
        return NULL;
    }

    // Copie de la sous-surface avec conservation du canal alpha
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    SDL_SetSurfaceBlendMode(subSurface, SDL_BLENDMODE_BLEND);
    SDL_Rect srcRect = {sx, sy, sw, sh};
    SDL_BlitSurface(surface, &srcRect, subSurface, NULL);
    //Free last texture
    SDL_DestroyTexture(texture->texture);
    //Free last surface
    SDL_FreeSurface(texture->surface);
    texture->surface = subSurface;


    // Création de la texture
    texture->texture = SDL_CreateTextureFromSurface(renderer, subSurface);
    if (texture->texture == NULL)
        errx(EXIT_FAILURE, "error SDL_CreateTextureFromSurface %s", SDL_GetError());
    texture->width = subSurface->w;
    texture->height = subSurface->h;



    #elif defined(CG_MODE) || defined(FX_MODE)
    PC_Texture* texture = (PC_Texture*)GetResource((unsigned char*)path);
    if (texture->IsExternalPath!=NULL && texture->texture == NULL)
    {
        List* args = List_new();
        List_add(args, (void*)texture->IsExternalPath);
        PC_Rect* rect = malloc(sizeof(PC_Rect));
        rect->x = sx;
        rect->y = sy;
        rect->w = sw;
        rect->h = sh;
        List_add(args, (void*)rect);
        texture->texture = (bopti_image_t*)gint_world_switch(GINT_CALL(__LoadExternalSubImageSize,(void*)args));
        free(List_get(args,1));
        List_clear(args);
        List_free(args);
    }
    SetupFormatTexture(texture);
    return texture;
    #endif
}

void PC_FreeTexture(PC_Texture* texture)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    SDL_DestroyTexture(texture->texture);
    SDL_FreeSurface(texture->surface);
    free(texture);
    #elif defined(PSP_MODE)
    free_texture(texture);
    free(texture);
    #elif defined(NDS_MODE)
    free(texture->imageData);
    texture->imageData = NULL;
    texture->TextureID = -1;
    #elif defined(CG_MODE) || defined(FX_MODE)
    if (texture->IsExternalPath!=NULL)
    {
        image_free(texture->texture);
        texture->texture = NULL;
        //Check if path finish with .Sprites
        if (strstr(texture->IsExternalPath,".Sprites")!=NULL)
            free(texture);
    }
    #endif
}


void PC_DrawTexture(PC_Texture* texture, int x, int y)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    SDL_Rect rect = {x, y, texture->width, texture->height};
    SDL_RenderCopy(renderer, texture->texture, NULL, &rect);
    #elif defined(PSP_MODE)
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, texture->vram);
    sceGuTexImage(0, texture->texture->pW, texture->texture->pH, texture->texture->pW, texture->texture->data);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_CLAMP, GU_CLAMP);
    sceGuAmbientColor(0xFFFFFFFF);
    sceGuColor(0xFFFFFFFF);
    sceGuDisable(GU_DEPTH_TEST);
    struct VertexTexture *vertices = (struct VertexTexture *)sceGuGetMemory(2 * sizeof(struct VertexTexture));
    vertices[0].color = 0xFFFFFFFF;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[0].u = 0;
    vertices[0].v = 0;
    vertices[1].color = 0xFFFFFFFF;
    vertices[1].x = x + texture->texture->width;
    vertices[1].y = y + texture->texture->height;
    vertices[1].z = 0;
    vertices[1].u = texture->texture->width;
    vertices[1].v = texture->texture->height;
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 2, 0, vertices);
    #elif defined(NDS_MODE)
    if (texture->imageData == NULL || texture->TextureID == -1)
        return;
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(5));
	glColor(PC_WHITE._rgb15);
	glSetActiveTexture(texture->TextureID);
	glAssignColorTable(0, texture->TextureID);
    glSprite(x, y, GL_FLIP_NONE, texture->imageData);
    #elif defined(CG_MODE) || defined(FX_MODE)
    dimage(x, y, texture->texture);
    #endif
}

void PC_DrawSubTexture(PC_Texture* texture, int x, int y, int sx, int sy, int sw, int sh)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    SDL_Rect rect = {x, y, sw, sh};
    SDL_Rect rect2 = {sx, sy, sw, sh};
    SDL_RenderCopy(renderer, texture->texture, &rect2, &rect);
    #elif defined(PSP_MODE)
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, texture->vram);
    sceGuTexImage(0, texture->texture->pW, texture->texture->pH, texture->texture->pW, texture->texture->data);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_CLAMP, GU_CLAMP);
    sceGuAmbientColor(0xFFFFFFFF);
    sceGuColor(0xFFFFFFFF);
    sceGuDisable(GU_DEPTH_TEST);
    struct VertexTexture *vertices = (struct VertexTexture *)sceGuGetMemory(2 * sizeof(struct VertexTexture));
    vertices[0].color = 0xFFFFFFFF;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[0].u = sx;
    vertices[0].v = sy;
    vertices[1].color = 0xFFFFFFFF;
    vertices[1].x = x + sw;
    vertices[1].y = y + sh;
    vertices[1].z = 0;
    vertices[1].u = sx + sw;
    vertices[1].v = sy + sh;
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 2, 0, vertices);
    #elif defined(NDS_MODE)
    if (texture->imageData == NULL || texture->TextureID == -1)
        return;
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(5));
    glColor(PC_WHITE._rgb15);
    glSetActiveTexture(texture->TextureID);
    glAssignColorTable(0, texture->TextureID);
    glSubSprite(x, y, sx, sy, sw, sh, texture->imageData);
    #elif defined(CG_MODE) || defined(FX_MODE)
    dsubimage(x, y,texture->texture, sx, sy, sw, sh, DIMAGE_NONE);
    #endif
}

void PC_DrawSubTextureSize(PC_Texture* texture, int x, int y, int sx, int sy, int sw, int sh, int w, int h)
{
    if (sw == w && sh == h)
    {
        PC_DrawSubTexture(texture, x, y, sx, sy, sw, sh);
        return;
    }
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    /*
    SDL_Rect rect = {x, y, w, h};
    SDL_Rect rect2 = {sx, sy, sw, sh};
    SDL_RenderCopy(renderer, texture->texture, &rect2, &rect);*/
    SDL_Rect rect = { x, y, abs(w), abs(h) };
    SDL_Rect rect2 = { sx, sy, sw, sh };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (w < 0) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (h < 0) {
        flip |= SDL_FLIP_VERTICAL;
    }

    SDL_RenderCopyEx(renderer, texture->texture, &rect2, &rect, 0, NULL, flip);

    #elif defined(PSP_MODE)
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, texture->vram);
    sceGuTexImage(0, texture->texture->pW, texture->texture->pH, texture->texture->pW, texture->texture->data);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_CLAMP, GU_CLAMP);
    sceGuAmbientColor(0xFFFFFFFF);
    sceGuColor(0xFFFFFFFF);
    sceGuDisable(GU_DEPTH_TEST);
    struct VertexTexture *vertices = (struct VertexTexture *)sceGuGetMemory(2 * sizeof(struct VertexTexture));
    vertices[0].color = 0xFFFFFFFF;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[0].u = sx;
    vertices[0].v = sy;
    vertices[1].color = 0xFFFFFFFF;
    vertices[1].x = x + w;
    vertices[1].y = y + h;
    vertices[1].z = 0;
    vertices[1].u = sx + sw;
    vertices[1].v = sy + sh;
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 2, 0, vertices);
    #elif defined(NDS_MODE)
    if (texture->imageData == NULL || texture->TextureID == -1)
        return;
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(5));
    glColor(PC_WHITE._rgb15);
    glSetActiveTexture(texture->TextureID);
    glAssignColorTable(0, texture->TextureID);
    glSubSpriteScale(x, y, sx, sy, sw, sh,w,h, texture->imageData);
    #elif defined(CG_MODE) || defined(FX_MODE)
    int flipX = 0;
    int flipY = 0;
    
    if (w < 0) {
        flipX = 1;
        w = -w;
    }
    if (h < 0) {
        flipY = 1;
        h = -h;
    }
    int x2 = 0, y2 = 0;
    unsigned int sx2 = 0, sy2 = 0;
    int xinc = (sw << 16) / w;
    int yinc = (sh << 16) / h;
    //limites les bornes de la texture
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;
    if(sx + sw > texture->texture->width) sw = texture->texture->width - sx;
    if(sy + sh > texture->texture->height) sh = texture->texture->height - sy;
    //limites les bornes en fonction de la taille de la fenetre
    if(x < 0) {
        sx -= x;
        w += x;
        x = 0;
    }
    if(y < 0) {
        sy -= y;
        h += y;
        y = 0;
    }
    if(x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if(y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    uint16_t *data_u16 = NULL;
    const int alph = texture->alpha;
    int row = 0;
    int col = 0;
    while (row < h) {
        x2 = 0;
        col = 0;
        
        sy2 = sy + (flipY ? (sh - 1 - (y2 >> 16)) : (y2 >> 16));
        while (col < w) {
            sx2 = sx + (flipX ? (sw - 1 - (x2 >> 16)) : (x2 >> 16));
            const int dataPix = texture->GetPixel(texture, sx2, sy2);
            
            if (dataPix != alph)
            {
                gint_vram[DWIDTH * (y + row) + (x + col)] = texture->DecodePixel(texture, dataPix);
            }
            
            x2 += xinc;
            col++;
        }
        
        y2 += yinc;
        row++;
    }
    #endif
}

void PC_DrawSubTextureSizeColored(PC_Texture* texture, int x, int y, int sx, int sy, int sw, int sh, int w, int h, PC_Color color)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    /*SDL_Rect rect = {x, y, w, h};
    SDL_Rect rect2 = {sx, sy, sw, sh};
    SDL_SetTextureColorMod(texture->texture, color.r, color.g, color.b);
    SDL_RenderCopy(renderer, texture->texture, &rect2, &rect);
    SDL_SetTextureColorMod(texture->texture, 255, 255, 255);*/
    SDL_Rect rect = { x, y, abs(w), abs(h) };
    SDL_Rect rect2 = { sx, sy, sw, sh };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (w < 0) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (h < 0) {
        flip |= SDL_FLIP_VERTICAL;
    }
    SDL_SetTextureColorMod(texture->texture, color.r, color.g, color.b);
    SDL_RenderCopyEx(renderer, texture->texture, &rect2, &rect, 0, NULL, flip);
    SDL_SetTextureColorMod(texture->texture, 255, 255, 255);
    #elif defined(PSP_MODE)
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_BLEND);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, texture->vram);
    sceGuTexImage(0, texture->texture->pW, texture->texture->pH, texture->texture->pW, texture->texture->data);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_CLAMP, GU_CLAMP);
    sceGuAmbientColor(0xFFFFFFFF);
    sceGuColor(0xFFFFFFFF);
    sceGuDisable(GU_DEPTH_TEST);
    struct VertexTexture *vertices = (struct VertexTexture *)sceGuGetMemory(4 * sizeof(struct VertexTexture));
    vertices[0].color = color.color;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0;
    vertices[0].u = sx;
    vertices[0].v = sy;
    vertices[1].color = color.color;
    vertices[1].x = x + w;
    vertices[1].y = y;
    vertices[1].z = 0;
    vertices[1].u = sx + sw;
    vertices[1].v = sy;
    vertices[2].color = color.color;
    vertices[2].x = x;
    vertices[2].y = y + h;
    vertices[2].z = 0;
    vertices[2].u = sx;
    vertices[2].v = sy + sh;
    vertices[3].color = color.color;
    vertices[3].x = x + w;
    vertices[3].y = y + h;
    vertices[3].z = 0;
    vertices[3].u = sx + sw;
    vertices[3].v = sy + sh;
    unsigned short indices[] = {0, 1, 2, 3};
    sceGuDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 4, indices, vertices);
    #elif defined(NDS_MODE)
    if (texture->imageData == NULL || texture->TextureID == -1)
        return;
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(5));
    glColor(color._rgb15);
    glSetActiveTexture(texture->TextureID);
    glAssignColorTable(0, texture->TextureID);
    glSubSpriteScale(x, y, sx, sy, sw, sh,w,h, texture->imageData);
    #elif defined(CG_MODE) || defined(FX_MODE)
    int flipX = 0;
    int flipY = 0;
    
    if (w < 0) {
        flipX = 1;
        w = -w;
    }
    if (h < 0) {
        flipY = 1;
        h = -h;
    }
    int x2 = 0, y2 = 0;
    unsigned int sx2 = 0, sy2 = 0;
    int xinc = (sw << 16) / w;
    int yinc = (sh << 16) / h;
    //limites les bornes de la texture
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;
    if(sx + sw > texture->texture->width) sw = texture->texture->width - sx;
    if(sy + sh > texture->texture->height) sh = texture->texture->height - sy;
    //limites les bornes en fonction de la taille de la fenetre
    if(x < 0) {
        sx -= x;
        w += x;
        x = 0;
    }
    if(y < 0) {
        sy -= y;
        h += y;
        y = 0;
    }
    if(x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if(y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    uint16_t *data_u16 = NULL;
    const int alph = texture->alpha;
    int row = 0;
    int col = 0;
    while (row < h) {
        x2 = 0;
        col = 0;
        
        sy2 = sy + (flipY ? (sh - 1 - (y2 >> 16)) : (y2 >> 16));
        while (col < w) {
            sx2 = sx + (flipX ? (sw - 1 - (x2 >> 16)) : (x2 >> 16));
            const int dataPix = texture->GetPixel(texture, sx2, sy2);
            
            if (dataPix != alph)
            {
                int colPix = texture->DecodePixel(texture, dataPix);
                //multiply color
                int R, G, B;
                ToRGB(colPix, &R, &G, &B);
                PC_Color colorRes =PC_ColorCreate(R * color.r / 255, G * color.g / 255, B * color.b / 255, 255);
                gint_vram[DWIDTH * (y + row) + (x + col)] = colorRes.color;
            }
            
            x2 += xinc;
            col++;
        }
        
        y2 += yinc;
        row++;
    }
    #endif
}

void PC_DrawSubTextureRect(PC_Texture* texture, PC_Rect* src, PC_Rect* dest)
{
    if (dest == NULL)
        return;
    if (TextureColorMod.a+TextureColorMod.b+TextureColorMod.g+TextureColorMod.r == 255*4)
    {
        if (src == NULL)
        {
            PC_DrawSubTexture(texture, dest->x, dest->y, GetWidth(texture), GetHeight(texture), dest->w, dest->h);
            return;
        }
        PC_DrawSubTextureSize(texture, dest->x, dest->y, src->x, src->y, src->w, src->h, dest->w, dest->h);
    }
    else
    {
        if (src == NULL)
        {
            PC_DrawSubTextureSizeColored(texture, dest->x, dest->y, 0, 0, GetWidth(texture), GetHeight(texture), dest->w, dest->h, TextureColorMod);
            return;
        }
        PC_DrawSubTextureSizeColored(texture, dest->x, dest->y, src->x, src->y, src->w, src->h, dest->w, dest->h, TextureColorMod);
    }
}

int GetWidth(PC_Texture* texture){
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    return texture->width;
    #elif defined(PSP_MODE)
    return texture->width;
    //return texture->texture->width;
    #elif defined(NDS_MODE)
    return texture->width;
    #elif defined(CG_MODE) || defined(FX_MODE)
    return texture->texture->width;
    #endif
}
int GetHeight(PC_Texture* texture){
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    return texture->height;
    #elif defined(PSP_MODE)
    return texture->height;
    //return texture->texture->height;
    #elif defined(NDS_MODE)
    return texture->height;
    #elif defined(CG_MODE) || defined(FX_MODE)
    return texture->texture->height;
    #endif
}

#if defined(CG_MODE)
int GetPixel_CG_RGB16(PC_Texture* texture, int x, int y){
    void *data = texture->texture->data + y * texture->texture->stride;
    uint16_t *data_u16 = data;
    return data_u16[x];
}

int GetPixel_CG_P8(PC_Texture* texture, int x, int y){
    void *data = texture->texture->data + y * texture->texture->stride;
    uint8_t *data_u8 = data;
    return (int8_t)data_u8[x];
}
int GetPixel_CG_P4(PC_Texture* texture, int x, int y){
    void *data = texture->texture->data + y * texture->texture->stride;
    uint8_t *data_u8 = data;
    return (x & 1) ? data_u8[x >> 1] & 0x0f : data_u8[x >> 1] >> 4;
}

int DecodePixel_CG_RGB16(PC_Texture* texture, int pixel){
    return pixel;
}

int DecodePixel_CG_P8(PC_Texture* texture, int pixel){
    return texture->texture->palette[pixel+128];
}
int DecodePixel_CG_P4(PC_Texture* texture, int pixel){
    return texture->texture->palette[pixel];
}
#endif