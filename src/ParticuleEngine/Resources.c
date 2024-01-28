#include "Resources.h"
#include "ParticuleEngine.h"
#include "ParticuleEngineFont.h"
#include "ParticuleEngineTexture.h"
#include <List.h>

//*<PROJECT_NAME>*/
#define PROJECT_NAME "P3D"
//*</PROJECT_NAME>*/

#if defined(PSP_MODE)
PSP_MODULE_INFO(PROJECT_NAME, 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

List* AllResources = NULL;

void ToLower(char** str)
{
    for (int i = 0; i < strlen(*str); i++)
    {
        if ((*str)[i] >= 'A' && (*str)[i] <= 'Z')
        {
            (*str)[i] = (*str)[i] + ('a' - 'A');
        }
    }
}

void* GetResource(unsigned char* path)
{
    for (List_node*cur=NULL; ForEach(AllResources,&cur);)
    {
        VirtualFile* file = (VirtualFile*)(cur->data);
        //ToLower(&file->path);
        //ToLower(&path);
        if (strcmp(file->path, path) == 0)
        {
            return file->data;
        }
    }
    return NULL;
}

#if defined(PSP_MODE)
void AddTexture(unsigned char* path,int width, int height){
    VirtualFile* file = malloc(sizeof(VirtualFile));
    file->path = malloc(strlen(path) + 1);
    strcpy(file->path, path);
    file->data = vector2_create_ptr(width, height);
    List_add(AllResources, file);
}
#elif defined(NDS_MODE)
void AddTexture(unsigned char* path, const unsigned int* Bitmap, const unsigned short* Pal, int width, int height)
{
    VirtualFile* file = malloc(sizeof(VirtualFile));
    file->path = malloc(strlen(path) + 1);
    strcpy(file->path, path);
    file->data = malloc(sizeof(PC_Texture));
    PC_Texture* texture = (PC_Texture*)file->data;
    texture->imageData = NULL;
    texture->width = width;
    texture->height = height;
    texture->Bitmap = Bitmap;
    texture->TextureID = 0;
    texture->Pal = Pal;
    List_add(AllResources, file);
}
#elif defined(CG_MODE) || defined(FX_MODE)
bopti_image_t* LoadExternalImage(char* path)
{
    FILE* file = fopen((const char*)path, "rb");
    if (file == NULL)
    {
        printf("Error: can't open file %s\n", path);
        return NULL;
    }
    uint8_t format;
    int16_t color_count;
    uint16_t width,height;
    int stride;
    fread(&format,sizeof(uint8_t),1,file);
    fread(&color_count,sizeof(int16_t),1,file);
    fread(&width,sizeof(uint16_t),1,file);
    fread(&height,sizeof(uint16_t),1,file);
    fread(&stride,sizeof(int),1,file);
    bopti_image_t* img = image_alloc(width,height,format);
    //img->stride = stride;
    img->color_count = color_count;
    img->palette = 0;
    unsigned long long SizeOfData = 0;
    fread(&SizeOfData,sizeof(unsigned long long),1,file);
    fread(img->data,1,SizeOfData,file);
    unsigned int SizeOfPalette = 0;
    fread(&SizeOfPalette,sizeof(unsigned int),1,file);
    if (SizeOfPalette!=0)
    {
        image_alloc_palette(img,color_count);
        fread(img->palette,1,SizeOfPalette,file);
    }
    fclose(file);
    return img;
}

bopti_image_t* LoadImageSprite(char* path,int index)
{
    FILE* file = fopen((const char*)path, "rb");
    if (file == NULL)
    {
        printf("Error: can't open file %s\n", path);
        return NULL;
    }
    while (index--)
    {
        fseek(file, sizeof(uint8_t)+sizeof(int16_t)+sizeof(uint16_t)+sizeof(uint16_t)+sizeof(int), SEEK_CUR);
        unsigned long long SizeOfData = 0;
        fread(&SizeOfData,sizeof(unsigned long long),1,file);
        fseek(file, SizeOfData, SEEK_CUR);
        unsigned int SizeOfPalette = 0;
        fread(&SizeOfPalette,sizeof(unsigned int),1,file);
        if (SizeOfPalette!=0)
        {
            fseek(file, SizeOfPalette, SEEK_CUR);
        }
    }
    
    uint8_t format;
    int16_t color_count;
    uint16_t width,height;
    int stride;
    fread(&format,sizeof(uint8_t),1,file);
    fread(&color_count,sizeof(int16_t),1,file);
    fread(&width,sizeof(uint16_t),1,file);
    fread(&height,sizeof(uint16_t),1,file);
    fread(&stride,sizeof(int),1,file);
    bopti_image_t* img = image_alloc(width,height,format);
    //img->stride = stride;
    img->color_count = color_count;
    img->palette = 0;
    unsigned long long SizeOfData = 0;
    fread(&SizeOfData,sizeof(unsigned long long),1,file);
    fread(img->data,1,SizeOfData,file);
    unsigned int SizeOfPalette = 0;
    fread(&SizeOfPalette,sizeof(unsigned int),1,file);
    if (SizeOfPalette!=0)
    {
        image_alloc_palette(img,color_count);
        fread(img->palette,1,SizeOfPalette,file);
    }
    fclose(file);
    return img;
}

bopti_image_t* LoadExternalSubImageSize(char* path,int sx, int sy, int sw, int sh)
{
    FILE* file = fopen((const char*)path, "rb");
    if (file == NULL)
    {
        printf("Error: can't open file %s\n", path);
        return NULL;
    }
    uint8_t format;
    int16_t color_count;
    uint16_t width,height;
    int stride;
    fread(&format,sizeof(uint8_t),1,file);
    fread(&color_count,sizeof(int16_t),1,file);
    fread(&width,sizeof(uint16_t),1,file);
    fread(&height,sizeof(uint16_t),1,file);
    fread(&stride,sizeof(int),1,file);
    if (sx>=width || sy>=height || sw<=0 || sh<=0)
    {
        return NULL;
    }
    if (sx+sw>width)
    {
        sw = width - sx;
    }
    if (sy+sh>height)
    {
        sh = height - sy;
    }
    if (sx<0)
    {
        sw += sx;
        sx = 0;
    }
    if (sy<0)
    {
        sh += sy;
        sy = 0;
    }

    bopti_image_t* img = image_alloc(sw,sh,format);
    //img->stride = stride;
    int SizeOfType = IMAGE_IS_RGB16(format) ? 2 : 1;
    float mul = 1.0f;
    if (IMAGE_IS_P4(format))
    {
        mul = 0.5f;
    }

    unsigned long long SizeOfData = 0;
    fread(&SizeOfData,sizeof(unsigned long long),1,file);
    
    //use fseek to go to the sy position
    fseek(file,(int)(sy * SizeOfType * width*mul),SEEK_CUR);
    unsigned int count = 0;
    for (int i = 0; i < sh; i++)
    {
        //use fseek to go to the sx position
        fseek(file,(int)(sx * SizeOfType*mul),SEEK_CUR);
        fread(img->data + count,SizeOfType,(int)(sw*mul),file);
        count += SizeOfType * sw*mul;
        //use fseek to go to the end of the line
        fseek(file,(int)((width - sx - sw) * SizeOfType*mul),SEEK_CUR);
    }
    //use fseek to go to the end of the image
    fseek(file,(int)((height - sy - sh) * SizeOfType * width*mul),SEEK_CUR);

    unsigned int SizeOfPalette = 0;
    fread(&SizeOfPalette,sizeof(unsigned int),1,file);
    if (SizeOfPalette!=0)
    {
        image_alloc_palette(img,color_count);
        fread(img->palette,1,SizeOfPalette,file);
    }
    fclose(file);
    return img;
}


void AddTexture(unsigned char* path, bopti_image_t* texture,char* IsExternal)
{
    VirtualFile* file = malloc(sizeof(VirtualFile));
    file->path = malloc(strlen(path) + 1);
    strcpy(file->path, path);
    file->data = malloc(sizeof(PC_Texture));
    PC_Texture* tex = (PC_Texture*)file->data;
    if (IsExternal != NULL)
    {
        tex->texture = NULL;
        tex->IsExternalPath = malloc(strlen(IsExternal) + 1);
        strcpy(tex->IsExternalPath, IsExternal);
    }
    else
    {
        tex->texture = texture;
        tex->IsExternalPath = NULL;
    }
    SetupFormatTexture(tex);

    List_add(AllResources, file);
}
void AddFont(unsigned char* path, font_t* font)
{
    VirtualFile* file = malloc(sizeof(VirtualFile));
    file->path = malloc(strlen(path) + 1);
    strcpy(file->path, path);
    file->data = malloc(sizeof(PC_Font));
    PC_Font* ft = (PC_Font*)file->data;
    ft->font = font;
    List_add(AllResources, file);
}
#endif

void LoadResources(){
    #if defined(NDS_MODE) || defined(CG_MODE) || defined(FX_MODE) || defined(PSP_MODE)
    AllResources = List_new();
    #endif
//*<RSC_LOAD>*/
	#if defined(CG_MODE) || defined(FX_MODE)
	AddTexture((unsigned char*)"assets/Images/Block.png", &IMG_ASSET_bf6c052f2b9e40858f1017634849e2e1,NULL);
	AddTexture((unsigned char*)"assets/Images/Players.png", &IMG_ASSET_2a5eaf758a7f449f8457705602c98351,NULL);
	AddTexture((unsigned char*)"assets/Images/Shadow.png", &IMG_ASSET_8d515b2d28494d9c84dc185cf93ab76d,NULL);
	AddTexture((unsigned char*)"assets/Images/UI.png", &IMG_ASSET_3e3a11dd22b64ed3a175866573390630,NULL);
	AddTexture((unsigned char*)"assets/Images/Items.png", NULL,(char*)"DATAS/Items.png.bin.items");
	#endif
//*</RSC_LOAD>*/
}

void UnloadResources(){
//*<RSC_UNLOAD>*/
//*</RSC_UNLOAD>*/
}

//*<RSC>*/
//*</RSC>*/
