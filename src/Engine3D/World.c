#include "World.h"
#include "Entity.h"
#include "Block.h"
#include <float.h>
#include "DialogueSystem.h"



World* world = NULL;

double GetTime()
{
    return (double)clock() / CLOCKS_PER_SEC;
}


void CreateWorld(int ID,int width, int height, int depth)
{
    if (world != NULL)
    {
        DestroyWorld();
    }
    world = (World *)malloc(sizeof(World));
    world->ID = ID;
    world->width = width;
    world->height = height;
    world->depth = depth;
    world->background = NULL;
    world->ambientLight = PC_ColorCreate(255, 255, 255, 255);
    world->backgroundColor = PC_ColorCreate(27, 171, 206, 255);
    world->map = (Block ****)malloc(sizeof(Block ***) * width);
    for (int i = 0; i < width; i++)
    {
        world->map[i] = (Block ***)malloc(sizeof(Block **) * height);
        for (int j = 0; j < height; j++)
        {
            world->map[i][j] = (Block **)malloc(sizeof(Block *) * depth);
            for (int k = 0; k < depth; k++)
            {
                world->map[i][j][k] = NULL;
            }
        }
    }
    world->Entities = List_new();
    world->DrawWorldUI = NULL;
    world->EntitiesToRemove = List_new();
    world->quit = 0;
}

void DestroyWorld()
{
    if (world == NULL)
    {
        return;
    }
    while (world->Entities->size > 0)
    {
        Entity* entity = (Entity*)List_pop(world->Entities);
        FreeEntity(entity);
    }
    List_free(world->Entities);
    List_free(world->EntitiesToRemove);
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                if (world->map[i][j][k] != NULL)
                {
                    free(world->map[i][j][k]);
                }
            }
            free(world->map[i][j]);
        }
        free(world->map[i]);
    }
    free(world->map);
    free(world);
    world = NULL;
}

void PlaceBlock(Block *block, int x, int y, int z)
{
    if (world->map[x][y][z] != NULL)
    {
        FreeBlock(world->map[x][y][z]);
    }
    world->map[x][y][z] = block;
    block->x = x*BLOCK_SIZE;
    block->y = y*BLOCK_SIZE;
    block->z = z*BLOCK_SIZE;
    
}

void RemoveBlock(int x, int y, int z)
{
    if (world->map[x][y][z] != NULL)
    {
        FreeBlock(world->map[x][y][z]);
        world->map[x][y][z] = NULL;
    }
}

void PlaceEntity(Entity *entity, int x, int y, int z)
{
    entity->x = x*BLOCK_SIZE;
    entity->y = y*BLOCK_SIZE;
    entity->z = z*BLOCK_SIZE + (BLOCK_SIZE/2);
    List_add(world->Entities, entity);
}

Block* BlockHit = NULL;
Entity* EntityHit = NULL;

