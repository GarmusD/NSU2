#ifndef UI_h
#define UI_h

#include <UTFT.h>
#include "fonts.h"
#include "UITheme.h"
#include "win_defs.h"
#include "consts.h"
#include "Events.h"
#include "msg.h"
#include "Scenario.h"
#include "TSensors.h"
#include "MAX31855.h"
#include "CirculationPumps.h"
#include "switches.h"
#include "WoodBoilers.h"
/*********************************************
*********************************************/
#define I_AM_PAINTED 1
#define IS_HOVER 2
#define IS_FOCUSED 3
#define FOCUSABLE 4
#define DOWN_ON_ME 5
#define DISABLE_DRAW 6

class CWindow;

enum UIClass
{
	UIC_None,
	UIC_Button,
	UIC_Input,
	UIC_Label,
	UIC_Spin,
	UIC_ImgButton,
	UIC_Graphics,
	UIC_SwitchButton
};

enum InputType{
	IT_NoFocus,
	IT_Text,
	IT_Float,
	IT_Decimal
};

enum TextAlign{
	AlignLeft,
	AlignCenter,
	AlignRight
};

struct Interval
{
	int start;
	int width;
};

typedef void(*OnFocus)();
/*********************************************
*********************************************/
class UIInput;
class UILabel;
/*********************************************
*********************************************/
class Cursor{
public:
	Cursor(UTFT& _tft);
	~Cursor();
	void TimeEvent(uint32_t t);
	void setBgColor(RGBColor clr);
	void setFaceColor(RGBColor clr);
	void turnoff();
	void SetPosition(int x, int y);
	void SetSize(int w, int h);
protected:

private:
	UTFT& tft;
	//UTFT_CTE* cte;
	//Fonts* fonts;
	RGBColor bgclr;
	RGBColor faceclr;  
	int x, y, h;
	uint32_t lastblink;
	boolean visible;
	void drawCursor();
};
/*********************************************
*********************************************/
class UIBase:public Events
{
public:
	UIBase(CWindow* window);
	~UIBase(void);
	TextAlign textAlign;
	const unsigned char* Font;
	uint8_t FontHeight;
	UIClass getUIClass();
	char* getUIID();
	void SetUIID(const char* id);
	virtual void SetSize(int w, int h);
	virtual void GetSize(int& w, int& h);
	virtual void SetPosition(int x, int y);
	virtual void GetPosition(int& x, int& y);
	virtual void TimeEvent(uint32_t t);
	virtual void SetFont(const uint8_t* fnt);
	void SetTag(int val);
	int GetTag();	
	virtual void Invalidate();
	virtual void Draw();
	bool Message(Msg msg);
	void DisableDrawing(bool value);
protected:
	UIClass uiclass;
	UIID uiid;
	int x, y, w, h;

	CWindow* window;
	Fonts fonts;
	//UTFT& tft;
	//UTFT_CTE* cte;
	
	int tag;
	uint8_t flags;
	//char *caption;

