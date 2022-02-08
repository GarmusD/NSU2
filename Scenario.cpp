#include "Scenario.h"

Scenario::Scenario(UTFT& utft):tft(utft)
{
}


Scenario::~Scenario(void)
{
}

short Scenario::getBufShort(byte* buf, int& idx)
{
	byte cl, ch;
	short s;
	ch = buf[idx];
	idx++;
	cl = buf[idx];
	idx++;
	s = (short)ch<<8 | cl;
	return s;
}

void Scenario::PlayScenario(int xbase, int ybase, byte* buf)
{

	if(!buf || !sizeof(buf)) return;

	int idx = 0;
	byte cmd;
	
	short s1, s2, s3, s4;
	byte b1, b2, b3;
	while((cmd = buf[idx])>0)
	{
		idx++;//increase to next byte after cmd
		switch (cmd)
		{
		case GC_SET_COLOR:
			b1 = buf[idx]; idx++;
			b2 = buf[idx]; idx++;
			b3 = buf[idx]; idx++;
			tft.setColor(b1, b2, b3);
			break;
		case GC_SET_BG_COLOR:
			b1 = buf[idx]; idx++;
			b2 = buf[idx]; idx++;
			b3 = buf[idx]; idx++;
			tft.setBackColor(b1, b2, b3);
			break;
		case GC_DRAW_PIXEL:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			tft.drawPixel(s1, s2);
			break;
		case GC_DRAW_LINE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			tft.drawLine(s1, s2, s3, s4);
			break;
		case GC_DRAW_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			tft.drawRect(s1, s2, s3, s4);
			break;
		case GC_DRAW_ROUND_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			tft.drawRoundRect(s1, s2, s3, s4);
			break;
		case GC_FILL_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			tft.fillRect(s1, s2, s3, s4);
			break;
		case GC_FILL_ROUND_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			tft.fillRoundRect(s1, s2, s3, s4);
			break;
		case GC_DRAW_CIRCLE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = getBufShort(buf, idx);
			tft.drawCircle(s1, s2, s3);
			break;
		case GC_FILL_CIRCLE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = getBufShort(buf, idx);
			tft.fillCircle(s1, s2, s3);
			break;
		case GC_DRAW_ARC:
			break;
		case GC_FILL_ARC:
			break;
		case GC_LOAD_IMAGE://from internal sd card
			break;
		case GC_DRAW_BITMAP://from byte array
			break;
		case GC_DRAW_GRAPHICS://from commands array
			break;
		default:
			break;
		}
	}
}

void Scenario::PlayScenario(int xbase, int ybase, Rect region, byte* buf)
{
	if(!buf || !sizeof(buf)) return;

	int idx = 0;
	byte cmd;
	
	short s1, s2, s3, s4;
	byte b1, b2, b3;
	while((cmd = buf[idx])>0)
	{
		idx++;//increase to next byte after cmd
		switch (cmd)
		{
		case GC_SET_COLOR:
			b1 = buf[idx]; idx++;
			b2 = buf[idx]; idx++;
			b3 = buf[idx]; idx++;
			tft.setColor(b1, b2, b3);
			break;
		case GC_SET_BG_COLOR:
			b1 = buf[idx]; idx++;
			b2 = buf[idx]; idx++;
			b3 = buf[idx]; idx++;
			tft.setBackColor(b1, b2, b3);
			break;
		case GC_DRAW_PIXEL:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region)) tft.drawPixel(s1, s2);
			break;
		case GC_DRAW_LINE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region) || InRect(s3, s4, region)) tft.drawLine(s1, s2, s3, s4);
			break;
		case GC_DRAW_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region) || InRect(s3, s4, region)) tft.drawRect(s1, s2, s3, s4);
			break;
		case GC_DRAW_ROUND_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region) || InRect(s3, s4, region)) tft.drawRoundRect(s1, s2, s3, s4);
			break;
		case GC_FILL_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region) || InRect(s3, s4, region)) tft.fillRect(s1, s2, s3, s4);
			break;
		case GC_FILL_ROUND_RECT:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = xbase + getBufShort(buf, idx);
			s4 = ybase + getBufShort(buf, idx);
			if(InRect(s1, s2, region) || InRect(s3, s4, region)) tft.fillRoundRect(s1, s2, s3, s4);
			break;
		case GC_DRAW_CIRCLE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = getBufShort(buf, idx);
			if(InRect(s1-s3, s2-s3, region) || InRect(s1 + s3, s1+s3, region)) tft.drawCircle(s1, s2, s3);
			break;
		case GC_FILL_CIRCLE:
			s1 = xbase + getBufShort(buf, idx);
			s2 = ybase + getBufShort(buf, idx);
			s3 = getBufShort(buf, idx);
			if(InRect(s1-s3, s2-s3, region) || InRect(s1 + s3, s1+s3, region)) tft.fillCircle(s1, s2, s3);
			break;
		case GC_DRAW_ARC:
			break;
		case GC_FILL_ARC:
			break;
		case GC_LOAD_IMAGE://from internal sd card
			break;
		case GC_DRAW_BITMAP://from byte array
			break;
		case GC_DRAW_GRAPHICS://from commands array
			break;
		default:
			break;
		}
	}
}

bool Scenario::InRect(int x, int y, Rect r)
{
	return x >= r.Left && x <= r.Right && y >= r.Top && y <= r.Bottom;
}

