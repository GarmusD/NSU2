#ifndef msg_h
#define msg_h

enum MsgType{
	NoTouch,
	TouchDown,
	TouchMove,
	TouchUp
};

struct Msg
{
	MsgType type;
	int x;
	int y;
};

#endif

