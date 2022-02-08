#ifndef fonts_h
#define fonts_h

#include "UTFT.h"
#include "Logger.h"
#include "consts.h"

// *** Hardwarespecific defines ***
#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask

#define regtype volatile uint32_t
#define regsize uint32_t


class Fonts
{
public:
	Fonts(UTFT& utft);
	~Fonts();

	void	Put_Text(const String st, int x, int y, const byte* font_data);
	void	Put_Text_array(const char *st, int x, int y, const byte* font_data);
	void	Set_character_spacing(unsigned char space);

	void	DrawMonoBitmap(int x, int y, const byte* bitmap);
	void	DrawColorBitmap(int x, int y, const unsigned short* bitmap);
	int		GetTextWidth(String st, const byte* font_data);
	int		GetTextArrayWidth(const char *st, const byte* font_data);
	int		GetCharWidth(const char st, const byte* font_data);
	int		GetCharSpacing(void);

	int		GetFontHeight(const byte* font_data);

	//static Fonts* getInstance(UTFT* _utft);

private:
	char buffer[MAX_LABEL_LENGTH];
	unsigned char font_height;
	unsigned int font_size;
	unsigned long font_address;
	unsigned int PositionX, PositionY;
	unsigned char Charspace;
	//static Fonts* instance;
	UTFT& tft;
	//unsigned char Charspace;
	void Draw_character(const unsigned char character, const byte* font_data);
};


#endif