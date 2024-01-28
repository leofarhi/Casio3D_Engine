#include "ParticuleEngine.h"

#include "ParticuleEngineDraw.h"
#include "ParticuleEngineTexture.h"
#include "ParticuleEngineInput.h"
#include "ParticuleEngineFont.h"
#include "Resources.h"

#if defined(WIN_MODE) || defined(LINUX_MODE)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int SCREEN_WIDTH = 396;
int SCREEN_HEIGHT = 224;

int delay_update = 50000;

void errx(int exitcode, const char *format, const char* error)
{
    fprintf(stderr, format, error);
    exit(exitcode);
}

void ResetDirectory()
{
    // Obtenez le chemin absolu du programme
    char *path = SDL_GetBasePath();
    if (path == NULL)
        errx(EXIT_FAILURE, "error SDL_GetBasePath %s", SDL_GetError());

    // Changez le répertoire de travail
    if (chdir(path) != 0)
        errx(EXIT_FAILURE, "error chdir %s", SDL_GetError());

    // Libérez la mémoire
    //printf("path: %s\n", path);
    SDL_free(path);
}

#elif defined(PSP_MODE)
#include "Libs/PSP/common/callbacks.h"
#include "Libs/PSP/common/common-sce.h"
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#include <pspkernel.h>
#include <pspdebug.h>

#elif defined(NDS_MODE)
int slcScreen = 0;
#elif defined(CG_MODE) || defined(FX_MODE)
#include <libprof.h>
prof_t FPS;
#endif


Vector2 vector2_create(int x, int y){
    Vector2 vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

Vector2* vector2_create_ptr(int x, int y){
    Vector2* vec = malloc(sizeof(Vector2));
    vec->x = x;
    vec->y = y;
    return vec;
}

Vector3 vector3_create(int x, int y, int z)
{
    Vector3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

Vector3* vector3_create_ptr(int x, int y, int z)
{
    Vector3* vec = malloc(sizeof(Vector3));
    vec->x = x;
    vec->y = y;
    vec->z = z;
    return vec;
}

float vector3_distance(Vector3 a, Vector3 b)
{
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2) + powf(a.z - b.z, 2));
}

void PC_Init()
{
    #if defined(WIN_MODE) || defined(LINUX_MODE)
        // Initializes the SDL.
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            errx(EXIT_FAILURE, "error Init %s", SDL_GetError());
            // Creates a window.
        window = SDL_CreateWindow("Engine", 50, 50, SCREEN_WIDTH*3, SCREEN_HEIGHT*3,
                SDL_WINDOW_SHOWN);// | SDL_WINDOW_RESIZABLE);
        if (window == NULL)
            errx(EXIT_FAILURE, "error window %s", SDL_GetError());

        ResetDirectory();

        // Creates a renderer.
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL)
            errx(EXIT_FAILURE, "error render %s", SDL_GetError());

        // Initializes the image library.
        if (IMG_Init(IMG_INIT_PNG) == 0)
            errx(EXIT_FAILURE, "error IMG_Init %s", IMG_GetError());

        // Initializes the font library.
        if (TTF_Init() == -1)
            errx(EXIT_FAILURE, "error TTF_Init %s", TTF_GetError());
        //enable Alpha Blending
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // Dispatches the events.
        InputEvents = List_new();
        InputEventsHeld = List_new();
    #elif defined(PSP_MODE)
        // Boilerplate
        SetupCallbacks();

        // Initialize Graphics
        initGraphics(PSP_list);

        // Initialize Matrices
        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();

        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();

        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    #elif defined(NDS_MODE)
        videoSetMode(MODE_5_3D | DISPLAY_BG3_ACTIVE);
        videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

        consoleDemoInit();
        keyboardDemoInit();
        glScreen2D();
        nitroFSInit(NULL);
        //dirlist("/");
        // Set  Bank A to texture (128 kb)
        lcdMainOnTop();
        //lcdMainOnBottom();

        vramSetBankA(VRAM_A_TEXTURE);
        vramSetBankB(VRAM_B_TEXTURE);
        vramSetBankE(VRAM_E_TEX_PALETTE);  // Allocate VRAM bank for all the palettes

        /*Font.Load(FontImages,				// pointer to glImage array
            FONT_SI_NUM_IMAGES, 		// Texture packer auto-generated #define
            font_si_texcoords,		// Texture packer auto-generated array
            GL_RGB256,				// texture type for glTexImage2D() in videoGL.h 
            TEXTURE_SIZE_64,			// sizeX for glTexImage2D() in videoGL.h
            TEXTURE_SIZE_128,		// sizeY for glTexImage2D() in videoGL.h
            GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T | TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
            256,						// Length of the palette (256 colors)
            (u16*)font_siPal,		// Palette Data
            (u8*)font_siBitmap		// image data generated by GRIT 
        );
        */
        unsigned short* hitPal = malloc(256 * sizeof(unsigned short));
        int i;
        for (i = 0; i < 256; i++)
        {
            hitPal[i] = (0xFF << 8) | 0xFF;
        }
        // Generate the texture and load the all-white palette
        glGenTextures(1, &PaletteID);
        glBindTexture(0, PaletteID);
        glColorTableEXT(0, 0, 256, 0, 0, hitPal);
        // free some memory
        free(hitPal);
    #elif defined(CG_MODE) || defined(FX_MODE)
        InputEvents = List_new();
    #endif
    TextureColorMod = PC_ColorCreate(255, 255, 255, 255);
    LoadResources();
}

