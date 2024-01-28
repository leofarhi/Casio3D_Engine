#ifndef RESOURCES_H
#define RESOURCES_H
#include <List.h>
#include "ParticuleEngine.h"

extern List* AllResources;

typedef struct VirtualFile
{
    char* path;
    void* data;
} VirtualFile;

void* GetResource(unsigned char* path);

#if defined(PSP_MODE)
void AddTexture(unsigned char* path,int width, int height);
#elif defined(NDS_MODE)
void AddTexture(unsigned char* path, const unsigned int* Bitmap, const unsigned short* Pal, int width, int height);
#elif defined(CG_MODE) || defined(FX_MODE)
bopti_image_t* LoadExternalImage(char* path);
bopti_image_t* LoadExternalSubImageSize(char* path,int sx, int sy, int sw, int sh);
bopti_image_t* LoadImageSprite(char* path,int index);
void AddTexture(unsigned char* path, bopti_image_t* texture,char* IsExternal);
void AddFont(unsigned char* path, font_t* font);
#endif

void LoadResources();

void UnloadResources();

//*<RSC>*/
	#if defined(CG_MODE) || defined(FX_MODE)
extern bopti_image_t IMG_ASSET_bf6c052f2b9e40858f1017634849e2e1;
extern bopti_image_t IMG_ASSET_2a5eaf758a7f449f8457705602c98351;
extern bopti_image_t IMG_ASSET_8d515b2d28494d9c84dc185cf93ab76d;
extern bopti_image_t IMG_ASSET_3e3a11dd22b64ed3a175866573390630;
	#endif
//*</RSC>*/


#endif