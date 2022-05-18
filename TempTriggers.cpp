#include "TempTriggers.h"
#include "SystemStatus.h"

const int CTempTriggers::CURRENT_CONFIG_VERSION = 2;
const VersionInfo CTempTriggers::VINFO = { 'T', 'R', 'G', CURRENT_CONFIG_VERSION };
const char* CTempTriggers::CFG_FILE = "triggers.cfg";

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
	//if(tsensor) tsensor->RemoveTempChangeHandler(this);
	Events::Reset();
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
	if(old_state != state) 
	{
		char s[128];
		if(state)
		{
			snprintf(s, 128, "TempTriggerPiece::HandleStatusChange. Status: STATUS_ON req_temp: %.1f. Sensor temp: %.1f. Condition: %d", req_temp, sens_temp, (int)condition);
			Log.debug(s);
			DispatchStatusChange(this, STATUS_ON);
		}
		else
		{
			snprintf(s, 128, "TempTriggerPiece::HandleStatusChange. Status: STATUS_OFF req_temp: %.1f. Sensor temp: %.1f. Condition: %d", req_temp, sens_temp, (int)condition);
			Log.debug(s);
			DispatchStatusChange(this, STATUS_OFF);
		}
	}
}

void TempTriggerPiece::Reset()
{
	Events::Reset();
	req_temp = -127.0;
	sens_temp = -127.0;
	histeresis = 1.0;
	state = false;
	condition = TrueIfHigher;
	tsensor = NULL;
}

/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/
/*************************************************************************************/

TempTrigger::TempTrigger(void)
{
	configPos = 0xFF;
	status = STATUS_OFF;
	name[0] = 0;
	pieces.Clear();
}


TempTrigger::~TempTrigger(void)
{

}

uint8_t TempTrigger::GetConfigPos()
{
	return configPos;
}

void TempTrigger::ApplyConfig(uint8_t cfgPos, const TempTriggerData & data)
{
	configPos = cfgPos;
	SetName(data.Name);
	for (int i = 0; i < MAX_TEMP_TRIGGER_PIECES; i++)
	{
		if (data.ttpiece[i].Valid)
		{
			AddTriggerPiece((TriggerCondition)data.ttpiece[i].Condition, data.ttpiece[i].SensorName, data.ttpiece[i].Temperature / 100.0, data.ttpiece[i].Histeresis / 100.0);
		}
	}
	PiecesDone();
}

bool TempTrigger::Check()
{
	for(uint8_t i=0; i < pieces.Count(); i++)
	{
		if(!pieces.Get(i)->GetStatus())
		{
			char s[64];
			sprintf(s, "Piece %d returned FALSE!", i);
			Log.debug(s);
			return false;
		}
	}
	return true;
}