void DrawWorld()
{
    if (world->background != NULL)
    {
        PC_DrawSubTextureSize(world->background->texture, 0, 0, world->background->rect.x,world->background->rect.y,world->background->rect.w,world->background->rect.h , SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    else
    {
        PC_DrawFillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, world->backgroundColor);
    }
    ClearZdepth();
    const int border = -(BLOCK_SIZE * 3);
    const int x0 = ((MainCamera->x + border) / BLOCK_SIZE);
    const int x1 = ((MainCamera->x + SCREEN_WIDTH - border) / BLOCK_SIZE);
    const int y0 = max(0, ((MainCamera->y - SCREEN_HEIGHT+ border + MainCamera->angle) / BLOCK_SIZE));
    const int y1 = min(world->height, ((MainCamera->y + SCREEN_HEIGHT - border + MainCamera->angle) / BLOCK_SIZE));
    int i, j, k; // move variable declarations outside of loops
    short zDepthHit = GetZDepth(MousePosition.x,MousePosition.y);
    BlockHit = NULL;
    EntityHit = NULL;
    for (List_node*cur = NULL; ForEach(world->Entities,&cur);)
    {
        Entity* entity = (Entity*)cur->data;
        if (entity->z + MainCamera->z <=BLOCK_SIZE*10 && entity->z + MainCamera->z>= -BLOCK_SIZE*3 && entity->x+entity->z > x0*BLOCK_SIZE && entity->x-entity->z < x1*BLOCK_SIZE && entity->y > y0*BLOCK_SIZE && entity->y < y1*BLOCK_SIZE)
        {
            DrawEntity(entity);
            if (zDepthHit != GetZDepth(MousePosition.x,MousePosition.y))
            {
                EntityHit = entity;
                zDepthHit = GetZDepth(MousePosition.x,MousePosition.y);
            }
        }
    }
    for (i = 0; i < world->depth; i++){
        int _x0 = max(0,x0-i);
        int _x1 = min(world->width,x1+i+1);
        int _y0 = max(0,y0-i-1);
        int _y1 = min(world->height,y1+i);
        for (j = _y0; j < _y1; j++)
        {
            for (k = _x0; k < _x1; k++)
            {
                if (world->map[k][j][i] != NULL)
                {
                    DrawBlock(world->map[k][j][i]);
                    if (zDepthHit != GetZDepth(MousePosition.x,MousePosition.y))
                    {
                        BlockHit = world->map[k][j][i];
                        zDepthHit = GetZDepth(MousePosition.x,MousePosition.y);
                    }
                }
            }
        }
    }
    if (BlockHit != NULL)
    {
        EntityHit = NULL;
    }
}

void CullingShadowWorld()
{
    //check face culling
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                if (world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                {
                    Block* block = world->map[i][j][k];
                    int back = 0;//(k+1 >= world->depth || world->map[i][j][k+1] == NULL || AllBlockConfig[world->map[i][j][k+1]->id].type != BLOCK_TYPE_SOLID);
                    int front = (k-1 < 0 || world->map[i][j][k-1] == NULL || AllBlockConfig[world->map[i][j][k-1]->id].type != BLOCK_TYPE_SOLID);

                    int left = (i-1 < 0 || world->map[i-1][j][k] == NULL || AllBlockConfig[world->map[i-1][j][k]->id].type != BLOCK_TYPE_SOLID);
                    int right = (i+1 >= world->width || world->map[i+1][j][k] == NULL || AllBlockConfig[world->map[i+1][j][k]->id].type != BLOCK_TYPE_SOLID);

                    int top = (j+1 >= world->height || world->map[i][j+1][k] == NULL || AllBlockConfig[world->map[i][j+1][k]->id].type != BLOCK_TYPE_SOLID);
                    int bottom = (j-1 < 0 || world->map[i][j-1][k] == NULL || AllBlockConfig[world->map[i][j-1][k]->id].type != BLOCK_TYPE_SOLID);
                    ShowSideBlock(block, front, left, right, top, bottom,back);
                    //ShowSideBlock(block, 1, 1, 1, 1, 1,1);
                }
            }
        }
    }

    //reset shadow
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                if (world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                {
                    SetShadow(world->map[i][j][k], 0);
                }
            }
        }
    }
    //check shadow
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                if (world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                {
                    int x = i-1;
                    int y = j-1;
                    while (x>=0 && x < world->width && y>=0 && y < world->height && world->map[x][y][k] == NULL)
                    {
                        y--;
                        x--;
                    }
                    if (y >= 0 && y < world->height && x>=0 && x < world->width)
                    {
                        Block* block = world->map[x][y][k];
                        SetShadow(block, 1);
                    }
                }
            }
        }
    }
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                if (world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                {
                    for (size_t l = 0; l < 6; l++)
                    {
                        PC_Color color = world->map[i][j][k]->Shadows[l];
                        PC_Color newColor = PC_ColorCreate((color.r*world->ambientLight.r)/255,(color.g*world->ambientLight.g)/255,(color.b*world->ambientLight.b)/255,1);
                        world->map[i][j][k]->Shadows[l] = newColor;
                    }
                    
                }
            }
        }
    }
}

