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

	void	Put_Text_array(const char *st, int x, int y, const uint8_t* font_data);
	void	Set_character_spacing(unsigned char space);
	void	DrawMonoBitmap(int x, int y, const uint8_t* bitmap);
	void	DrawColorBitmap(int x, int y, const unsigned short* bitmap);
	int		GetTextArrayWidth(const char *st, const uint8_t* font_data);
	int		GetCharWidth(const char st, const uint8_t* font_data);
	int		GetCharSpacing(void);
	int		GetFontHeight(const uint8_t* font_data);
private:
	char buffer[MAX_LABEL_LENGTH];
	unsigned char font_height;
	unsigned int font_size;
	unsigned long font_address;
	unsigned int PositionX, PositionY;
	unsigned char Charspace;
	UTFT& tft;
	void Draw_character(const unsigned char character, const uint8_t* font_data);
};


#endif