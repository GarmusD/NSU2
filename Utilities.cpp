#include "utilities.h"

void Utilities::clearString(char * str, uint8_t l)
{
	for (uint8_t i = 0; i < l; i++) str[i] = 0;
}

void Utilities::ClearAndCopyString(const char * source, char * dest)
{
	clearString(dest);
	strncpy(dest, source, MAX_NAME_LENGTH - 1);
}

bool Utilities::CheckMillis(uint32_t & mill, uint32_t count, bool reset_millis /*= true*/)
{
	uint32_t ml = millis();

	if(ml - mill >= count){
		if(reset_millis) mill = ml;
		return true;
	}
	return false;
}

float Utilities::pround(float value)
{
	if (value == 0.)
		return value;
	bool sign = value < 0.;
	if(sign) value = abs(value);
	double dintpart;
	float fractpart, intpart;
	fractpart = modf (value, &dintpart);
	intpart = dintpart;
	if(fractpart < 0.25)
	{
		fractpart = 0.00001;
	}else if(fractpart < 0.75)
	{
		fractpart = 0.50001;
	}else
	{
		fractpart = 1.00001;
	}
	intpart += fractpart;

	if(sign) intpart = 0 - intpart;

	return intpart;
}

// printFloat prints out the float 'value' rounded to 'places' places after the decimal point
float Utilities::roundFloat(float value, int places) 
{
	float prec;
	switch (places)
	{
	case 1:
		prec = 10.0f;
		break;
	case 2:
		prec = 100.0f;
		break;
	case 3:
		prec = 1000.0f;
		break;
	default:
		prec = 10.0f;
		break;
	}

	bool sign = value < 0.;
	if(sign) value = abs(value);
	int x = round( value * prec );
	float res = x / prec;

	if(sign) res = -(res);

	return res;
}

bool Utilities::nearEqual(float f1, float f2)
{
	if (abs(f1 - f2) < 0.001)
		return true;
	return false;
}
/************************************************************************************/
