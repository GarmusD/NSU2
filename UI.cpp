#include "UI.h"
#include "graphics.h"
#include "ftoa.h"
#include "itoa.h"
#include "windows.h"
#include "consts.h"

/*********************************************
*********************************************/
void UIBase::TimeEvent(uint32_t t)
{
}

void UIBase::SetTag(int val){
	tag = val;
}

int UIBase::GetTag(){
	return tag;
}

bool UIBase::Message(Msg msg)
{
	//translate world coors to client
	int tx, ty;
	Rect r = window->getClientArea();
	tx = msg.x - r.Left;
	ty = msg.y - r.Top;
	//check 
	if (tx >= this->x && tx <= (this->x + this->w) &&
		ty >= this->y && ty <= (this->y + this->h))
	{
		if(msg.type == TouchDown){
			SetFlag(DOWN_ON_ME);
		}
		if(!FlagIsSet(IS_HOVER)){
			SetFlag(IS_HOVER);
			doOnFocus();
		}
		if(msg.type == TouchUp){
			if(FlagIsSet(IS_HOVER)){
				ClearFlag(IS_HOVER);
				doOnLeave();
			}
			if(FlagIsSet(DOWN_ON_ME))
			{
				ClearFlag(DOWN_ON_ME);
				doOnClick(tx - x, ty - y);
			}
		}
		return true;
	}
	if(FlagIsSet(IS_HOVER))
	{
		ClearFlag(IS_HOVER);
		doOnLeave();
	}
	if(msg.type == TouchUp)
	{
		ClearFlag(DOWN_ON_ME);
	}
	return false;
}

void UIBase::DisableDrawing(bool value)
{
	if(value)
	{
		SetFlag(DISABLE_DRAW);
	}
	else
	{
		ClearFlag(DISABLE_DRAW);
		if(invalidated) Invalidate();
	}
}

void UIBase::doOnFocus()
{
}

void UIBase::doOnLeave()
{
}

void UIBase::doOnClick(int x, int y)
{
}

void UIBase::SetSize(int w, int h)
{
	this->w = w;
	this->h = h;
}

void UIBase::GetSize(int& w, int& h)
{
	w = this->w;
	h = this->h;
}

void UIBase::SetPosition(int x, int y)
{	
	this->x = x;
	this->y = y;
}

void UIBase::GetPosition(int& x, int& y)
{
	x = this->x;
	y = this->y;
}

int UIBase::getWX(int x)//world x
{
	Rect r = window->getClientArea();
	return x+r.Left;
}

int UIBase::getWY(int y)//world y
{
	Rect r = window->getClientArea();
	return y+r.Top;
}

void UIBase::SetFlag(uint8_t flag)
{
	bitSet(flags, flag);
}

void UIBase::ClearFlag(uint8_t flag)
{
	bitClear(flags, flag);
}

bool UIBase::FlagIsSet(uint8_t flag)
{
	return bitRead(flags, flag);
}

void UIBase::Draw()
{
}

void UIBase::Invalidate()
{
	if(FlagIsSet(DISABLE_DRAW))
	{
		invalidated = true;
		return;
	}
	invalidated = false;
	if (FlagIsSet(I_AM_PAINTED))
	{
		Draw();
	}	
}

UIClass UIBase::getUIClass()
{
	return uiclass;
}

char* UIBase::getUIID()
{
	return uiid;
}

void UIBase::SetUIID(const char* id)
{
	strlcpy(uiid, id, MAX_UUID_LENGTH);
}

void UIBase::SetFont(const uint8_t* fnt)
{
	Font = fnt;
	FontHeight = fonts.GetFontHeight(Font);
}

UIBase::UIBase(CWindow* window):fonts(window->GetUTFT())/*, tft(window->GetUTFT())*/
{
	Log.debug("UIBase::UIBase(Window* window):fonts(window->GetUTFT())");
	this->window = window;
	Font = DEFAULT_FONT;
	FontHeight = fonts.GetFontHeight(Font);
	x = 0; y = 0; w = 0; h = 0;
	tag = -1;
	flags = 0;
	uiid[0] = 0;
	uiclass = UIC_None;
	invalidated = false;
}

UIBase::~UIBase(void)
{
	RemoveOnClickHandler(this);
	RemoveStatusChangeHandler(this);
	RemoveTempChangeHandler(this);
}

/*********************************************
*********************************************/
Cursor::Cursor(UTFT& _tft):tft(_tft)
{
	visible = false;
	lastblink = 0;
	this->bgclr = WindowBackgroudColor;
	this->faceclr = WindowBorderColor;
}

Cursor::~Cursor()
{
}

void Cursor::TimeEvent(uint32_t t)
{
	if(abs(t-lastblink) >= BLINK_RATE){
		lastblink = t;
		visible = !visible;
		drawCursor();
	}
}