void DynamicCullingBlock(int x, int y, int z){
    //check 3x3x3 block around position x,y,z
    for (int i = x-1; i <= x+1; i++)
    {
        for (int j = y-1; j <= y+1; j++)
        {
            for (int k = z-1; k <= z+1; k++)
            {
                if (i>=0 && i < world->width && j>=0 && j < world->height && k>=0 && k < world->depth && world->map[i][j][k] != NULL && AllBlockConfig[world->map[i][j][k]->id].type ==BLOCK_TYPE_SOLID)
                {
                    Block* block = world->map[i][j][k];
                    int back = 0;//(k+1 >= world->depth || world->map[i][j][k+1] == NULL || AllBlockConfig[world->map[i][j][k+1]->id].type != BLOCK_TYPE_SOLID);
                    int front = (k-1 < 0 || world->map[i][j][k-1] == NULL || AllBlockConfig[world->map[i][j][k-1]->id].type != BLOCK_TYPE_SOLID);
                    int left = (i-1 < 0 || world->map[i-1][j][k] == NULL || AllBlockConfig[world->map[i-1][j][k]->id].type != BLOCK_TYPE_SOLID);
                    int right = (i+1 >= world->width || world->map[i+1][j][k] == NULL || AllBlockConfig[world->map[i+1][j][k]->id].type != BLOCK_TYPE_SOLID);
                    int top = (j+1 >= world->height || world->map[i][j+1][k] == NULL || AllBlockConfig[world->map[i][j+1][k]->id].type != BLOCK_TYPE_SOLID);
                    int bottom = (j-1 < 0 || world->map[i][j-1][k] == NULL || AllBlockConfig[world->map[i][j-1][k]->id].type != BLOCK_TYPE_SOLID);
                    ShowSideBlock(block, front, left, right, top, bottom,back);
                    //ShowSideBlock(block, 1, 1, 1, 1, 1,1);
                }
            }
        }
    }
}

void UpdateWorld(){
    //update entity
    for (List_node* cur=NULL;ForEach(world->Entities,&cur);)
    {
        Entity* entity = (Entity*)cur->data;
        //check if the entity is in camera view, if not continue
        int x = entity->x - MainCamera->x;
        int y = -entity->y + MainCamera->y -MainCamera->z + MainCamera->angle;
        if (entity->Update != NULL)
            entity->Update(entity);
        if (entity->OnTriggerEnter !=NULL){
            Block* otherBlock = Entity_CheckCollisionWithWorld(entity);
            Entity* otherEntity = Entity_CheckCollisionWithEntity(entity);
            entity->OnTriggerEnter(entity,otherBlock,otherEntity);
        }
    }
    //remove entity in the list
    for (List_node* cur=NULL;ForEach(world->EntitiesToRemove,&cur);){
        Entity* entity = (Entity*)cur->data;
        List_remove(world->Entities,entity);
        FreeEntity(entity);
    }
    List_clear(world->EntitiesToRemove);
}

void RemoveEntity(Entity* entity){
    List_add(world->EntitiesToRemove,entity);
}

int SideOfBlockHit(){
    if (BlockHit==NULL)
        return -1;
    SetZDepth(MousePosition.x,MousePosition.y,SHRT_MAX);
    int lastZDepth = SHRT_MAX;
    int side = -1;
    int tempShowSide = BlockHit->ShowSide;
    ShowSideBlock(BlockHit,1,0,0,0,0,0);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_FRONT;
    }
    ShowSideBlock(BlockHit,0,1,0,0,0,0);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_LEFT;
    }
    ShowSideBlock(BlockHit,0,0,1,0,0,0);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_RIGHT;
    }
    ShowSideBlock(BlockHit,0,0,0,1,0,0);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_TOP;
    }
    ShowSideBlock(BlockHit,0,0,0,0,1,0);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_BOTTOM;
    }
    ShowSideBlock(BlockHit,0,0,0,0,0,1);
    DrawBlock(BlockHit);
    if (GetZDepth(MousePosition.x,MousePosition.y) != lastZDepth){
        lastZDepth = GetZDepth(MousePosition.x,MousePosition.y);
        side = SIDE_BACK;
    }
    BlockHit->ShowSide = tempShowSide;
    return side;
}