void TempTrigger::AddTriggerPiece(TriggerCondition cond, const char* tsensname, float temperature, float hister)
{
	if (pieces.Count() < MAX_TEMP_TRIGGER_PIECES)
	{
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

void TempTrigger::Reset()
{
	/*
	for (uint8_t i = 0; i < pieces.Count(); i++)
	{
		TempTriggerPiece* ttp = pieces.Get(i);
		if(ttp)
		{
			ttp->RemoveStatusChangeHandler(this);
		}
	}
	*/
	status = STATUS_OFF;
	name[0] = 0;
	pieces.Clear();
	Events::Reset();
}

void TempTrigger::PrintInfo()
{
	DynamicJsonDocument jBuff(512);
	JsonObject root = jBuff.to<JsonObject>();
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

TempTrigger * CTempTriggers::GetByConfigPos(uint8_t cfgPos)
{
	TempTrigger* trigger;
	for (int i = 0; i < Count(); i++)
	{
		trigger = Get(i);
		if (trigger->GetConfigPos() == cfgPos) return trigger;
	}
	return nullptr;
}

void CTempTriggers::Reset()
{
	Clear();
}

void CTempTriggers::LoadConfig()
{
	switch (FileManager.OpenConfigFile(CFG_FILE, VINFO))
	{
	case FileStatus_OK:
		for (size_t i = 0; i < MAX_TEMP_TRIGGERS; i++)
		{
			TempTriggerData data;
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					TempTrigger* trg = Add();
					if (trg)
					{
						trg->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				//Read error
				char s[64];
				sprintf(s, "TTriggers config file read error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_ReCreate:
		//FilePos at end of header
		Log.error("Writing defaults...");
		for (size_t i = 0; i < MAX_TEMP_TRIGGERS; i++)
		{
			const TempTriggerData& data = Defaults.GetDefaultTempTriggerData(i);
			if (FileManager.FileWriteBuff(&data, sizeof(data)))
			{
				if (data.Valid)
				{
					TempTrigger* trg = Add();
					if (trg)
					{
						trg->ApplyConfig(i, data);
					}
				}
			}
			else
			{
				char s[64];
				snprintf(s, 64, "TTriggers config file write defaults error. Pos: %d", i);
				Log.error(s);
			}
		}
		FileManager.FileClose();
		break;
	case FileStatus_RWError:
	case FileStatus_Inaccessible:
		Log.error("SD inaccessible or Read/Write error. Reading defaults...");
		for (size_t i = 0; i < MAX_TEMP_TRIGGERS; i++)
		{
			const TempTriggerData& data = Defaults.GetDefaultTempTriggerData(i);
			if (data.Valid)
			{
				TempTrigger* trg = Add();
				if (trg)
				{
					trg->ApplyConfig(i, data);
				}
			}
		}
		break;
	default:
		break;
	}
}

bool CTempTriggers::GetConfigData(uint8_t cfgPos, TempTriggerData & data)
{
	if (FileManager.OpenConfigFile(CFG_FILE, VINFO) == FileStatus_OK)
	{
		uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
		if (FileManager.FileSeek(fpos))
		{
			if (FileManager.FileReadBuffer(&data, sizeof(data)))
			{
				FileManager.FileClose();
				return true;
			}
		}
		FileManager.FileClose();
	}
	return false;
}

bool CTempTriggers::ValidateSetupDataSet(JsonObject & jo)
{
	if (jo.containsKey(jKeyConfigPos) &&
		jo.containsKey(jKeyEnabled) &&
		jo.containsKey(jKeyName) &&
		jo.containsKey(jTriggerPieces)
		)
	{
		return true;
	}
	else
	{
		PrintJson.PrintResultError(jTargetTempTrigger, jErrorInvalidDataSet);
		return false;
	}
}

void CTempTriggers::ParseJson(JsonObject & jo)
{
	if (jo.containsKey(jKeyAction))
	{
		const char * action = jo[jKeyAction];
		if (strcmp(action, jKeySetup) == 0)
		{
			if (ValidateSetupDataSet(jo))
			{
				TempTriggerData data;
				uint8_t cfgPos = jo[jKeyConfigPos];
				data.Valid = jo[jKeyEnabled];
				Utilities::ClearAndCopyString(jo[jKeyName], data.Name);
				JsonArray ja = jo[jTriggerPieces];
				for (uint8_t i = 0; i < MAX_TEMP_TRIGGER_PIECES; i++)
				{
					JsonObject joo = ja[i];
					data.ttpiece[i].Valid = joo[jKeyEnabled];
					Utilities::ClearAndCopyString(joo[jTriggerSensorName], data.ttpiece[i].SensorName);
					data.ttpiece[i].Condition = joo[jTriggerCondition];
					data.ttpiece[i].Temperature = (int)((float)joo[jTriggerTemperature] * 100.0f);
					data.ttpiece[i].Histeresis = (int)((float)joo[jTriggerHisteresis] * 100.0f);
				}
				uint32_t fpos = sizeof(VersionInfo) + cfgPos * sizeof(data);
				if (FileManager.OpenConfigFile(CFG_FILE, VINFO))
				{
					if (FileManager.FileSeek(fpos))
					{
						if (FileManager.FileWriteBuff(&data, sizeof(data)))
						{
							FileManager.FileClose();
							SystemStatus.SetRebootRequired();
							PrintJson.PrintResultOk(jTargetTempTrigger, action, true);
							return;
						}
					}
					PrintJson.PrintResultError(jTargetTempTrigger, jErrorFileWriteError);
					return;
				}
				PrintJson.PrintResultError(jTargetTempTrigger, jErrorFileOpenError);
				return;
			}
		}
		else if (strcmp(action, "on") == 0)
		{
			const char * name = jo[jKeyName];
			TempTrigger * trg = GetByName(name);
			if (trg)
			{
			}
		}
		else
		{
			PrintJson.PrintResultUnknownAction(jTargetTempTrigger, action);
		}
	}
}

CTempTriggers TempTriggers;