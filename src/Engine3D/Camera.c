#include "Camera.h"
#include "Entity.h"
#include "World.h"

Camera* MainCamera = NULL;

short Zdepth[396*224];

void ClearZdepth(){
    for (int i = 0; i < 396*224; i++)
    {
        Zdepth[i] = SHRT_MAX;
    }
}

int multi_color(unsigned int pixel, PC_Color color) {
    unsigned int R = ((pixel >> 11) & 0b11111) << 3;
    unsigned int G = ((pixel >> 5) & 0b111111) << 2;
    unsigned int B = (pixel & 0b11111) << 3;

    unsigned int newR = min(((R * (unsigned int)color.r) >> 8) & 0b11111000,255);
    unsigned int newG = min(((G * (unsigned int)color.g) >> 8) & 0b11111100,255);
    unsigned int newB = min(((B * (unsigned int)color.b) >> 8) & 0b11111000,255);

    unsigned int result = (newR << 8) | (newG << 3) | (newB >> 3);
    return result;
}

void DrawZtexture(int z,PC_Color teinte, PC_Texture* texture, int x, int y, int sx, int sy, int sw, int sh, int w, int h){
    int x2 = 0, y2 = 0;
    unsigned int sx2 = 0, sy2 = 0;
    int xinc = (1 << 16);
    int yinc = (1 << 16);
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

    if (sw != w)
    {
        xinc = (sw << 16) / w;
    }
    if (sh != h)
    {
        yinc = (sh << 16) / h;
    }
    //limites les bornes de la texture
    if(sx < 0) sx = 0;
    if(sy < 0) sy = 0;
    if(sx + sw > GetWidth(texture)) sw = GetWidth(texture) - sx;
    if(sy + sh > GetHeight(texture)) sh = GetHeight(texture) - sy;
    //limites les bornes en fonction de la taille de la fenetre
    if(x < 0) {
        sx -= (x * sw) / w;
        w += x;
        x = 0;
    }
    if(y < 0) {
        sy -= (y * sh) / h;
        h += y;
        y = 0;
    }
    if(x + w > SCREEN_WIDTH) w = SCREEN_WIDTH - x;
    if(y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    //PC_DrawSubTextureSize(texture, x, y, sx, sy, sw, sh, w, h);
    int row = 0;
    int col = 0;
    Uint32 *pixels = (Uint32 *)texture->surface->pixels;
    while (row < h) {
        x2 = 0;
        col = 0;
        
        sy2 = sy + (flipY ? (sh - 1 - (y2 >> 16)) : (y2 >> 16));
        while (col < w) {
            sx2 = sx + (flipX ? (sw - 1 - (x2 >> 16)) : (x2 >> 16));
            //get pixel of the texture SDL_Surface
            Uint8 r, g, b, a;
            Uint32 pixel = pixels[sx2 + sy2 * texture->surface->w];
            SDL_GetRGBA(pixel, texture->surface->format, &r, &g, &b, &a);
            //draw pixel
            if ( Zdepth[SCREEN_WIDTH * (y + row) + (x + col)] > z &&
                    a >= 255/2)
            {
                a = 255;
                Zdepth[SCREEN_WIDTH * (y + row) + (x + col)] = z;
                PC_Color color = PC_ColorCreate(r, g, b, a);
                //
                color.r = min((color.r*teinte.r)/255,255);
                color.g = min((color.g*teinte.g)/255,255);
                color.b = min((color.b*teinte.b)/255,255);
                //
                PC_DrawPoint(x + col, y + row, color);
            }
            
            x2 += xinc;
            col++;
        }
        
        y2 += yinc;
        row++;
    }
    #elif defined(CG_MODE) || defined(FX_MODE)
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
            
            if ( Zdepth[SCREEN_WIDTH * (y + row) + (x + col)] > z && dataPix != alph)
            {
                unsigned int pixel = texture->DecodePixel(texture, dataPix);
                pixel = multi_color(pixel,teinte);

                Zdepth[SCREEN_WIDTH * (y + row) + (x + col)] = z;
                gint_vram[DWIDTH * (y + row) + (x + col)] = pixel;
            }
            
            x2 += xinc;
            col++;
        }
        
        y2 += yinc;
        row++;
    }
    #endif
}

void DrawZPixelImmediate(int FromX, int FromY,PC_Color teinte, PC_Texture* texture,int SetX, int SetY, int SetZ)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    Uint32 *pixels = (Uint32 *)texture->surface->pixels;
    Uint8 r, g, b, a;
    Uint32 pixel = pixels[FromX + FromY * texture->surface->w];
    SDL_GetRGBA(pixel, texture->surface->format, &r, &g, &b, &a);
    if ( Zdepth[SCREEN_WIDTH * SetY + SetX] > SetZ &&
            a >= 255/2)
    {
        a = 255;
        Zdepth[SCREEN_WIDTH * SetY + SetX] = SetZ;
        PC_Color color = PC_ColorCreate(r, g, b, a);
        //
        color.r = min((color.r*teinte.r)/255,255);
        color.g = min((color.g*teinte.g)/255,255);
        color.b = min((color.b*teinte.b)/255,255);
        //
        PC_DrawPoint(SetX, SetY, color);
    }
    #elif defined(CG_MODE) || defined(FX_MODE)
    const int alph = texture->alpha;
    const int dataPix = texture->GetPixel(texture, FromX, FromY);
    if ( Zdepth[SCREEN_WIDTH * SetY + SetX] > SetZ && dataPix != alph)
    {
        unsigned int pixel = texture->DecodePixel(texture, dataPix);
        pixel = multi_color(pixel,teinte);

        Zdepth[SCREEN_WIDTH * SetY + SetX] = SetZ;
        gint_vram[DWIDTH * SetY + SetX] = pixel;
    }
    #endif

}

