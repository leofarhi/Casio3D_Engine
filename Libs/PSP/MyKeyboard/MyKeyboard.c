#include "MyKeyboard.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <string.h>
#include <psputility.h>

#include "Libs/PSP/common/callbacks.h"
#include "Libs/PSP/common/common-sce.h"



char* scanf_keyboard(void * list,char* description,char* defaultTxt)
{
    endFrame();

    int NUM_INPUT_FIELDS= 3;
    int TEXT_LENGTH	=128;
	
	unsigned short intext[NUM_INPUT_FIELDS][TEXT_LENGTH];
	unsigned short outtext[NUM_INPUT_FIELDS][TEXT_LENGTH];
	unsigned short desc[NUM_INPUT_FIELDS][TEXT_LENGTH];
	
	memset(&intext, 0, NUM_INPUT_FIELDS * TEXT_LENGTH * sizeof(unsigned short));
	memset(&outtext, 0, NUM_INPUT_FIELDS * TEXT_LENGTH * sizeof(unsigned short));
	memset(&desc, 0, NUM_INPUT_FIELDS * TEXT_LENGTH * sizeof(unsigned short));
	
	int i;
    for(i = 0;i < NUM_INPUT_FIELDS;i++)
    {
        int j;
        int len = strlen(description);
        for(j = 0;j < len;j++)
        {
            desc[i][j] = description[j];
        }
        desc[i][j] = 0;

        len = strlen(defaultTxt);
        for(j = 0;j < len;j++)
        {
            intext[i][j] = defaultTxt[j];
        }
        intext[i][j] = 0;
    }

	SceUtilityOskData data[NUM_INPUT_FIELDS];
	
	for(i = 0; i < NUM_INPUT_FIELDS;i++)
	{
		memset(&data[i], 0, sizeof(SceUtilityOskData));
		data[i].language = PSP_UTILITY_OSK_LANGUAGE_DEFAULT; // Use system default for text input
		data[i].lines = 1;
		data[i].unk_24 = 1;
		data[i].inputtype = PSP_UTILITY_OSK_INPUTTYPE_ALL; // Allow all input types
		data[i].desc = desc[i];
		data[i].intext = intext[i];
		data[i].outtextlength = TEXT_LENGTH;
		data[i].outtextlimit = 32; // Limit input to 32 characters
		data[i].outtext = outtext[i];
	}
	
	SceUtilityOskParams params;
	memset(&params, 0, sizeof(params));
	params.base.size = sizeof(params);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &params.base.language);
	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &params.base.buttonSwap);
	params.base.graphicsThread = 17;
	params.base.accessThread = 19;
	params.base.fontThread = 18;
	params.base.soundThread = 16;
	params.datacount = NUM_INPUT_FIELDS;
	params.data = data;

    sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_FLAT);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);

	sceUtilityOskInitStart(&params);

    int done = 0;
	while(!done)
	{

		sceGuStart(GU_DIRECT,list);
		sceGuClearColor(0);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		sceGuFinish();
		sceGuSync(0,0);

		switch(sceUtilityOskGetStatus())
		{
			case PSP_UTILITY_DIALOG_INIT:
				break;
			
			case PSP_UTILITY_DIALOG_VISIBLE:
				sceUtilityOskUpdate(1);
				break;
			
			case PSP_UTILITY_DIALOG_QUIT:
				sceUtilityOskShutdownStart();
				break;
			
			case PSP_UTILITY_DIALOG_FINISHED:
				break;
				
			case PSP_UTILITY_DIALOG_NONE:
				done = 1;
				
			default :
				break;
		}

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}

    char *text = (char*)malloc(sizeof(char) * TEXT_LENGTH+1);
    for(i = 0; i < TEXT_LENGTH; i++)
    {
        text[i] = outtext[0][i];
    }
    text[TEXT_LENGTH] = '\0';
    return text;    
}