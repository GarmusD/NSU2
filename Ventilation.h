#pragma once
#include "Events.h"
#include "VentilationDefs.h"

class CVentilation :
	public Events
{
public:
	CVentilation();
	virtual ~CVentilation();
	void Begin(void);
	void ApplyConfig(uint8_t cfgPos, const VentilationData& data);
};

extern CVentilation Ventialtion;