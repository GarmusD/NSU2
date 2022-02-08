#include "InputWindow.h"
#include "Events.h"

InputWindow::InputWindow(Rect windowarea, UTFT& utft, Scenario& Sc, byte* sc_buf):CWindow(windowarea, utft, Sc, sc_buf)
{
	maxchars = 16;
	text = new char[maxchars+1];
	text[0] = 0;
	shiftstate = false;

	int dw = 642;
	int dh = 265;
	windowArea.Left = windowarea.Left + ((windowarea.Right - windowarea.Left - dw)/2);
	windowArea.Top = windowarea.Top + ((windowarea.Bottom - windowarea.Top - dh)/2);
	windowArea.Right = windowArea.Left + dw;
	windowArea.Bottom = windowArea.Top + dh;

	setClientArea();

	SetColor(ModalWindowColor);

	input = new UIInput(this);
	input->SetPosition(7, 6);
	input->SetSize(618, 33);

	for(int i=0; i<52; i++)
	{
		btn_all[i] = new UIButton(this);
		btn_all[i]->AddOnClickHandler(this);
		btn_all[i]->SetSize(52, 28);
	}

	btn_digits[0] = btn_all[0];
	btn_digits[1] = btn_all[1];
	btn_digits[2] = btn_all[2];
	btn_digits[3] = btn_all[3];
	btn_digits[4] = btn_all[4];
	btn_digits[5] = btn_all[5];
	btn_digits[6] = btn_all[6];
	btn_digits[7] = btn_all[7];
	btn_digits[8] = btn_all[8];
	btn_digits[9] = btn_all[9];

	btn_chars[0] = btn_all[10];
	btn_chars[1] = btn_all[11];
	btn_chars[2] = btn_all[12];
	btn_chars[3] = btn_all[13];
	btn_chars[4] = btn_all[14];
	btn_chars[5] = btn_all[15];
	btn_chars[6] = btn_all[16];
	btn_chars[7] = btn_all[17];
	btn_chars[8] = btn_all[18];
	btn_chars[9] = btn_all[19];
	btn_chars[10] = btn_all[20];
	btn_chars[11] = btn_all[21];
	btn_chars[12] = btn_all[22];
	btn_chars[13] = btn_all[23];
	btn_chars[14] = btn_all[24];
	btn_chars[15] = btn_all[25];
	btn_chars[16] = btn_all[26];
	btn_chars[17] = btn_all[27];
	btn_chars[18] = btn_all[28];
	btn_chars[19] = btn_all[29];
	btn_chars[20] = btn_all[30];
	btn_chars[21] = btn_all[31];
	btn_chars[22] = btn_all[32];
	btn_chars[23] = btn_all[33];
	btn_chars[24] = btn_all[34];
	btn_chars[25] = btn_all[35];
	btn_chars[26] = btn_all[36];
	btn_chars[27] = btn_all[37];
	btn_chars[28] = btn_all[39];
	btn_chars[29] = btn_all[40];
	btn_chars[30] = btn_all[41];
	btn_chars[31] = btn_all[42];
	btn_chars[32] = btn_all[43];
	btn_chars[33] = btn_all[44];
	btn_chars[34] = btn_all[45];

	btn_dot = btn_all[50];
	btn_underline = btn_all[48];

	btn_shift = btn_all[38];
	btn_backspace = btn_all[46];
	btn_cancel = btn_all[47];
	btn_ok = btn_all[51];
	btn_space = btn_all[49];

	setupDigits();
	setupChars();
	setupSpecials();
}


InputWindow::~InputWindow(void)
{
	delete[] text;
}

void InputWindow::setupDigits()
{
	btn_digits[0]->SetPosition(7, 49);
	btn_digits[0]->SetCaption("1");
	btn_digits[0]->SetTag(49);

	btn_digits[1]->SetPosition(70, 49);
	btn_digits[1]->SetCaption("2");
	btn_digits[1]->SetTag(50);

	btn_digits[2]->SetPosition(133, 49);
	btn_digits[2]->SetCaption("3");
	btn_digits[2]->SetTag(51);

	btn_digits[3]->SetPosition(196, 49);
	btn_digits[3]->SetCaption("4");
	btn_digits[3]->SetTag(52);

	btn_digits[4]->SetPosition(259, 49);
	btn_digits[4]->SetCaption("5");
	btn_digits[4]->SetTag(53);

	btn_digits[5]->SetPosition(321, 49);
	btn_digits[5]->SetCaption("6");
	btn_digits[5]->SetTag(54);

	btn_digits[6]->SetPosition(384, 49);
	btn_digits[6]->SetCaption("7");
	btn_digits[6]->SetTag(55);

	btn_digits[7]->SetPosition(447, 49);
	btn_digits[7]->SetCaption("8");
	btn_digits[7]->SetTag(56);

	btn_digits[8]->SetPosition(510, 49);
	btn_digits[8]->SetCaption("9");
	btn_digits[8]->SetTag(57);

	btn_digits[9]->SetPosition(573, 49);
	btn_digits[9]->SetCaption("0");
	btn_digits[9]->SetTag(48);
}