void DrawZLine(int zStart, Vector2 point1, int zEnd, Vector2 point2, PC_Color color)
{
    int x1 = point1.x;
    int y1 = point1.y;
    int x2 = point2.x;
    int y2 = point2.y;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    int z = zStart;
    while (1) {
        if ( (Zdepth[SCREEN_WIDTH * y1 + x1] > z) && (y1>=0 && x1>=0 && y1<SCREEN_HEIGHT && x1<SCREEN_WIDTH) )
        {
            Zdepth[SCREEN_WIDTH * y1 + x1] = z;
            #if defined(WIN_MODE)  || defined(LINUX_MODE)
            PC_DrawPoint(x1, y1, color);
            #elif defined(CG_MODE) || defined(FX_MODE)
            gint_vram[DWIDTH * y1 + x1] = color.r << 8 | color.g << 3 | color.b >> 3;
            #endif
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = err << 1;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
        z += (zEnd - zStart) / (dx + dy);
    }
}


Camera* Camera_new(float x, float y, float z)
{
    Camera* camera = (Camera*)malloc(sizeof(Camera));
    camera->x = x;
    camera->y = y;
    camera->z = z;
    camera->angle = 0;
    return camera;
}

void Camera_delete(Camera* camera)
{
    free(camera);
}

Vector3 GetNewPositionWithRotate(Vector3 position)
{
    //position.z = (MainCamera->rotateX==2)?(world->depth*BLOCK_SIZE)-position.z:position.z;
    return position;
}

RenderVars ApplyRenderVarsZ(RenderVars render)
{

    render.x = render.firstPosition.x - ((render.z*render.spaceX) >> 16);
    render.y = render.firstPosition.y - ((render.z*render.spaceY) >> 16);
    render.y += MainCamera->angle;

    return render;
}

RenderVars GetRenderVars(Vector3 position)
{
    position = GetNewPositionWithRotate(position);
    long x = position.x - MainCamera->x;
    long y = -position.y + MainCamera->y - MainCamera->z;
    const long z = MainCamera->z + position.z;
    const int midScreenW = SCREEN_WIDTH/2;
    const int midScreenH = SCREEN_HEIGHT/2;

    const int ratio = (BLOCK_SIZE<< 16)/SCREEN_WIDTH;
    const int BaseScale = BLOCK_SIZE-((z*ratio) >> 16);

    const long long spaceX = ((x-midScreenW) << 16) / SCREEN_WIDTH;
    const long long spaceY = ((y-midScreenH) << 16) / SCREEN_WIDTH;
    Vector2 firstPosition = (Vector2){x,y};

    RenderVars render = (RenderVars){x,y,z,ratio,spaceX,spaceY,firstPosition,BaseScale};
    render = ApplyRenderVarsZ(render);
    return render;
}


void SetZDepth(int x, int y, short z)
{
    Zdepth[x + y*396] = z;
}

short GetZDepth(int x, int y)
{
    return Zdepth[x + y*396];
}


void MoveFreeCamera(int speed)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
		//if Right arrow key is pressed
		if (IsKeyPressed(SDLK_RIGHT))
			MainCamera->x+=speed;
		//if Left arrow key is pressed
		if (IsKeyPressed(SDLK_LEFT))
			MainCamera->x-=speed;
        //if Up arrow key is pressed
        if (IsKeyPressed(SDLK_UP))
            MainCamera->y+=speed;
        //if Down arrow key is pressed
        if (IsKeyPressed(SDLK_DOWN))
            MainCamera->y-=speed;
        if (IsKeyPressed(SDLK_u))
            MainCamera->z-=speed;
        if (IsKeyPressed(SDLK_j))
            MainCamera->z+=speed;
        if (IsKeyPressed(SDLK_i))
            MainCamera->angle+=speed;
        if (IsKeyPressed(SDLK_k))
            MainCamera->angle-=speed;
		#elif defined(CG_MODE) || defined(FX_MODE)
        if (IsKeyPressed(KEY_RIGHT))
            MainCamera->x+=5;
        if (IsKeyPressed(KEY_LEFT))
            MainCamera->x-=5;
        if (IsKeyPressed(KEY_UP))
            MainCamera->y+=5;
        if (IsKeyPressed(KEY_DOWN))
            MainCamera->y-=5;
        if (IsKeyPressed(KEY_F1))
            MainCamera->z+=speed;
        if (IsKeyPressed(KEY_F2))
            MainCamera->z-=speed;
        if (IsKeyPressed(KEY_F3))
            MainCamera->angle+=speed;
        if (IsKeyPressed(KEY_F4))
            MainCamera->angle-=speed;
        #endif
}

void Entity_UpdateFreeCam(Entity* entity)
{
    int speed = 10*SpeedMultiplier();
    MoveFreeCamera(speed);
}


Entity* SpawnFreeCam()
{
    Entity* e = CreateEntity(NULL, 0, 0, 0);
    e->collider = NULL;
    e->OnTriggerEnter = NULL;
    PlaceEntity(e, 0,0,0);
    e->Update = Entity_UpdateFreeCam;
    return e;
}