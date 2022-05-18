#ifndef Utilities_h
#define Utilities_h

#include "Logger.h"
#include "consts.h"

class Utilities
{
public:
	static void clearString(char* str, uint8_t l = MAX_NAME_LENGTH);
	static void ClearAndCopyString(const char* source, char* dest);
	static float pround(float value);
	static float roundFloat(float value, int places);
	static bool nearEqual(float f1, float f2);
	static bool CheckMillis(uint32_t & mill, uint32_t count, bool reset_millis = true);

	static float round2(float f)
	{
		return floor(f * 10 + 0.5) / 10;
	}
};

template <size_t CAPACITY>
class AverageF
{
public:
	AverageF() { started = false; }
	~AverageF(void) {};

	float Add(float value, bool z5round = true)
	{
		float avg = 0.0;

		if (!started)
		{//starting
			init(value);
			avg = value;
		}
		else 
		{
			for (uint8_t i = 1; i < CAPACITY; i++) 
			{
				arr[i - 1] = arr[i];
				avg += arr[i];
			}
			arr[CAPACITY - 1] = value;
			avg += value;
		}

		avg = avg / (float)CAPACITY;

		if (z5round) return Utilities::pround(avg);
		else return avg;
	}

	int GetSize()
	{
		return CAPACITY;
	}

	void Reset()
	{
		started = false;
	}
private:
	
	void init(float value)
	{
		for (int i = 0; i < CAPACITY; i++)
		{
			arr[i] = value;
		}
		started = true;
	}
	float arr[CAPACITY];
	bool started;
};

template <size_t CAPACITY>
class AverageI
{
public:
	AverageI() { started = false; }
	~AverageI(void) {};

	int Add(int value)
	{
		return Add((float) value);
	}

	int Add(float value)
	{
		float avg = 0.0;

		if (!started)
		{//starting
			init(value);
			avg = value;
		}
		else
		{
			for (uint8_t i = 1; i < CAPACITY; i++)
			{
				arr[i - 1] = arr[i];
				avg += arr[i];
			}
			arr[CAPACITY - 1] = value;
			avg += value;
		}

		avg = avg / (float)CAPACITY;

		return (int)round(avg);
	}

	int GetSize()
	{
		return CAPACITY;
	}

	void Reset()
	{
		started = false;
	}
private:
	
	void init(float value)
	{
		for (int i = 0; i < CAPACITY; i++)
		{
			arr[i] = value;
		}
		started = true;
	}
	float arr[CAPACITY];
	bool started;
};


#endif