void Cursor::SetSize(int w, int h)
{
	this->h = h;
}

void Cursor::setBgColor(RGBColor clr)
{
	bgclr = clr;
}

void Cursor::setFaceColor(RGBColor clr)
{
	faceclr = clr;
}

void Cursor::turnoff()
{
	if(!visible) return;
	visible = false;
	tft.setColor(bgclr.R, bgclr.G, bgclr.B);
	tft.fillRect(x, y, x+CURSOR_WIDTH, y+DEFAULT_FONT_H);
}

void Cursor::SetPosition(int x, int y)
{
	turnoff();
	this->x = x;
	this->y = y;
	lastblink = 0;
}

void Cursor::drawCursor()
{
	if(visible)
	{
		tft.setColor(faceclr.R, faceclr.G, faceclr.B);
	}else{
		tft.setColor(bgclr.R, bgclr.G, bgclr.B);
	}
	tft.fillRect(x, y, x+CURSOR_WIDTH, y+DEFAULT_FONT_H);
}

/*********************************************
*********************************************/
UIButton::UIButton(CWindow* window, bool reg):UIBase(window), lbl(window, false)
{
	if(reg) window->AddUIElement(this);
	enabled = true;
	transparent = false;
	uiclass = UIC_Button;
	w = 100;
	h = 44;
	margin = 6;
	f_clr = BtnLabelColor;
	b_clr = BtnFaceColor;
	brdr_clr = BtnBorderColor;
	lbl.SetColor(f_clr, b_clr);
	lbl.SetFont(BTN_FONT);
	lbl.textAlign = AlignCenter;
	setLabelPosition();
}

UIButton::~UIButton()
{
}

void UIButton::SetCaption(const char *text)
{
	lbl.SetCaption(text);
}

void UIButton::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	setLabelPosition();
	Invalidate();
}

void UIButton::SetSize(int w, int h)
{
	this->w = w;
	this->h = h;
	setLabelPosition();
	Invalidate();
}

void UIButton::setLabelPosition()
{
	uint8_t marginy = ceil( (h - lbl.FontHeight) / 2 );
	if(marginy < margin) marginy = margin;
	uint8_t marginx = margin;
	int lx = x + marginx;
	int ly = y + marginy-1;
	lbl.SetPosition(lx, ly);
	lbl.SetSize(w-marginx*2, BTN_FONT_H);
}

void UIButton::SetColor(RGBColor fclr, RGBColor bclr, RGBColor brdrclr)
{
	f_clr = fclr;
	b_clr = bclr;
	brdr_clr = brdrclr;
	lbl.SetColor(f_clr, b_clr);
	Invalidate();
}

void UIButton::SetEnabled(bool value)
{
	enabled = value;
	if(enabled)
	{
		lbl.SetColor(f_clr, b_clr);
	}
	else
	{
		lbl.SetColor(f_clr, BtnDisabledColor);
	}
	Invalidate();
}

void UIButton::Draw()
{
	if (!window->IsActive())
		return;

	if(!enabled)
	{
		if(!transparent){
			window->GetUTFT().setColor(BtnDisabledColor.R, BtnDisabledColor.G, BtnDisabledColor.B);
			window->GetUTFT().fillRoundRect(getWX(x), getWY(y), getWX(x)+w, getWY(y)+h);
		}
		window->GetUTFT().setColor(brdr_clr.R, brdr_clr.G, brdr_clr.B);
	}
	else{
		if(!transparent){
			window->GetUTFT().setColor(b_clr.R, b_clr.G, b_clr.B);
			window->GetUTFT().fillRoundRect(getWX(x), getWY(y), getWX(x)+w, getWY(y)+h);
		}
		if(FlagIsSet(IS_HOVER))
		{
			window->GetUTFT().setColor(BtnHighlightColor.R, BtnHighlightColor.G, BtnHighlightColor.B);
		}else
		{
			window->GetUTFT().setColor(brdr_clr.R, brdr_clr.G, brdr_clr.B);
		}
	}
	window->GetUTFT().drawRoundRect(getWX(x), getWY(y), getWX(x)+w, getWY(y)+h);
	window->GetUTFT().drawRoundRect(getWX(x)+2, getWY(y)+2, getWX(x)+w-2, getWY(y)+h-2);
	lbl.Draw();
	SetFlag(I_AM_PAINTED);
}

void UIButton::doOnFocus()
{
	window->GetUTFT().setColor(BtnHighlightColor.R, BtnHighlightColor.G, BtnHighlightColor.B);
	window->GetUTFT().drawRoundRect(getWX(x), getWY(y), getWX(x)+w, getWY(y)+h);
	window->GetUTFT().drawRoundRect(getWX(x)+2, getWY(y)+2, getWX(x)+w-2, getWY(y)+h-2);
}

