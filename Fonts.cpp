#include "fonts.h"
#include "UTFT.h"

/*Fonts* Fonts::instance = NULL;
Fonts* Fonts::getInstance(UTFT* _utft)
{
	if(instance == NULL){
		instance = new Fonts(_utft);
	}
	return instance;
}
*/
Fonts::Fonts(UTFT& utft):tft(utft)
{
	Log.debug("Fonts::Fonts(UTFT* tft)");
	Charspace = 1;
}

Fonts::~Fonts()
{
}


int Fonts::GetFontHeight(const byte* font_data)
{
	return font_data[0];
}

void Fonts::Set_character_spacing(unsigned char space)
{
	Charspace=space;
}

void Fonts::Put_Text_array(const char *st, int x, int y, const byte* font_data)
{
	PositionX=x;
	PositionY=y;

	cbi(tft.P_CS, tft.B_CS);								
							
				
	uint length,i;
	length = strlen(st);

	font_address=0;
										
 	font_height=font_data[font_address];
													 
	if (font_height>70)
		font_size = font_data[font_address + 150];							 
	else 
	 	font_size = font_data[font_address + 35];								 
								 						
	font_size = font_size*18;



	for (i=0; i<length; i++)
		Draw_character(st[i], font_data);


	sbi(tft.P_CS, tft.B_CS);								 
}

void Fonts::Put_Text(const String st, int x, int y, const byte* font_data)
{
	st.toCharArray(buffer, MAX_LABEL_LENGTH);
	Put_Text_array(buffer, x, y, font_data);
}

void Fonts::Draw_character(const unsigned char character, const byte* font_data)
{
	char ch = tft.fch, cl = tft.fcl;						
	char ch2 = tft.bch, cl2 = tft.bcl;					
	unsigned long location;
	unsigned char font_header[2];

	if ((character>=0x20)&&(character<=0xFF))
	{
		location = (unsigned long)(character-0x20)*(unsigned long)font_size;
		font_header[0] = font_data[location++];
		font_header[1] = font_data[location++];//width
		
		
		tft.LCD_Write_COM_DATA(0x000D, 1);
		tft.LCD_Write_COM(0x0F);

		tft.setXY(PositionX, PositionY, PositionX+Charspace+font_header[1], PositionY+font_height-1);

		//cbi(tft.P_RS, tft.B_RS);

		sbi(tft.P_RS, tft.B_RS);

		unsigned int i,j;
		unsigned int temp = font_header[1]*font_height;
		unsigned char bitsleft = temp%8;
		temp=temp/8;
		//location=location+2;									 

		for(i=0;i<temp;i++) 
		{
			volatile unsigned char m=font_data[location++];// location++;
			for(j=0;j<8;j++) 
			{
				if((m&0x01)==0x01) tft.LCD_Writ_Bus(ch,cl,16); 
				else tft.LCD_Writ_Bus(ch2,cl2,16);
				m>>=1; 
			}
		}
				
		volatile unsigned char m=font_data[location++]; //location++;
		for(j=0;j<bitsleft;j++) 
		{
			if((m&0x01)==0x01) 	tft.LCD_Writ_Bus(ch,cl,16);
			else tft.LCD_Writ_Bus(ch2,cl2,16);
			m>>=1;
		}	
		for(i=0;i<Charspace;i++) 
		{
			for(unsigned char a=0;a<font_height;a++)
				tft.LCD_Writ_Bus(ch2,cl2,16);
		}
		
		cbi(tft.P_RS, tft.B_RS);	
		tft.LCD_Write_COM_DATA(0x000D, 0);
		tft.LCD_Write_COM(0x0F);
		PositionX =PositionX+ font_header[1]+Charspace;							
	}
	else return;
}