Block* GetBlockHit(){
    return BlockHit;
}

Entity* GetEntityHit(){
    return EntityHit;
}

int CheckStackEditor(Vector3 blockPos, Vector3 Size){
    int valid = 1;
    if (blockPos.x < 0 || blockPos.x + Size.x > world->width || blockPos.y < 0 || blockPos.y + Size.y > world->height || blockPos.z < 0 || blockPos.z + Size.z > world->depth)
        return 0;
    if (world->map[(int)blockPos.x][(int)blockPos.y][(int)blockPos.z] == NULL)
        return 0;
    int id = world->map[(int)blockPos.x][(int)blockPos.y][(int)blockPos.z]->id;
    int variation = world->map[(int)blockPos.x][(int)blockPos.y][(int)blockPos.z]->variation;
    //check if all block in the stack is valid : not null and have the same id
    for (int i=0;i<Size.x;i++){
        for (int j=0;j<Size.y;j++){
            for (int k=0;k<Size.z;k++){
                if (world->map[(int)blockPos.x+i][(int)blockPos.y+j][(int)blockPos.z+k] == NULL 
                || world->map[(int)blockPos.x+i][(int)blockPos.y+j][(int)blockPos.z+k]->id != id 
                || world->map[(int)blockPos.x+i][(int)blockPos.y+j][(int)blockPos.z+k]->variation != variation){
                    valid = 0;
                    break;
                }
            }
        }
    }
    return valid;
}


List* GenerateStackEditor(){
    printf("Generating code editor\n");
    Block* ***result = (Block ****)malloc(sizeof(Block ***) * world->width);
    for (int i = 0; i < world->width; i++)
    {
        result[i] = (Block ***)malloc(sizeof(Block **) * world->height);
        for (int j = 0; j < world->height; j++)
        {
            result[i][j] = (Block **)malloc(sizeof(Block *) * world->depth);
            for (int k = 0; k < world->depth; k++)
            {
                result[i][j][k] = NULL;
            }
        }
    }

    List* stack = List_new();

    //find stack of block
    for (int i=0;i<world->width;i++){
        for (int j=0;j<world->height;j++){
            for (int k=0;k<world->depth;k++){
                if (world->map[i][j][k] != NULL){
                    int id = world->map[i][j][k]->id;
                    int x = i;
                    int y = j;
                    int z = k;
                    Vector3 SizeStack = {1,1,1};
                    //extend x right
                    while (CheckStackEditor((Vector3){x,y,z},(Vector3){SizeStack.x+1,SizeStack.y,SizeStack.z})){
                        SizeStack.x++;
                    }
                    //extend y up
                    while (CheckStackEditor((Vector3){x,y,z},(Vector3){SizeStack.x,SizeStack.y+1,SizeStack.z})){
                        SizeStack.y++;
                    }
                    //extend z back
                    while (CheckStackEditor((Vector3){x,y,z},(Vector3){SizeStack.x,SizeStack.y,SizeStack.z+1})){
                        SizeStack.z++;
                    }
                    //extend x left
                    while (CheckStackEditor((Vector3){x-1,y,z},(Vector3){SizeStack.x+1,SizeStack.y,SizeStack.z})){
                        x--;
                        SizeStack.x++;
                    }
                    //extend y down
                    while (CheckStackEditor((Vector3){x,y-1,z},(Vector3){SizeStack.x,SizeStack.y+1,SizeStack.z})){
                        y--;
                        SizeStack.y++;
                    }
                    //extend z front
                    while (CheckStackEditor((Vector3){x,y,z-1},(Vector3){SizeStack.x,SizeStack.y,SizeStack.z+1})){
                        z--;
                        SizeStack.z++;
                    }
                    //printf("ID: %d, Pos: (%d,%d,%d), SizeStack: (%d,%d,%d)\n",id,x,y,z,SizeStack.x,SizeStack.y,SizeStack.z);
                    StackPlaceBlock* stackPlaceBlock = (StackPlaceBlock*)malloc(sizeof(StackPlaceBlock));
                    stackPlaceBlock->blockPos = (Vector3){x,y,z};
                    stackPlaceBlock->SizeStack = SizeStack;
                    stackPlaceBlock->ID = id;
                    stackPlaceBlock->variation = world->map[x][y][z]->variation;
                    List_add(stack,stackPlaceBlock);

                    for (int i2=0;i2<SizeStack.x;i2++){
                        for (int j2=0;j2<SizeStack.y;j2++){
                            for (int k2=0;k2<SizeStack.z;k2++){
                                //RemoveBlock(x+i2,y+j2,z+k2);
                                result[x+i2][y+j2][z+k2] = world->map[x+i2][y+j2][z+k2];
                                world->map[x+i2][y+j2][z+k2] = NULL;
                            }
                        }
                    }

                }
            }
        }
    }
    for (int i = 0; i < world->width; i++)
    {
        for (int j = 0; j < world->height; j++)
        {
            for (int k = 0; k < world->depth; k++)
            {
                world->map[i][j][k] = result[i][j][k];
            }
            free(result[i][j]);
        }
        free(result[i]);
    }
    free(result);
    return stack;
}

