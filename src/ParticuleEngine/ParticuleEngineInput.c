#include "ParticuleEngineInput.h"
#include "ParticuleEngine.h"
#include <List.h>

#if defined(WIN_MODE)  || defined(LINUX_MODE)
List* InputEvents = NULL;
List* InputEventsHeld = NULL;
#elif defined(PSP_MODE)
#include "Libs/PSP/MyKeyboard/MyKeyboard.h"
float InputAnalogX() {
    float v = (float)padData.Lx / 255.0f;
    v -= 0.5f;
    v *= 2.0f;

    return v;
}

float InputAnalogY() {
    float v = (float)padData.Ly / 255.0f;
    v -= 0.5f;
    v *= 2.0f;

    return v;
}
#elif defined(NDS_MODE)
#include <nds.h>
#include <stdio.h>
#elif defined(CG_MODE) || defined(FX_MODE)
#include <gint/keyboard.h>
#include <MyKeyboard.h>
#include <stdarg.h>
#include <gint/rtc.h>
List* InputEvents = NULL;
#endif

Vector2 MousePosition = {0,0};

void UpdateInputs()
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
        while (InputEvents->size > 0)
        {
            SDL_Event* event = List_pop(InputEvents);
            if (event->type == SDL_QUIT || (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE))
            {
                exit(0);
            }
            free(event);
        }
        List_clear(InputEvents);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.key.repeat) continue; // Ignore key repeats (holding down a key
            SDL_Event* eventCopy = malloc(sizeof(SDL_Event));
            memcpy(eventCopy, &event, sizeof(SDL_Event));
            List_add(InputEvents, eventCopy);
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN){
                SDL_Event* eventCopy2 = malloc(sizeof(SDL_Event));
                memcpy(eventCopy2, &event, sizeof(SDL_Event));
                List_add(InputEventsHeld, eventCopy2);
            }
            if (event.type == SDL_KEYUP){
                for (List_node*cur=NULL; ForEach(InputEventsHeld,&cur);)
                {
                    SDL_Event* eventHeld = (SDL_Event*)(cur->data);
                    if (eventHeld->key.keysym.sym == event.key.keysym.sym){
                        cur = cur->next;
                        List_remove(InputEventsHeld, eventHeld);
                        free(eventHeld);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP){
                for (List_node*cur=NULL; ForEach(InputEventsHeld,&cur);)
                {
                    SDL_Event* eventHeld = (SDL_Event*)(cur->data);
                    if (eventHeld->button.button == event.button.button){
                        cur = cur->next;
                        List_remove(InputEventsHeld, eventHeld);
                        free(eventHeld);
                    }
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                MousePosition.x = event.motion.x/3;
                MousePosition.y = event.motion.y/3;
            }
        }
    #elif defined(PSP_MODE)
    oldData = padData;
    sceCtrlReadBufferPositive(&padData,1);
    #elif defined(NDS_MODE)
    scanKeys();
    #elif defined(CG_MODE) || defined(FX_MODE)
    //clearevents();
    while (InputEvents->size > 0)
    {
        free(List_pop(InputEvents));
    }
    key_event_t ev;
    while((ev = pollevent()).type != KEYEV_NONE)
    {
        PC_GINT_keyinfo *key = malloc(sizeof(PC_GINT_keyinfo));
        key->type = ev.type;
        key->key = ev.key;
        List_add(InputEvents, key);
    }
    #endif
}

bool IsKeyPressed(int key)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEventsHeld,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == key)
        {
            return true;
        }
    }
    return false;
    #elif defined(PSP_MODE)
    bool current = (padData.Buttons & key) == key;
    bool last = (oldData.Buttons & key) == key;
    return current && last; // Currently pressed, and pressed last time
    #elif defined(NDS_MODE)
    return keysHeld() & ((KEYPAD_BITS)key);
    #elif defined(CG_MODE) || defined(FX_MODE)
    return keydown(key);
    #endif

    return false;
}

int GetInputEvent()
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    if (InputEvents->size > 0)
    {
        SDL_Event* event = List_pop(InputEvents);
        int key = event->key.keysym.sym;
        free(event);
        return key;
    }
    #elif defined(PSP_MODE)
    if (padData.Buttons != oldData.Buttons)
    {
        return padData.Buttons;
    }
    #elif defined(NDS_MODE)
    if (keysDown() != 0)
    {
        return keysDown();
    }
    #elif defined(CG_MODE) || defined(FX_MODE)
    if (InputEvents->size > 0)
    {
        PC_GINT_keyinfo* event = List_pop(InputEvents);
        int key = event->key;
        free(event);
        return key;
    }
    #endif
    return -1;
}

