#include "Events.h"
//#include <Arduino.h>

Events::Events(void)
{
}

Events::~Events(void)
{
	TempHandlers.Clear();
	OnClickHandlers.Clear();
	StatusHandlers.Clear();
}

void Events::AddTempChangeHandler(Events* handler)
{
	TempHandlers.Add(handler);
}

void Events::AddOnClickHandler(Events* handler)
{
	OnClickHandlers.Add(handler);
}

void Events::AddStatusChangeHandler(Events* handler)
{
	StatusHandlers.Add(handler);
}

void Events::RemoveTempChangeHandler(Events* handler)
{
	TempHandlers.Delete(handler);
}

void Events::RemoveOnClickHandler(Events* handler)
{
	OnClickHandlers.Delete(handler);
}

void Events::RemoveStatusChangeHandler(Events* handler)
{
	StatusHandlers.Delete(handler);
}

void Events::DispatchTemperature(void* Sender, float t)
{
	for(int i=0; i < TempHandlers.Count(); i++)
	{
		Events* evt = TempHandlers.Get(i);
		if(evt) evt->HandleTemperatureChange(Sender, t);
	}
}

void Events::DispatchOnClick(void* Sender, int x, int y, int tag)
{
	for(int i=0; i < OnClickHandlers.Count(); i++)
	{
		Events* evt = OnClickHandlers.Get(i);
		if(evt) evt->HandleOnClick(Sender, x, y, tag);
	}
}

void Events::DispatchStatusChange(void* Sender, Status status)
{
	for(int i=0; i < StatusHandlers.Count(); i++)
	{
		Events* evt = StatusHandlers.Get(i);
		if(evt) evt->HandleStatusChange(Sender, status);
	}
}

void Events::HandleTemperatureChange(void* Sender, float t)
{
}

void Events::HandleOnClick(void* Sender, int x, int y, int tag)
{
}

void Events::HandleStatusChange(void* uielement, Status status)
{
}

void Events::HandleTimerEvent(int te_id)
{
}

void Events::OnTimeSlice()
{
}

void Events::Reset()
{
	TempHandlers.Clear();
	OnClickHandlers.Clear();
	StatusHandlers.Clear();
}

const char* Events::GetStatusName(Status value)
{
	switch (value)
	{
	case STATUS_UNKNOWN:
		return cUNKNOWN;
		break;
	case STATUS_OFF:
		return cOFF;
		break;
	case STATUS_ON:
		return cON;
		break;
	case STATUS_MANUAL:
		return cMANUAL;
		break;
	case STATUS_DISABLED:
		return cDISABLED;
		break;
	case STATUS_DISABLED_OFF:
		return cDISABLED_OFF;
		break;
	case STATUS_DISABLED_ON:
		return cDISABLED_ON;
		break;
	case STATUS_KATILAS_UNKNOWN:
		return cUNKNOWN;
		break;
	case STATUS_KATILAS_UZGESES:
		return cKATILAS_UZGESES;
		break;
	case STATUS_KATILAS_IKURIAMAS:
		return cKATILAS_IKURIAMAS;
		break;
	case STATUS_KATILAS_KURENASI:
		return cKATILAS_KURENASI;
		break;
	case STATUS_KATILAS_GESTA:
		return cKATILAS_GESTA;
		break;
	case STATUS_LADOMAT_OFF:
		return cLADOMAT_OFF;
		break;
	case STATUS_LADOMAT_ON:
		return cLADOMAT_ON;
		break;
	case STATUS_LADOMAT_MANUAL:
		return cLADOMAT_MANUAL;
		break;
	case STATUS_EXHAUSTFAN_OFF:
		return cEXHAUSTFAN_OFF;
		break;
	case STATUS_EXHAUSTFAN_ON:
		return cEXHAUSTFAN_ON;
		break;
	case STATUS_EXHAUSTFAN_MANUAL:
		return cEXHAUSTFAN_MANUAL;
		break;
	default:
		return cSHIT;
		break;
	}
}