#ifdef WIN_MODE
#include <windows.h>
//file dialog open
char* FileDialogOpen(char* filter){
    char* result = NULL;
    OPENFILENAME ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    //default path
    char* path = SDL_GetBasePath();
    //add /worlds to path
    //set default path
    char* path2 = NULL;
    if (path!=NULL){
        path2 = (char*)malloc(sizeof(char)*(strlen(path)+strlen("worlds\\")+1));
        strcpy(path2,path);
        strcat(path2,"worlds\\");
        ofn.lpstrInitialDir = path2;
    }
    if (GetOpenFileNameA(&ofn))
    {
        result = (char*)malloc(sizeof(char)*strlen(szFile));
        strcpy(result,szFile);
    }
    if (path!=NULL)
        free(path);
    if (path2!=NULL)
        free(path2);
    return result;
}

//file dialog save
char* FileDialogSave(char* filter){
    char* result = NULL;
    OPENFILENAME ofn;
    char szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrTitle = "Select a File";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    char* path = SDL_GetBasePath();
    //add /worlds to path
    //set default path
    char* path2 = NULL;
    if (path!=NULL){
        path2 = (char*)malloc(sizeof(char)*(strlen(path)+strlen("worlds\\")+1));
        strcpy(path2,path);
        strcat(path2,"worlds\\");
        ofn.lpstrInitialDir = path2;
    }
    if (GetSaveFileNameA(&ofn))
    {
        result = (char*)malloc(sizeof(char)*strlen(szFile));
        strcpy(result,szFile);
    }
    if (path!=NULL)
        free(path);
    if (path2!=NULL)
        free(path2);
    return result;
}
#else
char* FileDialogOpen(char* filter){
    return NULL;
}
char* FileDialogSave(char* filter){
    return NULL;
}
#endif

#ifdef EDITOR_MODE

void InitEditor(){

}