void UIButton::doOnLeave()
{
	window->GetUTFT().setColor(brdr_clr.R, brdr_clr.G, brdr_clr.B);
	window->GetUTFT().drawRoundRect(getWX(x), getWY(y), getWX(x)+w, getWY(y)+h);
	window->GetUTFT().drawRoundRect(getWX(x)+2, getWY(y)+2, getWX(x)+w-2, getWY(y)+h-2);
}

void UIButton::doOnClick(int x, int y)
{
	if(enabled) DispatchOnClick(this, x, y, tag);
}
/*********************************************
*********************************************/
UIInput::UIInput(CWindow* window, bool reg):UIBase(window), cursor(window->GetUTFT()), label(window, false)
{
	if(reg) window->AddUIElement(this);
	inputType = IT_Text;
	uiclass = UIC_Input;
	fclr = InputFontColor;
	bclr = InputBGColor;
	//cursor = new Cursor(window->GetUTFT());
	//label = new UILabel(window, false);
	label.SetColor(InputFontColor, InputBGColor);
	cursor.setBgColor(bclr);
	cursor.setFaceColor(fclr);
	maxchars = -1;
	inputType = IT_Text;
}

UIInput::~UIInput()
{
	cursor.turnoff();
}

void UIInput::SetCaption(const char* text)
{
	cursor.turnoff();
	label.SetCaption(text);
	cursor.SetPosition(cx+label.GetCaptionWidth()+1, cy);
	Invalidate();
}

const char* UIInput::GetCaption()
{
	return label.GetCaption();
}

void UIInput::SetMaxChars(int max)
{
	maxchars = max;
}

void UIInput::AddChar(char c)
{
	//char *tmp = label.GetCaption();
	//uint8_t l = strlen(tmp);
	//if( l >= MAX_LABEL_LENGTH-1) return;	
	//tmp[l] = c;
	//tmp[l+1] = 0;
}

void UIInput::DelChar()
{
	//char *tmp = label.GetCaption();
	//uint8_t l = strlen(tmp);
	//if(l == 0) return;
	//tmp[l-1] = 0;
}

void UIInput::SetInputType(InputType type)
{
	inputType = type;
}

InputType UIInput::GetInputType()
{
	return inputType;
}

void UIInput::SetColor(RGBColor clr)
{
	fclr = clr;
	label.SetColor(fclr, bclr);
	cursor.setFaceColor(fclr);
	Invalidate();
}

void UIInput::SetBackColor(RGBColor clr)
{
	bclr = clr;
	label.SetColor(fclr, bclr);
	cursor.setBgColor(bclr);
	Invalidate();
}

void UIInput::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	setLabelPosition();
	Invalidate();
}

void UIInput::SetSize(int w, int h)
{
	if(h < FontHeight+6) h = FontHeight+6;
	this->w = w;
	this->h = h;
	setLabelPosition();
	Invalidate();
}

void UIInput::setLabelPosition()
{
	uint8_t marginy = 3;
	uint8_t marginx = 3;
	marginy = ceil( (h - FontHeight) / 2 );
	int lx = x + marginx;
	int ly = y + marginy-1;
	cx = getWX(lx);
	cy = getWY(ly);
	label.SetPosition(lx, ly);
	label.SetSize(w-marginx*2, FontHeight);
	cursor.SetPosition(cx+label.GetCaptionWidth()+1, cy);
}

void UIInput::Draw()
{
	if (!window->IsActive())
		return;

	int wx = getWX(x);
	int wy = getWY(y);
	window->GetUTFT().setColor(bclr.R, bclr.G, bclr.B);
	window->GetUTFT().fillRoundRect(wx, wy, wx+w, wy+h);
	window->GetUTFT().setColor(InputBorderColor.R, InputBorderColor.G, InputBorderColor.B);
	window->GetUTFT().drawRoundRect(wx, wy, wx+w, wy+h);
	label.Draw();
}

void UIInput::TimeEvent(uint32_t t)
{
	if(hasFocus && !FlagIsSet(DISABLE_DRAW))
	{
		cursor.TimeEvent(t);
	}
}

void UIInput::SetFocus()
{
	hasFocus = true;
}

void UIInput::ClearFocus()
{
	hasFocus = false;
	cursor.turnoff();
}

void UIInput::doOnClick(int x, int y)
{
	DispatchOnClick(this, x, y, tag);
}
/*********************************************
*********************************************/
UILabel::UILabel(CWindow* window, bool reg):UIBase(window)
{
	if(reg) window->AddUIElement(this);
	uiclass = UIC_Label;
	textAlign = AlignLeft;
	AutoSize = false;
	//caption = new char[1]; caption is set in UIBase
	caption[0] = 0;
	interval1.width = 0;
	interval2.width = 0;
	lastwidth = 0;
	fclr = LabelColor;
	bclr = WindowBackgroudColor;
	h = FontHeight;
}