	virtual void doOnFocus();
	virtual void doOnLeave();
	virtual void doOnClick(int x, int y);
	void SetFlag(uint8_t flag);
	void ClearFlag(uint8_t flag);
	bool FlagIsSet(uint8_t flag);
	int getWX(int x);//world x
	int getWY(int y);//world y
private:
	bool invalidated;
};
/*********************************************
*********************************************/
class UILabel : public UIBase {
public:
	UILabel(CWindow* window, bool reg = true);
	~UILabel();
	void Draw();
	void SetCaption(const char *text);
	void SetFont(const uint8_t* fnt);
	const char* GetCaption();
	void SetColor(RGBColor faceClr, RGBColor backClr);
	int GetCaptionWidth();
	bool AutoSize;
protected:

private:
	char caption[MAX_LABEL_LENGTH];
	RGBColor fclr, bclr;
	void doOnFocus();
	void doOnLeave();
	void doOnClick(int x, int y);
	int lastwidth;
	Interval interval1, interval2;
	int tout_shift;
};
/*********************************************
*********************************************/
class UIButton: public UIBase{
public:
	UIButton(CWindow* window, bool reg=true);
	~UIButton();
	void SetPosition(int x, int y);
	void SetSize(int w, int h);
	void Draw();
	void SetCaption(const char *text);
	void SetColor(RGBColor fclr, RGBColor bclr, RGBColor brdrclr);
	void SetEnabled(bool value);
protected:
	bool transparent;
	bool enabled;
private:
	UILabel lbl;
	int margin;
	RGBColor f_clr, b_clr, brdr_clr;
	char caption[MAX_CAPTION_LENGTH];
	void setLabelPosition();
	void doOnFocus();
	void doOnLeave();
	void doOnClick(int x, int y);
};
/*********************************************
*********************************************/
class UIInput: public UIBase{
public:
	UIInput(CWindow* window, bool reg=true);
	~UIInput();
	void TimeEvent(uint32_t t);
	void SetInputType(InputType type);
	InputType GetInputType();
	void SetCaption(const char* text);
	const char* GetCaption();
	void AddChar(char c);
	void DelChar();
	void SetPosition(int x, int y);
	void SetSize(int w, int h);
	void SetMaxChars(int max);
	void SetColor(RGBColor clr);
	void SetBackColor(RGBColor clr);
	void SetFocus();
	void ClearFocus();
	void Draw();
private:
	InputType inputType;
	Cursor cursor;
	UILabel label;
	int maxchars;
	int cx, cy;
	RGBColor fclr, bclr;
	void setLabelPosition();
	bool hasFocus;
	//void doOnFocus();
	//void doOnLeave();
	void doOnClick(int x, int y);
};
/*********************************************
*********************************************/
class UITempLabel: public UILabel{
public:
	UITempLabel(CWindow* window, bool reg=true);
	~UITempLabel();
	void Draw();
	void AttachTempSensor(TempSensor* sensor);
	void AttachKType(MAX31855* sensor);
	void SetCenterXPosition(int x, int y);
	void SetPosition(int x, int y);
private:
	TempSensor* tsensor;
	MAX31855 *ktype;
	int cx, cy;
	void doOnClick(int x, int y);
	void HandleTemperatureChange(void* Sender, float t);
};
/*********************************************
*********************************************/
class UISpinEdit: public UIBase{
public:
	UISpinEdit(CWindow* window, bool reg=true);
	~UISpinEdit();
	void Draw();
	bool isFloat;
};
/*********************************************
*********************************************/
class UIGraphics: public UIBase
{
public:
	UIGraphics(CWindow* window, bool reg=true);
	~UIGraphics();
	void SetGraphicsBytes(uint8_t* buf);
	void SetVectorBytes(uint8_t* buf);
	void SetColor(RGBColor color);
	void SetBackColor(RGBColor color);
	void Draw();
private:
	bool isVector;
	Scenario& _scenario;
	uint8_t* buffer;
	RGBColor clr, bclr;
	void doOnClick(int x, int y);
};
/*********************************************
*********************************************/
class UIImgButton: public UIButton{
public:
	UIImgButton(CWindow* window, bool reg=true);
	~UIImgButton();
	void SetBMPBytes(const unsigned short* data);
	void Draw();
private:
	const unsigned short* bmpdata;
};
/*********************************************
*********************************************/
class UITempBar: public UIBase
{
public:
	UITempBar(CWindow* window, bool reg=true);
	~UITempBar();
	void Draw();
	void SetTempSensors(TempSensor *s1, TempSensor *s2, TempSensor *s3);
	void SetTemperatures(float temp1, float temp2, float temp3);
private:
	TempSensor *ts_1, *ts_2, *ts_3;
	float t1, t2, t3;
	int sens_count;
	float RStep1, BStep1, RStep2, BStep2;
	uint8_t r1, r2, r3, b1, b2, b3;
	void calculate();
	void doOnClick(int x, int y);
	void HandleTemperatureChange(void* Sender, float t);
};
/*********************************************
*********************************************/
class UICircPump: public UIBase
{
public:
	UICircPump(CWindow* window, bool reg=true);
	~UICircPump(void);
	void AttachPump(CirculationPump* pump);
	void SetRotation(int val);
	void Draw();
	void SetPosition(int x, int y);
	void SetStatus(Status status);
private:
	CirculationPump* cp;
	UIGraphics gr;
	uint8_t* g_data;
	Status cp_status;
	bool manual;
	void doOnClick(int x, int y);
	void HandleStatusChange(void *Sender, Status status);
};

/*********************************************
*********************************************/
class UILadomat : public UIBase
{
public:
	UILadomat(CWindow* window, bool reg = true);
	~UILadomat(void);
	void Draw();
	void SetPosition(int x, int y);
	void AttachWoodBoiler(CWoodBoiler* woodboilder);
private:
	CWoodBoiler* wb;
	UIGraphics gr;
	uint8_t* g_data;
	Status cp_status;
	bool manual;
	void doOnClick(int x, int y);
	void HandleStatusChange(void *Sender, Status status);
};

/*********************************************
*********************************************/
class UIVentilator : public UIBase
{
public:
	UIVentilator(CWindow* window, bool reg = true);
	~UIVentilator(void);
	void Draw();
	void SetPosition(int x, int y);
	void AttachWoodBoiler(CWoodBoiler *woodboiler);
private:
	CWoodBoiler* wb;
	UIGraphics gr;
	uint8_t* g_data;
	Status cp_status;
	bool manual;
	void doOnClick(int x, int y);
	void HandleStatusChange(void *Sender, Status status);
};

/*********************************************
*********************************************/
class UIStatusGraphics: public UIGraphics
{
public:
	UIStatusGraphics(CWindow* window, bool reg=true);
	~UIStatusGraphics(void);
	//void Set
private:
	Events* obj;
	uint8_t* g_data;
	Status status;
	bool manual;
	void doOnClick(int x, int y);
	void HandleStatusChange(void *Sender, Status status);
};

/*********************************************
*********************************************/
class UISwitchButton: public UIImgButton{
public:
	UISwitchButton(CWindow* window, bool reg=true);
	~UISwitchButton();
	void SetOnBMPBytes(const unsigned short* data);
	void SetOffBMPBytes(const unsigned short* data);
	//void SetState(bool value);
	//void ChangeState();
	//bool GetState();
	void SetSwitch(Switch* value);
private:
	Status state;
	Switch* swth;
	const unsigned short* bmpOndata;
	const unsigned short* bmpOffdata;
	void doOnClick(int x, int y);
	void HandleStatusChange(void* Sender, Status value);
};
/*********************************************
*********************************************/
/*********************************************
*********************************************/
/*********************************************
*********************************************/
#endif