Block* EditorPaintID = NULL;
void EditorUpdate(){
    if (IsKeyDown(eventDataSystem->submitButton))
    {
        //open folder select save file with SDL
        //SaveWorld("worlds/world1.world");
        //LoadWorld("worlds/world1.world");
        /*char* path = FileDialogOpen("World File\0*.world\0");
        if (path!=NULL)
        {
            LoadWorld(path);
            free(path);
        }*/
        char* path = FileDialogSave("World File\0*.world\0");
        if (path!=NULL)
        {
            SaveWorld(path);
            free(path);
        }
    }
    if (BlockHit!=NULL)
    {
        //ctrl
        if (IsKeyPressed(SDLK_LCTRL)){
            if (IsMouseButtonDown(eventDataSystem->mouseLeftClick) && EditorPaintID!=NULL)
            {
                BlockHit->id = EditorPaintID->id;
                BlockHit->variation = EditorPaintID->variation;
            }
            if (IsMouseButtonDown(eventDataSystem->mouseRightClick))
            {
                EditorPaintID = BlockHit;
            }
        }
        //Shift
        else if (IsKeyPressed(SDLK_LSHIFT)){
            if (IsMouseButtonDown(eventDataSystem->mouseLeftClick))
            {
                BlockHit->id = (BlockHit->id+1)%MAX_ID_BLOCK;
            }
        }
        else{
            if (IsMouseButtonDown(eventDataSystem->mouseRightClick))
            {
                RemoveBlock(BlockHit->x/BLOCK_SIZE,BlockHit->y/BLOCK_SIZE,BlockHit->z/BLOCK_SIZE);
                CullingShadowWorld();
            }
            else if (IsMouseButtonDown(eventDataSystem->mouseLeftClick))
            {
                int x = BlockHit->x/BLOCK_SIZE;
                int y = BlockHit->y/BLOCK_SIZE;
                int z = BlockHit->z/BLOCK_SIZE;
                switch (SideOfBlockHit())
                {
                case SIDE_FRONT:
                    if (z-1 >= 0)
                        PlaceBlock(CreateBlock(0,0, 0, 0, 0),x,y,z-1);
                    break;
                case SIDE_LEFT:
                    if (x-1 >= 0)
                        PlaceBlock(CreateBlock(0,0, 0, 0, 0),x-1,y,z);
                    break;
                case SIDE_RIGHT:
                    if (x+1 < world->width)
                        PlaceBlock(CreateBlock(0,0, 0, 0, 0),x+1,y,z);
                    break;
                case SIDE_TOP:
                    if (y+1 < world->height)
                        PlaceBlock(CreateBlock(0,0, 0, 0, 0),x,y+1,z);
                    break;
                case SIDE_BOTTOM:
                    if (y-1 >= 0)
                        PlaceBlock(CreateBlock(0,0, 0, 0, 0),x,y-1,z);
                    break;
                default:
                    break;
                }
                CullingShadowWorld();
            }
        }
        
    }
}

void EditorDraw(){

}

void DestroyEditor(){

}

#endif


clock_t lastTime = 0;
double deltaTime = 0;

double DeltaTime(){
    return deltaTime;
}

double SpeedMultiplier(){
    return 1/max(1.0f-(DeltaTime()*2),0.1f);
}

void ResetDeltaClock(){
    lastTime = clock();
}

int GetFPS(){
    return (int)(1/max(DeltaTime(),0.000001));
}

void DrawFPS(){
    char* fps = (char*)malloc(sizeof(char)*10);
    sprintf(fps,"%d FPS",GetFPS());
    PC_DrawText((const unsigned char*)fps,SCREEN_WIDTH-50,0,PC_WHITE,MainFont);
    free(fps);
}


int MainWorld(){
    FadeOutAlpha((lambdaFunction){Background_DrawWorld, NULL});
    #ifdef EDITOR_MODE
    InitEditor();
    #endif
    while (!world->quit)
    {
        DrawFPS();
        UpdateScreen();
        ClearScreen();
        UpdateInputs();
        UpdateSpriteAnimation();
        #ifdef EDITOR_MODE
        EditorUpdate();
        EditorDraw();
        #endif
        UpdateWorld();
        lastTime = clock();
        DrawWorld();
        if (world->DrawWorldUI != NULL)
        {
            world->DrawWorldUI();
        }
        deltaTime = (double)(clock() - lastTime) / CLOCKS_PER_SEC;
    }
    int q = world->quit;
    ClearScreen();
    DestroyWorld();
    #ifdef EDITOR_MODE
    DestroyEditor();
    #endif
    return q;
}


void SaveWorldFILE(FILE* file){
    #ifdef EDITOR_MODE
    List* stack = GenerateStackEditor();
    fprintf(file,"%d\n",stack->size);
    while (stack->size > 0)
    {
        StackPlaceBlock* stackPlaceBlock = (StackPlaceBlock*)List_pop(stack);
        fprintf(file,"%d %d %d %d %d %d %d %d\n",stackPlaceBlock->ID,stackPlaceBlock->variation ,stackPlaceBlock->blockPos.x,stackPlaceBlock->blockPos.y,stackPlaceBlock->blockPos.z,stackPlaceBlock->SizeStack.x,stackPlaceBlock->SizeStack.y,stackPlaceBlock->SizeStack.z);
        free(stackPlaceBlock);
    }
    List_free(stack);
    #endif
}

