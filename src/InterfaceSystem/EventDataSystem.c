#include "EventDataSystem.h"

EventDataSystem* eventDataSystem = NULL;

void initEventDataSystem(){
    if (eventDataSystem != NULL)
        return;
    eventDataSystem = malloc(sizeof(EventDataSystem));
    eventDataSystem->selectedUI = NULL;
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    eventDataSystem->useController = false;

    eventDataSystem->mouseLeftClick = SDL_BUTTON_LEFT;
    eventDataSystem->mouseRightClick = SDL_BUTTON_RIGHT;
    eventDataSystem->submitButton = SDLK_RETURN;
    eventDataSystem->submitButton2 = SDLK_RETURN;
    eventDataSystem->cancelButton = SDLK_DELETE;
    eventDataSystem->deleteButton = SDLK_BACKSPACE;

    eventDataSystem->upButton = SDLK_UP;
    eventDataSystem->downButton = SDLK_DOWN;
    eventDataSystem->leftButton = SDLK_LEFT;
    eventDataSystem->rightButton = SDLK_RIGHT;
    #elif defined(CG_MODE) || defined(FX_MODE)
    eventDataSystem->useController = true;
    
    eventDataSystem->submitButton = KEY_EXE;
    eventDataSystem->submitButton2 = KEY_OPTN;
    eventDataSystem->cancelButton = KEY_EXIT;
    eventDataSystem->mouseLeftClick = KEY_EXE;
    eventDataSystem->mouseRightClick = KEY_NEG;
    eventDataSystem->deleteButton = KEY_DEL;

    eventDataSystem->upButton = KEY_UP;
    eventDataSystem->downButton = KEY_DOWN;
    eventDataSystem->leftButton = KEY_LEFT;
    eventDataSystem->rightButton = KEY_RIGHT;
    #endif
}