void InputWindow::setupChars()
{
	//LT chars
	//�
	char ltc[2] = {224, 0};
	btn_chars[0]->SetPosition(38, 83);
	btn_chars[0]->SetCaption(ltc);
	btn_chars[0]->SetTag(224);//�-192

	//�
	ltc[0] = 232;
	btn_chars[1]->SetPosition(101, 83);
	btn_chars[1]->SetCaption(ltc);
	btn_chars[1]->SetTag(232);//� - 200

	//�
	ltc[0] = 230;
	btn_chars[2]->SetPosition(164, 83);
	btn_chars[2]->SetCaption(ltc);
	btn_chars[2]->SetTag(230);//� - 198

	//�
	ltc[0] = 235;
	btn_chars[3]->SetPosition(227, 83);
	btn_chars[3]->SetCaption(ltc);
	btn_chars[3]->SetTag(235);//� - 203

	//�
	ltc[0] = 225;
	btn_chars[4]->SetPosition(289, 83);
	btn_chars[4]->SetCaption(ltc);
	btn_chars[4]->SetTag(225);//� - 193

	//�
	ltc[0] = 240;
	btn_chars[5]->SetPosition(352, 83);
	btn_chars[5]->SetCaption(ltc);
	btn_chars[5]->SetTag(240);//� - 208

	//�
	ltc[0] = 248;
	btn_chars[6]->SetPosition(415, 83);
	btn_chars[6]->SetCaption(ltc);
	btn_chars[6]->SetTag(248);//� - 216

	//�
	ltc[0] = 251;
	btn_chars[7]->SetPosition(478, 83);
	btn_chars[7]->SetCaption(ltc);
	btn_chars[7]->SetTag(251);//� - 219

	//�
	ltc[0] = 254;
	btn_chars[8]->SetPosition(541, 83);
	btn_chars[8]->SetCaption(ltc);
	btn_chars[8]->SetTag(254);//� - 222


	/////////////////////////////
	btn_chars[9]->SetPosition(7, 118);
	btn_chars[9]->SetCaption("q");
	btn_chars[9]->SetTag('q');

	btn_chars[10]->SetPosition(70, 118);
	btn_chars[10]->SetCaption("w");
	btn_chars[10]->SetTag('w');

	btn_chars[11]->SetPosition(133, 118);
	btn_chars[11]->SetCaption("e");
	btn_chars[11]->SetTag('e');

	btn_chars[12]->SetPosition(196, 118);
	btn_chars[12]->SetCaption("r");
	btn_chars[12]->SetTag('r');

	btn_chars[13]->SetPosition(259, 118);
	btn_chars[13]->SetCaption("t");
	btn_chars[13]->SetTag('t');

	btn_chars[14]->SetPosition(321, 118);
	btn_chars[14]->SetCaption("y");
	btn_chars[14]->SetTag('y');

	btn_chars[15]->SetPosition(384, 118);
	btn_chars[15]->SetCaption("u");
	btn_chars[15]->SetTag('u');

	btn_chars[16]->SetPosition(447, 118);
	btn_chars[16]->SetCaption("i");
	btn_chars[16]->SetTag('i');

	btn_chars[17]->SetPosition(510, 118);
	btn_chars[17]->SetCaption("o");
	btn_chars[17]->SetTag('o');

	btn_chars[18]->SetPosition(573, 118);
	btn_chars[18]->SetCaption("p");
	btn_chars[18]->SetTag('p');

	//////////////////////////////////////
	btn_chars[19]->SetPosition(38, 152);
	btn_chars[19]->SetCaption("a");
	btn_chars[19]->SetTag('a');

	btn_chars[20]->SetPosition(101, 152);
	btn_chars[20]->SetCaption("s");
	btn_chars[20]->SetTag('s');

	btn_chars[21]->SetPosition(164, 152);
	btn_chars[21]->SetCaption("d");
	btn_chars[21]->SetTag('d');

	btn_chars[22]->SetPosition(227, 152);
	btn_chars[22]->SetCaption("f");
	btn_chars[22]->SetTag('f');

	btn_chars[23]->SetPosition(289, 152);
	btn_chars[23]->SetCaption("g");
	btn_chars[23]->SetTag('g');

	btn_chars[24]->SetPosition(352, 152);
	btn_chars[24]->SetCaption("h");
	btn_chars[24]->SetTag('h');

	btn_chars[25]->SetPosition(415, 152);
	btn_chars[25]->SetCaption("j");
	btn_chars[25]->SetTag('j');

	btn_chars[26]->SetPosition(478, 152);
	btn_chars[26]->SetCaption("k");
	btn_chars[26]->SetTag('k');

	btn_chars[27]->SetPosition(541, 152);
	btn_chars[27]->SetCaption("l");
	btn_chars[27]->SetTag('l');

	/////////////////////////////////////////
	btn_chars[28]->SetPosition(101, 187);
	btn_chars[28]->SetCaption("z");
	btn_chars[28]->SetTag('z');

	btn_chars[29]->SetPosition(164, 187);
	btn_chars[29]->SetCaption("x");
	btn_chars[29]->SetTag('x');

	btn_chars[30]->SetPosition(226, 187);
	btn_chars[30]->SetCaption("c");
	btn_chars[30]->SetTag('c');

	btn_chars[31]->SetPosition(289, 187);
	btn_chars[31]->SetCaption("v");
	btn_chars[31]->SetTag('v');

	btn_chars[32]->SetPosition(352, 187);
	btn_chars[32]->SetCaption("b");
	btn_chars[32]->SetTag('b');

	btn_chars[33]->SetPosition(415, 187);
	btn_chars[33]->SetCaption("n");
	btn_chars[33]->SetTag('n');

	btn_chars[34]->SetPosition(478, 187);
	btn_chars[34]->SetCaption("m");
	btn_chars[34]->SetTag('m');

	btn_space->SetPosition(196, 221);
	btn_space->SetSize(240, 28);
	btn_space->SetTag(32);
	btn_space->SetCaption(" ");

	btn_dot->SetPosition(447, 221);
	btn_dot->SetCaption(".");
	btn_dot->SetTag('.');

	btn_underline->SetPosition(133, 221);
	btn_underline->SetCaption("_");
	btn_underline->SetTag('_');
}