void PC_Quit()
{
    UnloadResources();
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        // Quits the font library.
        TTF_Quit();

        // Quits the image library.
        IMG_Quit();

        // Destroys the renderer.
        SDL_DestroyRenderer(renderer);

        // Destroys the window.
        SDL_DestroyWindow(window);

        // Quits the SDL.
        SDL_Quit();
    #elif defined(PSP_MODE)
        // Terminate Graphics
        termGraphics();

        // Exit Game
        sceKernelExitGame();
    #elif defined(NDS_MODE)
    #elif defined(CG_MODE) || defined(FX_MODE)
    #endif
}

void ClearScreen()
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    #elif defined(PSP_MODE)
        startFrame(PSP_list);

        // We're doing a 2D, Non-Textured render 
        sceGuDisable(GU_DEPTH_TEST);
        //sceGuDisable(GU_TEXTURE_2D);
        //sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_BLEND);

        //Clear background to Bjack
        sceGuClearColor(0xFF000000);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
    #elif defined(NDS_MODE)
    	glBegin2D();
	    glBoxFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PC_BLACK._rgb15);
    #elif defined(CG_MODE) || defined(FX_MODE)
        dclear(C_BLACK);
    #endif
}

void ClearScreenColor(PC_Color color){
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    #elif defined(PSP_MODE)
        startFrame(PSP_list);

        // We're doing a 2D, Non-Textured render 
        sceGuDisable(GU_DEPTH_TEST);
        //sceGuDisable(GU_TEXTURE_2D);
        //sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_BLEND);

        //Clear background to Bjack
        sceGuClearColor(color.color);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT | GU_STENCIL_BUFFER_BIT);
    #elif defined(NDS_MODE)
    	glBegin2D();
        glBoxFilled(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color._rgb15);
    #elif defined(CG_MODE) || defined(FX_MODE)
        dclear(color.color);
    #endif
}

void UpdateScreen()
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        //upscale screen *3
        SDL_RenderSetScale(renderer, 3, 3);
        SDL_RenderPresent(renderer);
        usleep(delay_update);
    #elif defined(PSP_MODE)
        endFrame();
    #elif defined(NDS_MODE)
        glEnd2D();
        glFlush(0);
        //glFlush(1);
        swiWaitForVBlank();
    #elif defined(CG_MODE) || defined(FX_MODE)
        dupdate();
    #endif
}

void SelectScreen(int screen)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    #elif defined(PSP_MODE)
    #elif defined(NDS_MODE)
    #elif defined(CG_MODE)
    #elif defined(FX_MODE)
    #endif
}

int GetTicks()
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        return SDL_GetTicks();
    #elif defined(PSP_MODE)
        return sceKernelGetSystemTimeLow();
    #elif defined(NDS_MODE)
        return 0; //Not implemented
    #elif defined(CG_MODE) || defined(FX_MODE)
        return 0; //Not implemented
    #endif
}

lambdaFunction lambdaFunction_create(void *function, void* data)
{
    lambdaFunction result;
    result.function = (void (*)(void*))function;
    result.data = data;
    return result;
}

void lambdaFunction_call(lambdaFunction function)
{
    if (function.function != NULL)
    {
        function.function(function.data);
    }
}

void lambdaFunction_clear(lambdaFunction function)
{
    function.function = NULL;
    function.data = NULL;
}