void SaveWorld(char* path){
    #ifdef EDITOR_MODE
    char* name = strrchr(path,'\\');
    name[0] = '/';
    //path2 = "../assets/worlds/"+name+".world";
    char* currentPath = SDL_GetBasePath();
    char* path2 = malloc(sizeof(char)*(strlen(currentPath)+strlen(name)+20));
    strcpy(path2,currentPath);
    free(currentPath);
    strcat(path2,"../assets/worlds");
    strcat(path2,name);
    FILE* file2 = fopen(path2,"wb");
    SaveWorldFILE(file2);
    fclose(file2);
    #endif
    FILE* file = fopen(path,"wb");
    SaveWorldFILE(file);
    fclose(file);
}


void LoadWorldFILE(FILE* file){
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    int sizeStack;
    fscanf(file,"%d\n",&sizeStack);
    int ID,variation,x,y,z,sx,sy,sz;
    for (int i = 0; i < sizeStack; i++)
    {
        fscanf(file,"%d %d %d %d %d %d %d %d\n",&ID,&variation,&x,&y,&z,&sx,&sy,&sz);
        for (int i2=0;i2<sx;i2++){
            for (int j2=0;j2<sy;j2++){
                for (int k2=0;k2<sz;k2++){
                    PlaceBlock(CreateBlock(ID,variation, 0, 0, 0),x+i2,y+j2,z+k2);
                }
            }
        }
    }
    #elif defined(CG_MODE) || defined(FX_MODE)
    int sizeStack;
    fread(&sizeStack,sizeof(int),1,file);
    int ID,variation,x,y,z,sx,sy,sz;
    for (int i = 0; i < sizeStack; i++)
    {
        fread(&ID,sizeof(int),1,file);
        fread(&variation,sizeof(int),1,file);
        fread(&x,sizeof(int),1,file);
        fread(&y,sizeof(int),1,file);
        fread(&z,sizeof(int),1,file);
        fread(&sx,sizeof(int),1,file);
        fread(&sy,sizeof(int),1,file);
        fread(&sz,sizeof(int),1,file);
        for (int i2=0;i2<sx;i2++){
            for (int j2=0;j2<sy;j2++){
                for (int k2=0;k2<sz;k2++){
                    PlaceBlock(CreateBlock(ID,variation, 0, 0, 0),x+i2,y+j2,z+k2);
                }
            }
        }
    }
    #endif
}

#if defined(CG_MODE) || defined(FX_MODE)

unsigned char* DeserializeString(FILE* fp) {
    int sizeSTR;
    fread(&sizeSTR, sizeof sizeSTR, 1, fp);
    unsigned char* str = malloc(sizeSTR + 1);
    fread(str, sizeof * str, sizeSTR, fp);
    str[sizeSTR] = '\0';
    return str;
};

void __LoadWorldGintCall(char* path){
    FILE* file = fopen("TOTN/worlds.bin","rb");
    if (file == NULL){
        return;
    }
    int sizeFiles;
    fread(&sizeFiles,sizeof(int),1,file);
    for (int i = 0; i < sizeFiles; i++)
    {
        unsigned char* pathFile = DeserializeString(file);
        if (strcmp(pathFile,path) == 0){
            LoadWorldFILE(file);
            fclose(file);
            free(pathFile);
            return;
        }
        else{
            int sizeStack;
            fread(&sizeStack,sizeof(int),1,file);
            fseek(file, sizeStack*(8*sizeof(int)), SEEK_CUR);
        }
        free(pathFile);
    }
    fclose(file);
}

#endif

void LoadWorld(char* path){
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    FILE* file = fopen(path,"rb");
    LoadWorldFILE(file);
    fclose(file);
    #elif defined(CG_MODE) || defined(FX_MODE)
    gint_world_switch(GINT_CALL(__LoadWorldGintCall,(void *)path));
    #endif
}