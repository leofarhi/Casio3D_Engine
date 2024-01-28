#ifndef EVENT_DATA_SYSTEM_H
#define EVENT_DATA_SYSTEM_H
#include "../ParticuleEngine/ParticuleEngineInput.h"
#include "../ParticuleEngine/ParticuleEngine.h"

typedef struct Interface Interface;

typedef struct EventDataSystem{
    Interface* selectedUI;
    bool useController;
    //Input :
    int mouseLeftClick;
    int mouseRightClick;
    int submitButton;
    int submitButton2;
    int cancelButton;
    int deleteButton;
    
    int upButton;
    int downButton;
    int leftButton;
    int rightButton;
} EventDataSystem;

extern EventDataSystem* eventDataSystem;

void initEventDataSystem();

#endif