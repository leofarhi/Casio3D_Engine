#ifndef MYKEYBOARD_H
#define MYKEYBOARD_H
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/rtc.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/clock.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int key_id(int keycode)
{
	uint col = (keycode & 0x0f) - 1;
	uint row = 9 - ((keycode & 0xf0) >> 4);

	if(col > 5 || row > 8) return -1;
	return 6 * row + col;
}

static uint8_t map_flat[30] = {
	 0,    0,   '(',  ')',  ',',  '=',
	'7',  '8',  '9',   0,    0,    0,
	'4',  '5',  '6',  '*',  '/',   0,
	'1',  '2',  '3',  '+',  '-',   0,
	'0',  '.',  'e',  '-',   0,    0,
};
static uint8_t map_alpha[36] = {
	'a',  'b',  'c',  'd',  'e',  'f',
	'g',  'h',  'i',  'j',  'k',  'l',
	'm',  'n',  'o',   0,    0,    0,
	'p',  'q',  'r',  's',  't',   0,
	'u',  'v',  'w',  'x',  'y',   0,
	'z',  ' ',  '"',   0,    0,    0,
};

char* scanf_keyboard();

#endif