void Fonts::DrawMonoBitmap(int x, int y, const byte* bitmap)
{
	char ch, cl;
	ch=tft.fch;//((fcolorr&248)|fcolorg>>5);
	cl=tft.fcl;//((fcolorg&28)<<3|fcolorb>>3);
	char ch2, cl2;
	ch2=tft.bch;//((bcolorr&248)|bcolorg>>5);
	cl2=tft.bcl;//((bcolorg&28)<<3|bcolorb>>3);

	unsigned int w=0, h=0;
	w = (bitmap[0] << 8) | bitmap[1];
	h = (bitmap[2] << 8) | bitmap[3];

	int idx = 4;		
			   
	if((h>801)||(w>801)) return;						
	/////////

 	cbi(tft.P_CS, tft.B_CS);							 

	tft.setXY(x, y, x+w, y+h-1);
	tft.LCD_Write_COM_DATA(0x000D, 1);
	tft.LCD_Write_COM(0x0F);

	sbi(tft.P_RS, tft.B_RS);

	unsigned int i,j;
	unsigned int temp = w*h;
	unsigned char bitsleft = temp % 8;
	temp=temp / 8+2;

	/*
	byte b = 10;
	volatile unsigned char m;
	for(int xx=0; xx < w; xx++){
		for(int yy=0; yy < h; yy++){
			if(b>=8){
				m=bitmap[idx]; idx++;
				b = 0;
			}
			if((m & 0x01) == 0x01){
				tft.drawPixel(x+xx, y+yy); 
			}
			m>>=1;
			b++;
		}
	}
	*/
	
	for(i=2;i<temp;i++) 
	{
		volatile unsigned char m=bitmap[idx]; idx++;
		for(j=0;j<8;j++) 
		{
			if((m&0x01)==0x01) 	tft.LCD_Writ_Bus(ch,cl,16); 
			else tft.LCD_Writ_Bus(ch2,cl2, 16);
			m>>=1; 
		}
	}

	volatile unsigned char m=bitmap[idx]; idx++;
	for(j=0;j<bitsleft;j++) 
	{
		if((m&0x01)==0x01) 	tft.LCD_Writ_Bus(ch,cl, 16);
		else tft.LCD_Writ_Bus(ch2,cl2, 16);
		m>>=1;
	}		
	
	cbi(tft.P_RS, tft.B_RS);
	tft.LCD_Write_COM_DATA(0x000D, 0);
	tft.LCD_Write_COM(0x0F);
	sbi(tft.P_CS, tft.B_CS);
}

void Fonts::DrawColorBitmap(int x, int y, const unsigned short* bitmap)
{
	//char ch, cl;
	//ch=tft.fch;//((fcolorr&248)|fcolorg>>5);
	//cl=tft.fcl;//((fcolorg&28)<<3|fcolorb>>3);
	//char ch2, cl2;
	//ch2=tft.bch;//((bcolorr&248)|bcolorg>>5);
	//cl2=tft.bcl;//((bcolorg&28)<<3|bcolorb>>3);

	unsigned int w=0, h=0;
	w = bitmap[0];
	h = bitmap[1];

	int idx = 2;		
			   
	if((h>801)||(w>801)) return;						
	/////////
	
 	cbi(tft.P_CS, tft.B_CS);							 

	tft.setXY(x, y, x+w-1, y+h-1);	

	for(uint i=0; i < h*w; i++){
		tft.setPixel(bitmap[idx++]);
	}		
	
	sbi(tft.P_CS, tft.B_CS);
}

int Fonts::GetTextWidth(const String st, const byte* font_data)
{
	char buf[st.length()+1];
	st.toCharArray(buf, st.length()+1);
	return GetTextArrayWidth(buf, font_data);
}

int Fonts::GetTextArrayWidth(const char *st, const byte* font_data)
{
	int length,i, w = 0;
	length = strlen(st);
	font_address=0;

	font_height=font_data[0];
	//Serial.println("FontHeight: "+String(font_height));

	if (font_height>70){
		font_size = font_data[150] * 18;
	}
	else {
		font_size = font_data[35]*18;
	}

	for (i=0; i<length; i++)
	{
		unsigned long location;
		unsigned char font_header[2];
		if ((st[i] >= 0x20) && (st[i] <= 0xFF))
		{
			location = (unsigned long)(st[i]-0x20)*(unsigned long)font_size;

			font_header[0] = font_data[location]; location++;
			font_header[1] = font_data[location]; location++;

			//Serial.println("DBG: Char '"+String(st[i])+"' width: "+String(font_header[1]));

			w += font_header[1] + Charspace;
		}
	}
	if(w>Charspace) w -= Charspace;
	return w;
}

int Fonts::GetCharWidth(const char st, const byte* font_data)
{
	int w = 0;
	font_address=0;

	font_height=font_data[font_address];

	if (font_height>70)
		font_size=font_data[150];
	else 
		font_size=font_data[35];

	font_size=font_size*18;

	unsigned long location;
	unsigned char font_header[2];
	if ((st >= 0x20) && (st <= 0xFF))
	{
		location = (unsigned long)(st-0x20)*(unsigned long)font_size + (unsigned long)font_address;

		font_header[0] = font_data[location]; location++;
		font_header[1] = font_data[location]; location++;

		w = font_header[1];
	}

	return w;
}

int Fonts::GetCharSpacing(void)
{
	return Charspace;
}