UILabel::~UILabel()
{
}

void UILabel::SetCaption(const char *text)
{
	uint len = strlen(text);
	if (len > MAX_LABEL_LENGTH) len = MAX_LABEL_LENGTH;
	int strw = fonts.GetTextArrayWidth(text, Font);
	if(strw > w)//string is longer when allocated width, truncating
	{		
		int cw, neww = 0, captidx = 0, csp;
		csp = fonts.GetCharSpacing();
		for(uint i=0; i < len; i++)
		{
			cw = fonts.GetCharWidth(text[i], Font);
			if(i > 0)
			{
				if((neww + csp + cw) < this->w)
				{
					neww += csp + cw;
					caption[captidx] = text[i];
					captidx++;
				}else break;
			}
			else
			{
				if((neww + cw) < this->w)
				{
					neww += cw;
					caption[captidx] = text[i];
					captidx++;
				}else break;
			}
		}
		caption[captidx] = 0;//null terminating
		strw = neww;
	}
	else
	{
		strlcpy(caption, text, MAX_LABEL_LENGTH);
	}

	if(lastwidth > strw)//string before was longer, clear area
	{
		//
		int diff = lastwidth - strw;
		switch (textAlign)
		{
		case AlignLeft:
			interval1.start = x + strw;
			interval1.width = diff;
			interval2.width = 0;
			break;
		case AlignCenter:
			{
				int sidediff = ceil(diff / 2.0);
				int freespace = ceil((w - strw) / 2.0);
				interval1.start = x + (freespace - sidediff)-1;
				interval1.width = sidediff;
				interval2.start = x + freespace + strw;
				interval2.width = sidediff;
			}
			break;
		case AlignRight:
			interval1.start = x + w - strw - diff;
			interval1.width = diff;
			interval2.width = 0;
			break;
		default:
			break;
		}		
	}
	lastwidth = strw;
	//set text out shift
	switch (textAlign)
	{
	case AlignLeft:
		tout_shift = 0;
		break;
	case AlignCenter:
		tout_shift = (w - strw) / 2;
		break;
	case AlignRight:
		tout_shift = w - strw;
		break;
	default:
		break;
	}
	Invalidate();
}

const char* UILabel::GetCaption()
{
	return caption;
}

int UILabel::GetCaptionWidth()
{
	return lastwidth;
}

void UILabel::SetColor(RGBColor faceClr, RGBColor backClr)
{
	fclr = faceClr;
	bclr = backClr;
}

void UILabel::SetFont(const uint8_t* fnt)
{
	Font = fnt;
	FontHeight = fonts.GetFontHeight(fnt);
	h = FontHeight;
}

void UILabel::Draw()
{
	if (!window->IsActive())
		return;
	//Log.debug("UILabel::Draw() - Window ACTIVE. OK.");
	int wx = getWX(x);
	int wy = getWY(y);
	if(interval1.width > 0)
	{
		window->GetUTFT().setColor(bclr.R, bclr.G, bclr.B);
		window->GetUTFT().fillRect(getWX(interval1.start), wy, getWX(interval1.start)+interval1.width, wy+h);
		if(interval2.width > 0)
		{
			window->GetUTFT().fillRect(getWX(interval2.start), wy, getWX(interval2.start)+interval2.width, wy+h);
		}
	}

	window->GetUTFT().setColor(fclr.R, fclr.G, fclr.B);
	window->GetUTFT().setBackColor(bclr.R, bclr.G, bclr.B);
	if(caption)
	{
		fonts.Put_Text_array(caption, wx + tout_shift, wy, Font);
	}
	SetFlag(I_AM_PAINTED);
}

void UILabel::doOnFocus()
{
}

void UILabel::doOnLeave()
{
}

void UILabel::doOnClick(int x, int y)
{
	HandleOnClick(this, x, y, tag);
}
/*********************************************
*********************************************/
//const char TSIGN[3] = {32, 176, 67};
const char TSIGN[2] = {176, 67};

UITempLabel::UITempLabel(CWindow* window, bool reg):UILabel(window, false)
{
	if(reg) window->AddUIElement(this);
	textAlign = AlignCenter;
	char c[15] = "-99.9";
	strcat(c, TSIGN);
	w = fonts.GetTextArrayWidth(c, Font)+4;
	h = FontHeight;
	SetCaption(c);
}

UITempLabel::~UITempLabel()
{
	if(tsensor)
	{
		tsensor->RemoveTempChangeHandler(this);
	}
}

