#ifndef CAMERA_H
#define CAMERA_H
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "../ParticuleEngine/ParticuleEngine.h"
#include "../ParticuleEngine/Resources.h"
#include "../InterfaceSystem/InterfaceSystem.h"
#include "../InterfaceSystem/EventDataSystem.h"

typedef struct Camera {
    int x;
    int y;
    int z;
    int angle;
} Camera;

extern Camera* MainCamera;

extern short Zdepth[396*224];

Camera* Camera_new(float x, float y, float z);

void Camera_delete(Camera* camera);

typedef struct RenderVars
{
    long x;
    long y;
    long z;
    int ratio;
    long long spaceX;
    long long spaceY;
    Vector2 firstPosition;
    int BaseScale;
} RenderVars;

Vector3 GetNewPositionWithRotate(Vector3 position);

RenderVars ApplyRenderVarsZ(RenderVars render);

RenderVars GetRenderVars(Vector3 position);

void MoveFreeCamera(int speed);

void SetZDepth(int x, int y, short z);

short GetZDepth(int x, int y);

void ClearZdepth();

int multi_color(unsigned int pixel, PC_Color color);

void DrawZtexture(int z, PC_Color teinte, PC_Texture* texture, int x, int y, int sx, int sy, int sw, int sh, int w, int h);

void DrawZLine(int zStart, Vector2 point1, int zEnd, Vector2 point2, PC_Color color);

void DrawZPixelImmediate(int FromX, int FromY,PC_Color teinte, PC_Texture* texture,int SetX, int SetY, int SetZ);

typedef struct Entity Entity;

Entity* SpawnFreeCam();

#endif