void InputWindow::setupSpecials()
{
	btn_shift->SetPosition(7, 187);
	btn_shift->SetSize(52, 28);
	btn_shift->SetTag(15);
	btn_shift->SetCaption("Shift");

	btn_backspace->SetPosition(573, 187);
	btn_backspace->SetSize(52, 28);
	btn_backspace->SetTag(8);
	btn_backspace->SetCaption("back");

	btn_cancel->SetPosition(7, 221);
	btn_cancel->SetSize(84, 28);
	btn_cancel->SetTag(1);
	btn_cancel->SetCaption("Atsisakyti");

	btn_ok->SetPosition(541, 221);
	btn_ok->SetSize(84, 28);
	btn_ok->SetTag(2);
	btn_ok->SetCaption("Gerai");
}

void InputWindow::SetInputFocus()
{
	input->SetFocus();
}

void InputWindow::SetInputType(InputType type)
{
	if(itype != type)
	{
		itype = type;
	}
}

void InputWindow::SetMaxChars(int max)
{
	maxchars = max;
	if(maxchars > 16) maxchars = 16;
}

void InputWindow::SetText(char* value)
{
	strncpy(text, value, 16);
	text[16] = 0;
	input->SetCaption(text);
}

char* InputWindow::GetText()
{
	return text;
}

void InputWindow::ChangeShiftState()
{
	char inc;
	shiftstate = !shiftstate;
	if(shiftstate) inc = -32;
	else inc = 32;

	for(uint i=0; i < sizeof(btn_chars)/sizeof(UIButton*); i++)
	{
		char t = btn_chars[i]->GetTag();
		t += inc;
		btn_chars[i]->SetTag(t);
		btn_chars[i]->SetCaption(String(t));
	}

	if(shiftstate)
	{
		btn_dot->SetCaption(",");
		btn_dot->SetTag(',');

		btn_underline->SetCaption("-");
		btn_underline->SetTag('-');
	}
	else
	{
		btn_dot->SetCaption(".");
		btn_dot->SetTag('.');

		btn_underline->SetCaption("_");
		btn_underline->SetTag('_');
	}
}

void InputWindow::AddChar(char c)
{
	byte l = strlen(text);
	if( l >= maxchars) return;
	text[l] = c;
	text[l+1] = 0;
	input->SetCaption(text);
}

void InputWindow::DelChar()
{
	byte l = strlen(text);
	if(l == 0) return;
	text[l-1] = 0;
	input->SetCaption(text);
}

bool InputWindow::DotPresent()
{
	return false;
}

void InputWindow::HandleOnClick(void* Sender, int x, int y, int tag)
{
	switch (tag)
	{
	case 1://cancel
		mresult = MR_CANCEL;
		break;
	case 2://ok
		mresult = MR_OK;
		break;
	case 8://del
		DelChar();
		break;
	case 15://shift
		ChangeShiftState();
		break;
	default:
		break;
	}
	if(tag >= 32 && tag <= 254) //' '..Z
	{
		AddChar(tag);
		if(shiftstate) ChangeShiftState();
	}
}