void UITempLabel::SetCenterXPosition(int x, int y)
{
	cx = x; cy = y;
	this->y = y+3;
	this->x = x - (w / 2) - 3;
}

void UITempLabel::SetPosition(int x, int y)
{
	this->x = x+3;
	this->y = y+3;
}

void UITempLabel::Draw()
{
	if (!window->IsActive())
		return;

	UILabel::Draw();
	window->GetUTFT().setColor(WindowBorderColor.R, WindowBorderColor.G, WindowBorderColor.B);
	window->GetUTFT().drawRect(getWX(x)-3, getWY(y)-3, getWX(x)+w+6, getWY(y)+FontHeight+6);
}

void UITempLabel::AttachTempSensor(TempSensor* sensor)
{
	tsensor = sensor;
	if(tsensor)
	{
		tsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(tsensor, tsensor->getTemp());
	}
}

void UITempLabel::AttachKType(MAX31855* sensor)
{
	ktype = sensor;
	if(ktype)
	{
		ktype->AddTempChangeHandler(this);
		HandleTemperatureChange(ktype, ktype->GetTemp());
	}
	else
	{
		char c[15] = "-9999";
		strcat(c, TSIGN);
		w = fonts.GetTextArrayWidth(c, Font)+4;
		UILabel::SetCaption(c);
	}
}

void UITempLabel::doOnClick(int x, int y)
{
	DispatchOnClick(this, x, y, tag);
}

void UITempLabel::HandleTemperatureChange(void* Sender, float t)
{
	char temp[10];
	if(Sender == ktype)
	{
		itoa((int)t, temp, 10);
	}
	else
	{
		ftoa(temp, t, 1);
	}
	strcat(temp, TSIGN);
	UILabel::SetCaption(temp);
}
/*********************************************
*********************************************/
UISpinEdit::UISpinEdit(CWindow* window, bool reg):UIBase(window)
{
	if(reg) window->AddUIElement(this);
	uiclass = UIC_Spin;
	//inputType = NoFocus;
}

UISpinEdit::~UISpinEdit()
{
}

/*********************************************
*********************************************/
UIImgButton::UIImgButton(CWindow* window, bool reg):UIButton(window, false)
{
	if(reg) window->AddUIElement(this);
	uiclass = UIC_ImgButton;
	bmpdata = NULL;
	transparent = true;
}

UIImgButton::~UIImgButton()
{
	
}

void UIImgButton::SetBMPBytes(const unsigned short *data)
{
	bmpdata = data;
	if(bmpdata != NULL){
		w = bmpdata[0];
		h = bmpdata[1];
	}
	Invalidate();
}

void UIImgButton::Draw()
{
	if (!window->IsActive())
		return;

	fonts.DrawColorBitmap(getWX(x), getWY(y), bmpdata);
	UIButton::Draw();
}


/*********************************************
*********************************************/
UIGraphics::UIGraphics(CWindow* window, bool reg):UIBase(window),_scenario(window->getScenario())
{
	if(reg) window->AddUIElement(this);
	uiclass = UIC_Graphics;
	//_scenario = window->getScenario();
	clr = GraphicsColor;
	bclr = WindowBackgroudColor;
}

UIGraphics::~UIGraphics()
{
}

void UIGraphics::SetColor(RGBColor color)
{
	clr = color;
	Invalidate();
}

void UIGraphics::SetBackColor(RGBColor color)
{
	bclr = color;
	Invalidate();
}

void UIGraphics::SetGraphicsBytes(uint8_t* buf)
{
	isVector = false;
	buffer = buf;
}

void UIGraphics::SetVectorBytes(uint8_t* buf)
{
	isVector = true;
	buffer = buf;
}

void UIGraphics::Draw()
{
	if (!window->IsActive())
		return;

	window->GetUTFT().setBackColor(bclr.R, bclr.G, bclr.B);
	window->GetUTFT().setColor(clr.R, clr.G, clr.B);
	if (isVector)
	{
		_scenario.PlayScenario(getWX(x), getWY(y), buffer);		
	}
	else
	{
		fonts.DrawMonoBitmap(getWX(x), getWY(y), buffer);
	}
	SetFlag(I_AM_PAINTED);
}

void UIGraphics::doOnClick(int x, int y)
{
	DispatchOnClick(this, x, y, tag);
}
/*********************************************
*********************************************/
UITempBar::UITempBar(CWindow* window, bool reg):UIBase(window)
{
	if(reg) window->AddUIElement(this);
	ts_1 = NULL;
	ts_2 = NULL;
	ts_3 = NULL;
	t1 = 0.0f;
	t2 = 0.0f;
	t3 = 0.0f;
	sens_count = 0;
	calculate();
}

