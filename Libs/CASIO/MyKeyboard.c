#include "MyKeyboard.h"

uint32_t keymap_translate(int key, bool shift, bool alpha)
{
	int id = key_id(key);
	if(id < 0) return 0;

	if(!shift && !alpha) {
		/* The first 4 rows have no useful characters */
		return (id < 24) ? 0 : map_flat[id - 24];
	}
	if(shift && !alpha) {
		if(key == KEY_MUL) return '{';
		if(key == KEY_DIV) return '}';
		if(key == KEY_ADD) return '[';
		if(key == KEY_SUB) return ']';
		if(key == KEY_DOT) return '=';
		if(key == KEY_EXP) return 0x3c0; // 'π'
	}
	if(!shift && alpha) {
		/* The first 3 rows have no useful characters */
		return (id < 18) ? 0 : map_alpha[id - 18];
	}
	if(shift && alpha) {
		int c = keymap_translate(key, false, true);
		return (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
	}

	return 0;
}

char* scanf_keyboard(){
    while (keydown(KEY_EXE))
    {
        clearevents();
    }
    char* text = (char*)malloc(sizeof(char)*1);
    text[0] = '\0';
    int shift = 0;
    int Alpha = 0;
    while (!keydown(KEY_EXE))
    {
        dclear(C_BLACK);
        key_event_t ev;
        while((ev = pollevent()).type != KEYEV_NONE)
        {
            if (ev.type == KEYEV_DOWN)
            {
                if (ev.key == KEY_SHIFT)
                {
                    shift = 1;
                }
                else if (ev.key == KEY_ALPHA)
                {
                    Alpha = 1;
                }
                else if (ev.key == KEY_DEL)
                {
                    if (strlen(text) > 0)
                    {
                        text[strlen(text) - 1] = '\0';
                        text = (char*)realloc(text, sizeof(char)*(strlen(text) + 1));
                    }
                }
                else
                {
                    char c = keymap_translate(ev.key, shift, Alpha);
                    if (c != 0)
                    {
                        int len = strlen(text);
                        text = (char*)realloc(text, sizeof(char)*(len + 2));
                        text[len] = c;
                        text[len + 1] = '\0';
                    }
                }
            }
            else if (ev.type == KEYEV_UP)
            {
                if (ev.key == KEY_SHIFT)
                {
                    shift = 0;
                }
                else if (ev.key == KEY_ALPHA)
                {
                    Alpha = 0;
                }
            }
        }
        dtext(0, 0, C_WHITE, (const char*)text);
        dupdate();
    }
    while (keydown(KEY_EXE))
    {
        clearevents();
    }
    return  text;
}