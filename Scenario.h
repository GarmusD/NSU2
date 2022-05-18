#ifndef Scenario_h
#define Scenario_h

#include <UTFT.h>
#include "win_defs.h"

//Graphics commands
#define GC_SET_COLOR 1
#define GC_SET_BG_COLOR 2
#define GC_DRAW_PIXEL 3
#define GC_DRAW_LINE 4
#define GC_DRAW_RECT 5
#define GC_DRAW_ROUND_RECT 6
#define GC_FILL_RECT 7
#define GC_FILL_ROUND_RECT 8
#define GC_DRAW_CIRCLE 9
#define GC_FILL_CIRCLE 10
#define GC_DRAW_ARC 11
#define GC_FILL_ARC 12
#define GC_LOAD_IMAGE 13//from internal sd card
#define GC_DRAW_BITMAP 14//from uint8_t array
#define GC_DRAW_GRAPHICS 15//from commands array

#define GC_OBJECT_WIDTH 25
#define GC_OBJECT_HEIGHT 26
//UI commands

class Scenario
{
public:
	Scenario(UTFT& utft);
	~Scenario(void);
	void LoadScenario(char* name);
	void PlayScenario(int xbase, int ybase, uint8_t* buf);
	void PlayScenario(int xbase, int ybase, Rect region, uint8_t* buf);
private:
	UTFT& tft;
	short getBufShort(uint8_t *buf, int &idx);
	bool InRect(int x, int y, Rect r);
};

#endif

