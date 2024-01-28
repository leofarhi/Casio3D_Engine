#include "ParticuleEngine/ParticuleEngine.h"
#include "ParticuleEngine/Resources.h"
#include "InterfaceSystem/InterfaceSystem.h"
#include "InterfaceSystem/EventDataSystem.h"
#include "InterfaceSystem/Sprite.h"

#include "Engine3D/World.h"

//Game includes
#include "Examples/Ressources.h"
#include "Examples/Examples.h"


#if defined(CG_MODE)
#include <gint/clock.h>
//reminder :
/*enum {
    CLOCK_SPEED_UNKNOWN = 0,
    CLOCK_SPEED_F1 = 1,
    CLOCK_SPEED_F2 = 2,
    CLOCK_SPEED_F3 = 3,
    CLOCK_SPEED_F4 = 4,
    CLOCK_SPEED_F5 = 5,
    CLOCK_SPEED_DEFAULT = CLOCK_SPEED_F1,
};*/
#endif

int main(){//int argc, char *argv[]) {
    srand(time(NULL));
    PC_Init();
    initEventDataSystem();
    #if defined(CG_MODE)
    clock_set_speed(CLOCK_SPEED_F4);
    #endif
    //////////////////////////
    
    //Main Game Part
    InitGameConfig();
    //////////////////////////
    //Main Loop
    MainMenu();
    //////////////////////////
    CleanGameConfig();

    //////////////////////////
    PC_Quit();
    #if defined(CG_MODE)
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    #endif
    return 0;
}