#pragma once

#include "List.h"
#include "Events.h"

class Action : public Events
{
	Action(void);
	~Action(void);
};

class Actions
{
public:
	Actions(void);
	~Actions(void);
public:
	List<Action> actions;
};

