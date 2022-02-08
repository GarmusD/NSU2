#include "TempTrigger.h"

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

TempTriggerPiece::TempTriggerPiece(void)
{
	req_temp = -127.0;
	sens_temp = -127.0;
	histeresis = 1.0;
	state = false;
	condition = TrueIfHigher;
	tsensor = NULL;
}

TempTriggerPiece::~TempTriggerPiece(void)
{
	if(tsensor) tsensor->RemoveTempChangeHandler(this);
}

void TempTriggerPiece::SetTriggerCondition(TriggerCondition cond)
{
	condition = cond;
}

void TempTriggerPiece::SetTempSensor(TempSensor* sensor)
{	
	if(tsensor != NULL) tsensor->RemoveTempChangeHandler(this);
	tsensor = NULL;
	if(sensor)
	{
		tsensor = sensor;
		sens_temp = tsensor->getTemp();
		tsensor->AddTempChangeHandler(this);
		HandleTemperatureChange(sensor, sens_temp);
	}
	else
	{
		Log.error("Adding NULL TempSensor to TempTriggerPiece");
	}
}

void TempTriggerPiece::SetTemperature(float value)
{
	req_temp = value;
}

void TempTriggerPiece::SetHisteresis(float value)
{
	histeresis = abs(value);
	if(histeresis == 0.0) histeresis = 1.0;
}

bool TempTriggerPiece::GetStatus()
{
	return state;
}

void TempTriggerPiece::HandleTemperatureChange(void* Sender, float value)
{
	bool old_state = state;
	sens_temp = value;
	switch (condition)
	{
	case TrueIfLower:
		if(state)
		{
			state = sens_temp <= req_temp+histeresis;
		}
		else
		{
			state = sens_temp <= req_temp;
		}
		break;
	case TrueIfHigher:
		if(state)
		{
			state = sens_temp >= req_temp-histeresis;
		}
		else
		{
			state = sens_temp >= req_temp;
		}
		break;
	default:
		break;
	}
	if(old_state != state) {
		if(state){
			Log.debug("TempTriggerPiece::HandleStatusChange. Status: STATUS_ON req_temp: "+String(req_temp) + ". Sensor temp: " + String(sens_temp) + ". Condition: " + String(condition));
			DispatchStatusChange(this, STATUS_ON);
		}else{
			Log.debug("TempTriggerPiece::HandleStatusChange. Status: STATUS_OFF req_temp: "+String(req_temp) + ". Sensor temp: " + String(sens_temp) + ". Condition: " + String(condition));
			DispatchStatusChange(this, STATUS_OFF);
		}
	}
}

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

TempTrigger::TempTrigger(void)
{
	status = STATUS_OFF;
	name[0] = 0;
	pieces.Clear();
}


TempTrigger::~TempTrigger(void)
{

}

bool TempTrigger::Check()
{
	for(byte i=0; i < pieces.Count(); i++)
	{
		if(!pieces.Get(i)->GetStatus())
		{
			Log.debug("Piece "+String(i)+" returned FALSE!");
			return false;
		}
	}
	return true;
}

void TempTrigger::AddTriggerPiece(TriggerCondition cond, const char* tsensname, float temperature, float hister)
{
	if (pieces.Count() < MAX_TEMP_TRIGGER_PIECES)
	{
		Log.debug("AddTriggerPiece - Condition: " + String((byte)cond) + ", TSensorName: '" + String(tsensname) + "', Temp: " + String(temperature) + ", Hist: " + String(hister) + ".");
		TempTriggerPiece* piece = pieces.Add();
		if (piece)
		{
			TempSensor* ts = TSensors.getByName(tsensname);
			piece->SetTriggerCondition(cond);
			piece->SetTemperature(temperature);
			piece->SetHisteresis(hister);
			piece->SetTempSensor(ts);
			piece->AddStatusChangeHandler(this);
		}
	}
	else
	{
		Log.error("TempTrigger::AddTriggerPiece() - MAX_TEMP_TRIGGER_PIECES reached.");
	}
}

void TempTrigger::PiecesDone()
{
	HandleStatusChange(NULL, STATUS_UNKNOWN);
}

Status TempTrigger::GetStatus(){
	return status;
}

String TempTrigger::GetInfoString()
{
	return "TRIGGER: "+String(name)+" "+GetStatusName(status);
}

void TempTrigger::SetName(const char* value)
{
	strlcpy(name, value, MAX_NAME_LENGTH);
}

const char* TempTrigger::GetName()
{
	return name;
}

void TempTrigger::HandleStatusChange(void* Sender, Status value)
{
	if(Check())
	{
		ChangeStatus(STATUS_ON);
	}else{
		ChangeStatus(STATUS_OFF);
	}	
}

void TempTrigger::ChangeStatus(Status value)
{
	if (status != value)
	{
		status = value;
		PrintInfo();
		DispatchStatusChange(this, status);
	}
}

void TempTrigger::PrintInfo()
{
	StaticJsonBuffer<512> jBuff;
	JsonObject& root = jBuff.createObject();
	root[jKeyTarget] = jTargetTempTrigger;
	root[jKeyAction] = jValueInfo;
	root[jKeyName] = name;
	root[jKeyStatus] = Events::GetStatusName(GetStatus());
	PrintJson.Print(root);
}
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

TempTrigger* CTempTriggers::GetByName(const char* value)
{
	TempTrigger* trigger;
	for(int i=0; i<Count(); i++)
	{
		trigger = Get(i);
		if(strcmp(value, trigger->GetName()) == 0) return trigger;
	}
	return NULL;

}


CTempTriggers TempTriggers;