UITempBar::~UITempBar()
{
	if(ts_1)
	{
		ts_1->RemoveTempChangeHandler(this);
	}
	if(ts_2)
	{
		ts_2->RemoveTempChangeHandler(this);
	}
	if(ts_3)
	{
		ts_3->RemoveTempChangeHandler(this);
	}
}

void UITempBar::SetTemperatures(float temp1, float temp2, float temp3)
{
	t1 = temp1;
	t2 = temp2;
	t3 = temp3;
	calculate();
	Invalidate();
}

void UITempBar::Draw()
{
	if (!window->IsActive())
		return;

	int wx = getWX(x);
	int wy = getWY(y);
	window->GetUTFT().setColor(WindowBorderColor.R, WindowBorderColor.G, WindowBorderColor.B);
	window->GetUTFT().drawRect(wx, wy, wx+w, wy+h);
	float r, b;
	int middle = wy+h/2;
	int steps = middle - wy;
	RStep1 = (float)(r1 - r2) / (float)steps;
	RStep2 = (float)(r2 - r3) / (float)steps;

	BStep1 = (float)(b1 - b2) / (float)steps;
	BStep2 = (float)(b2 - b3) / (float)steps;

	r = r1;
	b = b1;
	for(int i=wy+1; i < middle; i++)
	{
		window->GetUTFT().setColor((uint8_t)r, 0, (uint8_t)b);
		window->GetUTFT().drawLine(wx+1, i, wx+w-1, i);
		r -= RStep1;
		b -= BStep1;
	}

	r = r2;
	b = b2;
	for(int i=middle; i <= wy+h-1; i++)
	{
		window->GetUTFT().setColor((uint8_t)r, 0, (uint8_t)b);
		window->GetUTFT().drawLine(wx+1, i, wx+w-1, i);
		r -= RStep2;
		b -= BStep2;
	}
	SetFlag(I_AM_PAINTED);
}

void UITempBar::SetTempSensors(TempSensor *s1, TempSensor *s2, TempSensor *s3)
{
	if(s1)
	{
		ts_1 = s1;
		ts_1->AddTempChangeHandler(this);
		sens_count++;
		HandleTemperatureChange(s1, s1->getTemp());
	}
	if(s2)
	{
		ts_2 = s2;
		ts_2->AddTempChangeHandler(this);
		sens_count++;
		HandleTemperatureChange(s2, s2->getTemp());
	}
	if(s3)
	{
		ts_3 = s3;
		ts_3->AddTempChangeHandler(this);
		sens_count++;
		HandleTemperatureChange(s3, s3->getTemp());
	}
	calculate();
	Invalidate();
}

void UITempBar::HandleTemperatureChange(void* Sender, float t)
{
	if(Sender == ts_1)
	{
		t1 = t;
	}
	if(Sender == ts_2)
	{
		t2 = t;
	}
	if(Sender == ts_3)
	{
		t3 = t;
	}
	calculate();
	Invalidate();
}