bool IsKeyDown(int key)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
	for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
	{
		SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == key)
        {
            return true;
        }
	}
    return false;
    #elif defined(PSP_MODE)
    bool current = (padData.Buttons & key) == key;
    bool last = (oldData.Buttons & key) == key;
    return current && !last; // Currently pressed, not pressed last time
    #elif defined(NDS_MODE)
    return keysDown() & ((KEYPAD_BITS)key);
    #elif defined(CG_MODE) || defined(FX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        PC_GINT_keyinfo* event = (PC_GINT_keyinfo*)(cur->data);
        if (event->type == KEYEV_DOWN && event->key == key)
        {
            return true;
        }
    }
    return false;
    #endif

    return false;
}

bool IsKeyDownWait(int key)
{
    if (IsKeyDown(key))
    {
        while (IsKeyPressed(key))
        {
            UpdateInputs();
        }
        return true;
    }
    return false;
}

bool IsKeyUp(int key)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_KEYUP && event->key.keysym.sym == key)
        {
            return true;
        }
    }
    return false;
    #elif defined(PSP_MODE)  || defined(LINUX_MODE)
    bool current = (padData.Buttons & key) == key;
    bool last = (oldData.Buttons & key) == key;
    return !current && last; // Currently not pressed, and pressed last time
    #elif defined(NDS_MODE)
    return keysUp() & ((KEYPAD_BITS)key);
    #elif defined(CG_MODE) || defined(FX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        PC_GINT_keyinfo* event = (PC_GINT_keyinfo*)(cur->data);
        if (event->type == KEYEV_UP && event->key == key)
        {
            return true;
        }
    }
    return false;
    #endif

    return false;
}

bool IsMouseButtonPressed(int button)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEventsHeld,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == button)
        {
            return true;
        }
    }
    return false;
    #elif defined(PSP_MODE) || defined(NDS_MODE) || defined(CG_MODE) || defined(FX_MODE)
    return IsKeyPressed(button);
    #endif

    return false;
}

bool IsMouseButtonDown(int button)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == button)
        {
            return true;
        }
    }
    return false;
    #elif defined(PSP_MODE) || defined(NDS_MODE) || defined(CG_MODE) || defined(FX_MODE)
    return IsKeyDown(button);
    #endif

    return false;
}

bool IsMouseButtonUp(int button)
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_MOUSEBUTTONUP && event->button.button == button)
        {
            return true;
        }
    }
    return false;
    #elif defined(PSP_MODE) || defined(NDS_MODE) || defined(CG_MODE) || defined(FX_MODE)
    return IsKeyUp(button);
    #endif

    return false;
}

bool PC_MouseOver(int x, int y, int w, int h)
{
    return MousePosition.x >= x && MousePosition.x <= x+w && MousePosition.y >= y && MousePosition.y <= y+h;
}

char* PC_GetText() //don't forget to free the returned string
{
    #if defined(WIN_MODE)  || defined(LINUX_MODE)
    for (List_node*cur=NULL; ForEach(InputEvents,&cur);)
    {
        SDL_Event* event = (SDL_Event*)(cur->data);
        if (event->type == SDL_TEXTINPUT)
        {
            if (event->text.text==NULL || strlen(event->text.text)==0)
                return NULL;
            char* text = (char*)malloc(sizeof(char)*strlen(event->text.text)+1);
            strcpy(text, event->text.text);
            return text;
        }
    }
    #elif defined(PSP_MODE)
    //C:\pspsdk\psp\sdk\samples\utility\osk
    return scanf_keyboard(PSP_list,"please enter some text","");
    #elif defined(NDS_MODE)
    //C:\devkitPro\examples\nds\input\keyboard\keyboard_stdin
    char* text = (char*)malloc(sizeof(char)*256);
    scanf("%s", text);
    return text;
    //A terminer
    #elif defined(CG_MODE) || defined(FX_MODE)
    return scanf_keyboard();
    #endif
    return NULL;
}