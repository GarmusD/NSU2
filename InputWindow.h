#ifndef InputWindow_h
#define InputWindow_h

#include <UTFT.h>
//#include <UTFT_CTE.h>
#include "Events.h"
#include "windows.h"
#include "UI.h"
#include "Scenario.h"

class InputWindow: public CWindow
{
public:
	InputWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf);
	~InputWindow(void);
	void SetInputFocus();
	void SetInputType(InputType type);
	void SetMaxChars(int max);
	void SetText(char* value);
	char* GetText();
private:
	InputType itype;
	bool shiftstate;
	int maxchars;
	char* text;

	CWindow* win;
	UIInput* input;
	UIButton* btn_all[52];
	UIButton* btn_digits[10];
	UIButton* btn_chars[35];
	UIButton *btn_dot, *btn_underline;
	UIButton *btn_shift, *btn_backspace;
	UIButton *btn_cancel, *btn_ok;
	UIButton *btn_space;

	void setupDigits();
	void setupChars();
	void setupSpecials();
	void HandleOnClick(void* Sender, int x, int y, int tag);

	void ChangeShiftState();
	void AddChar(char c);
	void DelChar();
	bool DotPresent();
};

#endif
