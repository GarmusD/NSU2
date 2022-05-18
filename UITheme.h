#ifndef UITheme_h
#define UITheme_h

#include "bvs_19.h"
#include "bvs_22.h"

#define BLINK_RATE (500)
#define CURSOR_WIDTH (1)

struct RGBColor{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

#define bb RGBColor ({255, 255, 255})

static const unsigned char* DEFAULT_FONT = &BVS_22_ctf[0];
static const unsigned char* BTN_FONT  = &BVS_19_ctf[0];

const uint8_t DEFAULT_FONT_H = 22;
const uint8_t BTN_FONT_H  = 19;

const RGBColor COLOR_WHITE = {255, 255, 255};
const RGBColor COLOR_BLACK = {0, 0, 0};

//Button
const RGBColor BtnFaceColor = {210, 235, 240};
const RGBColor BtnDisabledColor = {127, 127, 127};
const RGBColor BtnHighlightColor = {255, 0, 0};
const RGBColor BtnBorderColor = {0, 0, 255};
const RGBColor BtnLabelColor = {0, 0, 255};
const RGBColor BorderColor = {0, 0, 0};

//Label
const RGBColor LabelColor = {0, 0, 255};
const RGBColor LabelBGColor = {255, 255, 255};

//Input
const RGBColor InputFontColor = {0, 0, 255};
const RGBColor InputBGColor = {255, 255, 255};
const RGBColor InputBorderColor = {0, 0, 255};

//Graphics
const RGBColor GraphicsColor = {0, 0, 255};
/*const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
const RGBColor btncolor = {0, 0, 0};
*/

//Circulation pumps
const RGBColor CP_ON_Color = {139, 197, 63};
const RGBColor CP_OFF_Color = {255, 0, 0};
const RGBColor CP_Manual_Color = {127, 63, 151};

/*********************************************
* Windows
*********************************************/
#define SIDE_WINDOW_WIDTH 86

const RGBColor SideWindowBackgroudColor = {75, 150, 190};
const RGBColor SideWindowBorderColor = {0, 0, 255};

const RGBColor WindowBackgroudColor = {255, 255, 255};
const RGBColor WindowBorderColor = {0, 0, 255};

const RGBColor ModalWindowColor = {207, 236, 242};

#endif