void UITempBar::calculate()
{
	if(sens_count == 2)
	{
		t2 = t3 + ((t1 - t3) / 2);
	}
	r1 = map(constrain(t1, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 0, 255);
	r2 = map(constrain(t2, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 0, 255);
	r3 = map(constrain(t3, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 0, 255);
	b1 = map(constrain(t1, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 255, 0);
	b2 = map(constrain(t2, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 255, 0);
	b3 = map(constrain(t3, MIN_BAR_TEMP, MAX_BAR_TEMP), MIN_BAR_TEMP, MAX_BAR_TEMP, 255, 0);
}

void UITempBar::doOnClick(int x, int y)
{
	DispatchOnClick(this, x, y, tag);
}

/*********************************************
*********************************************/
UICircPump::UICircPump(CWindow* window, bool reg):UIBase(window), gr(window, false)
{
	if(reg) window->AddUIElement(this);
	//gr = new UIGraphics(window, false);
	gr.SetSize(41, 41);
	w = 41; h = 41;
	g_data = (uint8_t*)g_circulation1;
	gr.SetVectorBytes(g_data);
	cp = NULL;
	manual = false;
	cp_status = STATUS_OFF;
}

UICircPump::~UICircPump(void)
{
	//delete gr;
	if(cp)
	{
		cp->RemoveStatusChangeHandler(this);
	}
}


void UICircPump::SetRotation(int val)
{
	switch (val)
	{
	case 1:
		g_data = (uint8_t*)g_circulation1;
		break;
	case 2:
		g_data = (uint8_t*)g_circulation2;
		break;
	case 3:
		g_data = (uint8_t*)g_circulation3;
		break;
	case 4:
		g_data = (uint8_t*)g_circulation4;
		break;
	default:
		break;
	}
	gr.SetVectorBytes(g_data);
	Invalidate();
}

void UICircPump::Draw()
{
	if (!window->IsActive())
		return;

	if (!cp)
	{
		gr.SetColor(COLOR_BLACK);
	}
	else if(cp_status == STATUS_MANUAL)
	{
		gr.SetColor(CP_Manual_Color);
	}
	else if (cp_status == STATUS_ON)
	{
		gr.SetColor(CP_ON_Color);
	}
	else if(cp_status == STATUS_OFF)
	{
		gr.SetColor(CP_OFF_Color);
	}
	else if(cp_status == STATUS_DISABLED)
	{
		gr.SetColor(CP_OFF_Color);
	}
	else
	{
		gr.SetColor(COLOR_BLACK);
	}
	gr.Draw();
	SetFlag(I_AM_PAINTED);
}

void UICircPump::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	gr.SetPosition(x, y);
}

void UICircPump::SetStatus(Status status)
{
	HandleStatusChange(NULL, status);
}

void UICircPump::doOnClick(int x, int y)
{
	if(cp)
	{
		cp->SwitchManualMode();
		//Invalidate();
	}
}

void UICircPump::AttachPump(CirculationPump* pump)
{
	if(cp)
	{
		cp->RemoveStatusChangeHandler(this);
	}
	cp = pump;
	if(cp)
	{
		cp->AddStatusChangeHandler(this);
		HandleStatusChange(cp, cp->getStatus());
		Log.debug("UICircPump::AttachPump(CirculationPump* pump) OK");
	}
	else
	{
		Log.debug("UICircPump::AttachPump(CirculationPump* pump = NULL).");
	}
}

void UICircPump::HandleStatusChange(void *Sender, Status status)
{
	cp_status = status;
	Invalidate();
}

/*********************************************
*********************************************/
UILadomat::UILadomat(CWindow* window, bool reg) :UIBase(window), gr(window, false)
{
	if (reg) window->AddUIElement(this);
	//gr = new UIGraphics(window, false);
	gr.SetSize(61, 85);
	w = 61; h = 85;
	g_data = (uint8_t*)bmp_ladomatas;
	gr.SetGraphicsBytes(g_data);
	manual = false;	
	cp_status = STATUS_LADOMAT_OFF;
	wb = NULL;
}

UILadomat::~UILadomat(void)
{
	//delete gr;
}

void UILadomat::Draw()
{
	if (!window->IsActive())
		return;

	if (cp_status == STATUS_UNKNOWN)
	{
		gr.SetColor(COLOR_BLACK);
		gr.Draw();
	}
	else if (cp_status == STATUS_LADOMAT_MANUAL)
	{
		gr.SetColor(CP_Manual_Color);
		gr.Draw();
	}
	else if (cp_status == STATUS_LADOMAT_ON)
	{
		gr.SetColor(CP_ON_Color);
		gr.Draw();
	}
	else if (cp_status == STATUS_LADOMAT_OFF)
	{
		gr.SetColor(CP_OFF_Color);
		gr.Draw();
	}
	SetFlag(I_AM_PAINTED);
}

void UILadomat::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	gr.SetPosition(x, y);
}

void UILadomat::AttachWoodBoiler(CWoodBoiler * woodboilder)
{
	wb = woodboilder;
	if (wb)
	{
		wb->AddStatusChangeHandler(this);
		HandleStatusChange(wb, wb->GetLadomatStatus());
	}
}

void UILadomat::doOnClick(int x, int y)
{
	if(wb)
		wb->ChangeLadomatManual();
}

void UILadomat::HandleStatusChange(void *Sender, Status status)
{
	Log.debug("UILadomat::HandleStatusChange.");
	switch (status)
	{
	case STATUS_UNKNOWN:
	case STATUS_LADOMAT_OFF:
	case STATUS_LADOMAT_ON:
	case STATUS_LADOMAT_MANUAL:
		cp_status = status;
		Invalidate();
		break;
	default:
		break;
	}
}

/*********************************************
*********************************************/
UIVentilator::UIVentilator(CWindow* window, bool reg) :UIBase(window), gr(window, false)
{
	if (reg) window->AddUIElement(this);
	//gr = new UIGraphics(window, false);
	gr.SetSize(41, 41);
	w = 41; h = 41;
	g_data = (uint8_t*)bmp_ventiliatorius;
	gr.SetGraphicsBytes(g_data);
	manual = false;
	cp_status = STATUS_EXHAUSTFAN_OFF;
	wb = NULL;
}

UIVentilator::~UIVentilator(void)
{
	//delete gr;
}

void UIVentilator::Draw()
{
	if (!window->IsActive())
		return;

	if (cp_status == STATUS_EXHAUSTFAN_MANUAL)
	{
		gr.SetColor(COLOR_BLACK);
		gr.Draw();
	}
	else if (cp_status == STATUS_EXHAUSTFAN_MANUAL)
	{
		gr.SetColor(CP_Manual_Color);
		gr.Draw();
	}
	else if (cp_status == STATUS_EXHAUSTFAN_ON)
	{
		gr.SetColor(CP_ON_Color);
		gr.Draw();
	}
	else if (cp_status == STATUS_EXHAUSTFAN_OFF)
	{
		gr.SetColor(CP_OFF_Color);
		gr.Draw();
	}
	SetFlag(I_AM_PAINTED);
}

void UIVentilator::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	gr.SetPosition(x, y);
}

void UIVentilator::AttachWoodBoiler(CWoodBoiler * woodboiler)
{
	wb = woodboiler;
	if (wb)
	{
		wb->AddStatusChangeHandler(this);
		HandleStatusChange(wb, wb->GetSmokeFanStatus());
	}
}

void UIVentilator::doOnClick(int x, int y)
{
	if(wb)
		wb->ChangeExhaustFanManual();
}

void UIVentilator::HandleStatusChange(void *Sender, Status status)
{
	Log.debug("UIVentilator::HandleStatusChange.");
	switch (status)
	{
	case STATUS_UNKNOWN:
	case STATUS_EXHAUSTFAN_OFF:
	case STATUS_EXHAUSTFAN_ON:
	case STATUS_EXHAUSTFAN_MANUAL:
		cp_status = status;
		Invalidate();
		break;
	default:
		break;
	}
}

/*********************************************
*********************************************/
UIStatusGraphics::UIStatusGraphics(CWindow* window, bool reg):UIGraphics(window, false)
{
}

UIStatusGraphics::~UIStatusGraphics(void)
{
}

void UIStatusGraphics::doOnClick(int x, int y)
{
	//if(obj) obj->HandleOnClick(this, x, y, (int)TAG_CLICK);
}

void UIStatusGraphics::HandleStatusChange(void *Sender, Status status)
{
}

/*********************************************
*********************************************/

UISwitchButton::UISwitchButton(CWindow* window, bool reg):UIImgButton(window, false)
{
	if(reg) window->AddUIElement(this);
	uiclass = UIC_SwitchButton;
	bmpOndata = NULL;
	bmpOffdata = NULL;
	transparent = true;
	state = STATUS_DISABLED_ON;
	enabled = false;
}

UISwitchButton::~UISwitchButton()
{
	if(swth != NULL)
	{
		swth->RemoveStatusChangeHandler(swth);
	}
}

void UISwitchButton::SetOnBMPBytes(const unsigned short* data)
{
	bmpOndata = data;
	if(state == STATUS_ON || state == STATUS_DISABLED_ON)
	{
		SetBMPBytes(data);
	}
}

void UISwitchButton::SetOffBMPBytes(const unsigned short* data)
{
	bmpOffdata = data;
	if(state == STATUS_OFF || state == STATUS_DISABLED_OFF)
	{
		SetBMPBytes(data);
	}
}

//void SwitchButton::SetState(bool value)
//{
//	if(state != value){
//		state = value;
//		if(state){
//			SetBMPBytes(bmpOndata);
//			DispatchStatusChange(this, STATUS_ON);
//		}else{
//			SetBMPBytes(bmpOffdata);
//			DispatchStatusChange(this, STATUS_OFF);
//		}
//	}
//}

//void SwitchButton::ChangeState()
//{
//	SetState(!state);
//}

//bool SwitchButton::GetState()
//{
//	return state;
//}

void UISwitchButton::SetSwitch(Switch* value)
{
	swth = value;
	if(swth != NULL)
	{
		swth->AddStatusChangeHandler(this);
		HandleStatusChange(swth, swth->GetStatus());
	}
}

void UISwitchButton::doOnClick(int x, int y)
{
	if(state == STATUS_ON || state == STATUS_OFF){
		swth->SwitchState();
	}
}

void UISwitchButton::HandleStatusChange(void* Sender, Status value)
{
	state = value;
	switch (state)
	{
	case STATUS_OFF:
		enabled = true;
		SetBMPBytes(bmpOffdata);
		break;
	case STATUS_ON:
		enabled = true;
		SetBMPBytes(bmpOndata);
		break;
	case STATUS_DISABLED_ON:
		enabled = false;
		SetBMPBytes(bmpOndata);
		break;
	case STATUS_DISABLED_OFF:
		enabled = false;
		SetBMPBytes(bmpOffdata);
		break;
	default:
		break;
	}
}

/*********************************************
*********************************************/
/*********************************************
*********************************************/
/*********************************************
*********************************************/
