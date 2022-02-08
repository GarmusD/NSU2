#ifndef win_defs_h
#define win_defs_h

#include "consts.h"

typedef char UIID[MAX_NAME_LENGTH];

struct Rect
{
	int Left;
	int Top;
	int Right;
	int Bottom;
};

enum ModalResult
{
	MR_NO_RESULT,
	MR_CANCEL,
	MR_OK
};

#endif