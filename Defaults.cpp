#include "Defaults.h"

void SetAddr(uint8_t* a, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7)
{
	a[0] = b0;
	a[1] = b1;
	a[2] = b2;
	a[3] = b3;
	a[4] = b4;
	a[5] = b5;
	a[6] = b6;
	a[7] = b7;
}

const SwitchData& DefaultsClass::GetDefaultSwitchData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_WINTER_MODE], MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (uint8_t)STATUS_OFF;
		switchData.forcestate = (uint8_t)STATUS_OFF;
		switchData.status = (uint8_t)STATUS_ON;
		switchData.Valid = 1;
		break;
	case 1:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE], MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (uint8_t)STATUS_OFF;
		switchData.forcestate = (uint8_t)STATUS_OFF;
		switchData.status = (uint8_t)STATUS_ON;
		switchData.Valid = 1;
		break;
	case 2:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE], MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		strlcpy(switchData.DependOn, SWITCH_NAMES[SWITCH_NAME_WINTER_MODE], MAX_NAME_LENGTH);
		switchData.depstate = (uint8_t)STATUS_OFF;
		switchData.forcestate = (uint8_t)STATUS_ON;
		switchData.status = (uint8_t)STATUS_OFF;
		switchData.Valid = 1;
		break;
	case 3:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_BURN_MODE], MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (uint8_t)STATUS_OFF;
		switchData.forcestate = (uint8_t)STATUS_OFF;
		switchData.status = (uint8_t)STATUS_ON;
		switchData.Valid = 1;
		break;
	default:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (uint8_t)STATUS_OFF;
		switchData.forcestate = (uint8_t)STATUS_OFF;
		switchData.status = (uint8_t)STATUS_OFF;
		switchData.Valid = 0;
		break;
	}

	return switchData;

}

const TSensorData& DefaultsClass::GetDefaultTSensorData(uint8_t pos)
{
	tsensorData.Valid = 1;
	switch (pos)
	{
	case 0:
		SetAddr(tsensorData.sid.addr, 0x28, 0x80, 0x1F, 0x94, 0x04, 0x00, 0x00, 0x96);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "tualetas", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	//case 1:
	//	SetAddr(tsensorData.sid.addr, 0x28, 0xE0, 0x14, 0x94, 0x4, 0x00, 0x00, 0x25);
	//	for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
	//	strlcpy(tsensorData.sid.name, "svetaine_maza", MAX_NAME_LENGTH);
	//	tsensorData.interval = 240;
	//	break;
	case 1:
		SetAddr(tsensorData.sid.addr, 0x28, 0x54, 0x5, 0x94, 0x4, 0x0, 0x0, 0x4f);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_vidurys", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 2:
		SetAddr(tsensorData.sid.addr, 0x28, 0x2a, 0xa, 0x94, 0x4, 0x0, 0x0, 0xab);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "boileris_virsus", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 3:
		SetAddr(tsensorData.sid.addr, 0x28, 0x3e, 0x93, 0xa0, 0x4, 0x0, 0x0, 0x2e);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_apacia", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 4:
		SetAddr(tsensorData.sid.addr, 0x28, 0xfe, 0xe9, 0x93, 0x4, 0x0, 0x0, 0xa1);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "svetaine", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 5:
		SetAddr(tsensorData.sid.addr, 0x28, 0xa1, 0x5, 0x94, 0x4, 0x0, 0x0, 0xd6);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "virtuve", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 6:
		SetAddr(tsensorData.sid.addr, 0x28, 0xf1, 0xf0, 0x93, 0x4, 0x0, 0x0, 0xa);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "katilas", MAX_NAME_LENGTH);
		tsensorData.interval = 15;
		break;
	case 7:
		SetAddr(tsensorData.sid.addr, 0x28, 0x25, 0x42, 0x94, 0x4, 0x0, 0x0, 0x58);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "darbo_kambarys", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 8:
		SetAddr(tsensorData.sid.addr, 0x28, 0x5d, 0x47, 0xa0, 0x4, 0x0, 0x0, 0xf5);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "arduino", MAX_NAME_LENGTH);
		tsensorData.interval = 60;
		break;
	case 9:
		SetAddr(tsensorData.sid.addr, 0x28, 0x3, 0xe3, 0x93, 0x4, 0x0, 0x0, 0x24);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "koridorius", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 10:
		SetAddr(tsensorData.sid.addr, 0x28, 0x43, 0xb9, 0x93, 0x4, 0x0, 0x0, 0x79);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "sveciu_kamb", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 11:
		SetAddr(tsensorData.sid.addr, 0x28, 0x87, 0xb1, 0x93, 0x4, 0x0, 0x0, 0x4);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "boileris_apacia", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 12:
		SetAddr(tsensorData.sid.addr, 0x28, 0x9f, 0xd7, 0x9f, 0x4, 0x0, 0x0, 0x61);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "svabrine", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 13:
		SetAddr(tsensorData.sid.addr, 0x28, 0xff, 0x78, 0x93, 0x4, 0x0, 0x0, 0xec);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_virsus", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 14:
		SetAddr(tsensorData.sid.addr, 0x28, 0x41, 0x93, 0x93, 0x4, 0x0, 0x0, 0x52);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "laukas", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 15:
		SetAddr(tsensorData.sid.addr, 0x28, 0x11, 0x45, 0xA0, 0x4, 0x0, 0x0, 0x7e);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "pozemis", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 16:
		SetAddr(tsensorData.sid.addr, 0x28, 0xfe, 0x05, 0xa0, 0x4, 0x0, 0x0, 0x9a);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "rusys1", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 17:
		SetAddr(tsensorData.sid.addr, 0x28, 0x48, 0xa0, 0xa0, 0x4, 0x0, 0x0, 0xce);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "rusys2", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 18:
		SetAddr(tsensorData.sid.addr, 0x28, 0xB2, 0x8A, 0x9F, 0x4, 0x0, 0x0, 0xE4);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "grindu_paduodamas", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	default:
		SetAddr(tsensorData.sid.addr, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		tsensorData.interval = 0;
		tsensorData.Valid = 0;
		break;
	}

	return tsensorData;
}

const RelayModuleData& DefaultsClass::GetDefaultRelayModuleData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		relayData.Valid = 1;
		relayData.Inverted = 0;
		relayData.ActiveLow = 1;
		break;
	case 1:
		relayData.Valid = 1;
		relayData.Inverted = 0;
		relayData.ActiveLow = 1;
		break;
	case 2:
		relayData.Valid = 1;
		relayData.Inverted = 1;
		relayData.ActiveLow = 1;
		break;
	case 3:
		relayData.Valid = 0;
		relayData.Inverted = 0;
		relayData.ActiveLow = 0;
		break;
	case 4:
		relayData.Valid = 0;
		relayData.Inverted = 0;
		relayData.ActiveLow = 0;
		break;
	default:
		relayData.Valid = 0;
		relayData.Inverted = 0;
		relayData.ActiveLow = 0;
		break;
	}

	return relayData;
}

const TempTriggerData& DefaultsClass::GetDefaultTempTriggerData(uint8_t pos)
{
		switch (pos)
		{
		case 0:
			triggerData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.Name[i] = 0;
			strlcpy(triggerData.Name, "ladomat", MAX_NAME_LENGTH);

			triggerData.ttpiece[0].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[0].SensorName, "akum_apacia", MAX_NAME_LENGTH);
			triggerData.ttpiece[0].Condition = (TriggerCondition)0;
			triggerData.ttpiece[0].Temperature = 5500;
			triggerData.ttpiece[0].Histeresis = 100;

			triggerData.ttpiece[1].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[1].SensorName[i] = 0;
			triggerData.ttpiece[1].Condition = (TriggerCondition)0;
			triggerData.ttpiece[1].Temperature = 0;
			triggerData.ttpiece[1].Histeresis = 0;

			triggerData.ttpiece[2].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[2].SensorName[i] = 0;
			triggerData.ttpiece[2].Condition = (TriggerCondition)0;
			triggerData.ttpiece[2].Temperature = 0;
			triggerData.ttpiece[2].Histeresis = 0;

			triggerData.ttpiece[3].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[3].SensorName[i] = 0;
			triggerData.ttpiece[3].Condition = (TriggerCondition)0;
			triggerData.ttpiece[3].Temperature = 0;
			triggerData.ttpiece[3].Histeresis = 0;
			break;
		case 1:
			triggerData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.Name[i] = 0;
			strlcpy(triggerData.Name, "boilerio", MAX_NAME_LENGTH);

			triggerData.ttpiece[0].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[0].SensorName, "boileris_virsus", MAX_NAME_LENGTH);
			triggerData.ttpiece[0].Condition = (TriggerCondition)0;
			triggerData.ttpiece[0].Temperature = 5500;
			triggerData.ttpiece[0].Histeresis = 500;

			triggerData.ttpiece[1].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[1].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[1].SensorName, "katilas", MAX_NAME_LENGTH);
			triggerData.ttpiece[1].Condition = (TriggerCondition)1;
			triggerData.ttpiece[1].Temperature = 7200;
			triggerData.ttpiece[1].Histeresis = 200;

			triggerData.ttpiece[2].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[2].SensorName[i] = 0;
			triggerData.ttpiece[2].Condition = (TriggerCondition)0;
			triggerData.ttpiece[2].Temperature = 0;
			triggerData.ttpiece[2].Histeresis = 0;

			triggerData.ttpiece[3].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[3].SensorName[i] = 0;
			triggerData.ttpiece[3].Condition = (TriggerCondition)0;
			triggerData.ttpiece[3].Temperature = 0;
			triggerData.ttpiece[3].Histeresis = 0;
			break;
		case 2:
			triggerData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.Name, "grindu", MAX_NAME_LENGTH);

			triggerData.ttpiece[0].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[0].SensorName, "akum_virsus", MAX_NAME_LENGTH);
			triggerData.ttpiece[0].Condition = (TriggerCondition)1;
			triggerData.ttpiece[0].Temperature = 3400;
			triggerData.ttpiece[0].Histeresis = 2500;

			triggerData.ttpiece[1].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[1].SensorName[i] = 0;
			triggerData.ttpiece[1].Condition = (TriggerCondition)0;
			triggerData.ttpiece[1].Temperature = 0;
			triggerData.ttpiece[1].Histeresis = 0;

			triggerData.ttpiece[2].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[2].SensorName[i] = 0;
			triggerData.ttpiece[2].Condition = (TriggerCondition)0;
			triggerData.ttpiece[2].Temperature = 0;
			triggerData.ttpiece[2].Histeresis = 0;

			triggerData.ttpiece[3].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[3].SensorName[i] = 0;
			triggerData.ttpiece[3].Condition = (TriggerCondition)0;
			triggerData.ttpiece[3].Temperature = 0;
			triggerData.ttpiece[3].Histeresis = 0;
			break;
		case 3:
			triggerData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.Name[i] = 0;
			strlcpy(triggerData.Name, "akumuliac70", MAX_NAME_LENGTH);

			triggerData.ttpiece[0].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[0].SensorName, "akum_virsus", MAX_NAME_LENGTH);
			triggerData.ttpiece[0].Condition = (TriggerCondition)1;
			triggerData.ttpiece[0].Temperature = 7000;
			triggerData.ttpiece[0].Histeresis = 200;

			triggerData.ttpiece[1].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[1].SensorName[i] = 0;
			triggerData.ttpiece[1].Condition = (TriggerCondition)0;
			triggerData.ttpiece[1].Temperature = 7200;
			triggerData.ttpiece[1].Histeresis = 200;

			triggerData.ttpiece[2].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[2].SensorName[i] = 0;
			triggerData.ttpiece[2].Condition = (TriggerCondition)0;
			triggerData.ttpiece[2].Temperature = 0;
			triggerData.ttpiece[2].Histeresis = 0;

			triggerData.ttpiece[3].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[3].SensorName[i] = 0;
			triggerData.ttpiece[3].Condition = (TriggerCondition)0;
			triggerData.ttpiece[3].Temperature = 0;
			triggerData.ttpiece[3].Histeresis = 0;
			break;
		default:
			triggerData.Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.Name[i] = 0;

			triggerData.ttpiece[0].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			triggerData.ttpiece[0].Condition = (TriggerCondition)0;
			triggerData.ttpiece[0].Temperature = 0;
			triggerData.ttpiece[0].Histeresis = 0;

			triggerData.ttpiece[1].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[1].SensorName[i] = 0;
			triggerData.ttpiece[1].Condition = (TriggerCondition)0;
			triggerData.ttpiece[1].Temperature = 0;
			triggerData.ttpiece[1].Histeresis = 0;

			triggerData.ttpiece[2].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[2].SensorName[i] = 0;
			triggerData.ttpiece[2].Condition = (TriggerCondition)0;
			triggerData.ttpiece[2].Temperature = 0;
			triggerData.ttpiece[2].Histeresis = 0;

			triggerData.ttpiece[3].Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[3].SensorName[i] = 0;
			triggerData.ttpiece[3].Condition = (TriggerCondition)0;
			triggerData.ttpiece[3].Temperature = 0;
			triggerData.ttpiece[3].Histeresis = 0;
			break;
		}

		return triggerData;
}

const CircPumpData& DefaultsClass::GetDefaultCircPumpData(uint8_t pos)
{
		switch (pos)
		{
		case 0:
			cpumpData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.Name[i] = 0;
			strlcpy(cpumpData.Name, "grindu", MAX_NAME_LENGTH);
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.TempTriggerName[i] = 0;
			strlcpy(cpumpData.TempTriggerName, "grindu", MAX_NAME_LENGTH);
			//cpumpData.Channel = 19;
			cpumpData.MaxSpeed = 1;
			cpumpData.Spd1Channel = 19;
			cpumpData.Spd2Channel = 0xFF;
			cpumpData.Spd3Channel = 0xFF;
			break;
		case 1:
			cpumpData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.Name[i] = 0;
			strlcpy(cpumpData.Name, "radiatoriu", MAX_NAME_LENGTH);
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.TempTriggerName[i] = 0;
			strlcpy(cpumpData.TempTriggerName, "akumuliac70", MAX_NAME_LENGTH);
			//cpumpData.Channel = 20;
			cpumpData.MaxSpeed = 1;
			cpumpData.Spd1Channel = 20;
			cpumpData.Spd2Channel = 0xFF;
			cpumpData.Spd3Channel = 0xFF;
			break;
		case 2:
			cpumpData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.Name[i] = 0;
			strlcpy(cpumpData.Name, "boilerio", MAX_NAME_LENGTH);
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.TempTriggerName[i] = 0;
			//strcpy(data.TempTriggerName, "");
			//cpumpData.Channel = 21;
			cpumpData.MaxSpeed = 1;
			cpumpData.Spd1Channel = 21;
			cpumpData.Spd2Channel = 0xFF;
			cpumpData.Spd3Channel = 0xFF;
			break;
		default:
			cpumpData.Valid = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.Name[i] = 0;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.TempTriggerName[i] = 0;
			//cpumpData.Channel = 0xFF;
			cpumpData.MaxSpeed = 1;
			cpumpData.Spd1Channel = 0xFF;
			cpumpData.Spd2Channel = 0xFF;
			cpumpData.Spd3Channel = 0xFF;
			break;
		}
		
		return cpumpData;
}

const CollectorData& DefaultsClass::GetDefaultCollectorData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		collectorData.Valid = 1;
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		strlcpy(collectorData.Name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;
		strlcpy(collectorData.CircPump, "grindu", MAX_NAME_LENGTH);

		collectorData.valve_count = 8;

		collectorData.valves[0].type = (ValveType)0;
		collectorData.valves[0].relay_channel = 1;

		collectorData.valves[1].type = (ValveType)0;
		collectorData.valves[1].relay_channel = 2;

		collectorData.valves[2].type = (ValveType)0;
		collectorData.valves[2].relay_channel = 3;

		collectorData.valves[3].type = (ValveType)0;
		collectorData.valves[3].relay_channel = 4;

		collectorData.valves[4].type = (ValveType)0;
		collectorData.valves[4].relay_channel = 5;

		collectorData.valves[5].type = (ValveType)0;
		collectorData.valves[5].relay_channel = 6;

		collectorData.valves[6].type = (ValveType)0;
		collectorData.valves[6].relay_channel = 7;

		collectorData.valves[7].type = (ValveType)0;
		collectorData.valves[7].relay_channel = 8;

		collectorData.valves[8].type = (ValveType)0;
		collectorData.valves[8].relay_channel = 0xFF;

		collectorData.valves[9].type = (ValveType)0;
		collectorData.valves[9].relay_channel = 0xFF;

		collectorData.valves[10].type = (ValveType)0;
		collectorData.valves[10].relay_channel = 0xFF;

		collectorData.valves[11].type = (ValveType)0;
		collectorData.valves[11].relay_channel = 0xFF;

		collectorData.valves[12].type = (ValveType)0;
		collectorData.valves[12].relay_channel = 0xFF;

		collectorData.valves[13].type = (ValveType)0;
		collectorData.valves[13].relay_channel = 0xFF;

		collectorData.valves[14].type = (ValveType)0;
		collectorData.valves[14].relay_channel = 0xFF;

		collectorData.valves[15].type = (ValveType)0;
		collectorData.valves[15].relay_channel = 0xFF;
		break;
	case 1:
		collectorData.Valid = 1;
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		strlcpy(collectorData.Name, "2aukstas", MAX_NAME_LENGTH);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;
		strlcpy(collectorData.CircPump, "grindu", MAX_NAME_LENGTH);

		collectorData.valve_count = 5;

		collectorData.valves[0].type = (ValveType)0;
		collectorData.valves[0].relay_channel = 9;

		collectorData.valves[1].type = (ValveType)0;
		collectorData.valves[1].relay_channel = 10;

		collectorData.valves[2].type = (ValveType)0;
		collectorData.valves[2].relay_channel = 11;

		collectorData.valves[3].type = (ValveType)0;
		collectorData.valves[3].relay_channel = 12;

		collectorData.valves[4].type = (ValveType)0;
		collectorData.valves[4].relay_channel = 13;

		collectorData.valves[5].type = (ValveType)0;
		collectorData.valves[5].relay_channel = 0xFF;

		collectorData.valves[6].type = (ValveType)0;
		collectorData.valves[6].relay_channel = 0xFF;

		collectorData.valves[7].type = (ValveType)0;
		collectorData.valves[7].relay_channel = 0xFF;

		collectorData.valves[8].type = (ValveType)0;
		collectorData.valves[8].relay_channel = 0xFF;

		collectorData.valves[9].type = (ValveType)0;
		collectorData.valves[9].relay_channel = 0xFF;

		collectorData.valves[10].type = (ValveType)0;
		collectorData.valves[10].relay_channel = 0xFF;

		collectorData.valves[11].type = (ValveType)0;
		collectorData.valves[11].relay_channel = 0xFF;

		collectorData.valves[12].type = (ValveType)0;
		collectorData.valves[12].relay_channel = 0xFF;

		collectorData.valves[13].type = (ValveType)0;
		collectorData.valves[13].relay_channel = 0xFF;

		collectorData.valves[14].type = (ValveType)0;
		collectorData.valves[14].relay_channel = 0xFF;

		collectorData.valves[15].type = (ValveType)0;
		collectorData.valves[15].relay_channel = 0xFF;
		break;
	case 2:
		collectorData.Valid = 1;
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		strlcpy(collectorData.Name, "rusys", MAX_NAME_LENGTH);
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;
		strlcpy(collectorData.CircPump, "radiatoriu", MAX_NAME_LENGTH);

		collectorData.valve_count = 2;

		collectorData.valves[0].type = (ValveType)0;
		collectorData.valves[0].relay_channel = 14;

		collectorData.valves[1].type = (ValveType)0;
		collectorData.valves[1].relay_channel = 15;

		collectorData.valves[2].type = (ValveType)0;
		collectorData.valves[2].relay_channel = 0xFF;

		collectorData.valves[3].type = (ValveType)0;
		collectorData.valves[3].relay_channel = 0xFF;

		collectorData.valves[4].type = (ValveType)0;
		collectorData.valves[4].relay_channel = 0xFF;

		collectorData.valves[5].type = (ValveType)0;
		collectorData.valves[5].relay_channel = 0xFF;

		collectorData.valves[6].type = (ValveType)0;
		collectorData.valves[6].relay_channel = 0xFF;

		collectorData.valves[7].type = (ValveType)0;
		collectorData.valves[7].relay_channel = 0xFF;

		collectorData.valves[8].type = (ValveType)0;
		collectorData.valves[8].relay_channel = 0xFF;

		collectorData.valves[9].type = (ValveType)0;
		collectorData.valves[9].relay_channel = 0xFF;

		collectorData.valves[10].type = (ValveType)0;
		collectorData.valves[10].relay_channel = 0xFF;

		collectorData.valves[11].type = (ValveType)0;
		collectorData.valves[11].relay_channel = 0xFF;

		collectorData.valves[12].type = (ValveType)0;
		collectorData.valves[12].relay_channel = 0xFF;

		collectorData.valves[13].type = (ValveType)0;
		collectorData.valves[13].relay_channel = 0xFF;

		collectorData.valves[14].type = (ValveType)0;
		collectorData.valves[14].relay_channel = 0xFF;

		collectorData.valves[15].type = (ValveType)0;
		collectorData.valves[15].relay_channel = 0xFF;
		break;
	default:
		collectorData.Valid = 0;
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		for (uint8_t i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;

		collectorData.valve_count = 0;

		collectorData.valves[0].type = (ValveType)0;
		collectorData.valves[0].relay_channel = 0xff;

		collectorData.valves[1].type = (ValveType)0;
		collectorData.valves[1].relay_channel = 0xff;

		collectorData.valves[2].type = (ValveType)0;
		collectorData.valves[2].relay_channel = 0xff;

		collectorData.valves[3].type = (ValveType)0;
		collectorData.valves[3].relay_channel = 0xff;

		collectorData.valves[4].type = (ValveType)0;
		collectorData.valves[4].relay_channel = 0xff;

		collectorData.valves[5].type = (ValveType)0;
		collectorData.valves[5].relay_channel = 0xff;

		collectorData.valves[6].type = (ValveType)0;
		collectorData.valves[6].relay_channel = 0xff;

		collectorData.valves[7].type = (ValveType)0;
		collectorData.valves[7].relay_channel = 0xff;

		collectorData.valves[8].type = (ValveType)0;
		collectorData.valves[8].relay_channel = 0xff;

		collectorData.valves[9].type = (ValveType)0;
		collectorData.valves[9].relay_channel = 0xff;

		collectorData.valves[10].type = (ValveType)0;
		collectorData.valves[10].relay_channel = 0xFF;

		collectorData.valves[11].type = (ValveType)0;
		collectorData.valves[11].relay_channel = 0xFF;

		collectorData.valves[12].type = (ValveType)0;
		collectorData.valves[12].relay_channel = 0xFF;

		collectorData.valves[13].type = (ValveType)0;
		collectorData.valves[13].relay_channel = 0xFF;

		collectorData.valves[14].type = (ValveType)0;
		collectorData.valves[14].relay_channel = 0xFF;

		collectorData.valves[15].type = (ValveType)0;
		collectorData.valves[15].relay_channel = 0xFF;
		break;
	}
	
	return collectorData;
}

const ComfortZoneData& DefaultsClass::GetDefaultComfortZoneData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		//Name: svetaine_maza Channel: 1 Col name: 1aukstas Room sensor: svetaine_maza Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "svetaine_maza", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Svetaine maza", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svetaine", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 1;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 1:
		//Name: svetaine_didele Channel: 3 Col name: 1aukstas Room sensor: svetaine_didele Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "svetaine_didele", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Svetaine didele", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svetaine", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 3;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 2:
		//Name: darbo_kambarys Channel: 2 Col name: 1aukstas Room sensor: darbo_kambarys Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "darbo_kambarys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Darbo kambarys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "darbo_kambarys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 2;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 3:
		//Name: sveciu_kambarys Channel: 5 Col name: 1aukstas Room sensor: sveciu_kamb Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "sveciu_kambarys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Sveciu kambarys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "sveciu_kamb", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 4;
		czonesData.room_temp_hi = 2000;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 4:
		//Name: skalbykla Channel: 8 Col name: 1aukstas Room sensor: svabrine Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "skalbykla", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Skalbykla", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svabrine", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 6;
		czonesData.room_temp_hi = 2000;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 5:
		//Name: koridorius Channel: 9 Col name: 1aukstas Room sensor: koridorius Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "koridorius", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Koridorius", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "koridorius", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 7;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 6:
		//Name: tualetas Channel: 10 Col name: 1aukstas Room sensor: tualetas Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "tualetas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Tualetas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "tualetas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 8;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 7:
		//Name: virtuve Channel: 6 Col name: 1aukstas Room sensor: virtuve Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "virtuve", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Virtuve", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "virtuve", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 5;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 8:
		//Name: rusys1 Channel: 255 Col name: rusys Room sensor: rusys1 Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "rusys1", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Rusys1", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "rusys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "rusys1", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 1;
		czonesData.room_temp_hi = 500;
		czonesData.room_temp_low = 500; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 500; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 500; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 9:
		//Name: rusys2 Channel: 255 Col name: rusys Room sensor: rusys2 Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "rusys2", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Rusys2", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "rusys", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "rusys2", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 2;
		czonesData.room_temp_hi = 500;
		czonesData.room_temp_low = 500; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 500; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 500; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	
	default:
		czonesData.Valid = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.actuator = 0xff;
		czonesData.room_temp_hi = 0;
		czonesData.room_temp_low = 0; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 0; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 0; //   <<<<<------- pakeitimas
		czonesData.histeresis = 0;
		czonesData.low_temp_mode = 0;
		break;
	}

	return czonesData;
}

const KTypeData& DefaultsClass::GetDefaultKTypeData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		ktypeData.Valid = 1;
		ktypeData.interval = 5;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) ktypeData.Name[j] = 0;
		strlcpy(ktypeData.Name, "dumu_temp", MAX_NAME_LENGTH);
		break;
	default:
		ktypeData.Valid = 0;
		ktypeData.interval = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) ktypeData.Name[j] = 0;
		break;
	}
	return ktypeData;
}

const WaterBoilerData& DefaultsClass::GetDefaultWaterBoilerData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		waterBoilerData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.Name[j] = 0;
		strlcpy(waterBoilerData.Name, "default", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TSensorName[j] = 0;
		strlcpy(waterBoilerData.TSensorName, "boileris_virsus", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TempTriggerName[j] = 0;
		strlcpy(waterBoilerData.TempTriggerName, "boilerio", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.CircPumpName[j] = 0;
		strlcpy(waterBoilerData.CircPumpName, "boilerio", MAX_NAME_LENGTH);
		waterBoilerData.ElHeatingEnabled = 0;
		waterBoilerData.ElHeatingChannel = 0xFF;

		for (uint8_t i = 0; i<7; i++)
		{
			waterBoilerData.HeatingData[i].StartHour = 0;
			waterBoilerData.HeatingData[i].StartMin = 0;
			waterBoilerData.HeatingData[i].EndHour = 0;
			waterBoilerData.HeatingData[i].EndMin = 0;
		}
		break;
	default:
		waterBoilerData.Valid = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TSensorName[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TempTriggerName[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.CircPumpName[j] = 0;
		waterBoilerData.ElHeatingEnabled = 0;
		waterBoilerData.ElHeatingChannel = 0xFF;

		for (uint8_t i = 0; i<7; i++)
		{
			waterBoilerData.HeatingData[i].StartHour = 0;
			waterBoilerData.HeatingData[i].StartMin = 0;
			waterBoilerData.HeatingData[i].EndHour = 0;
			waterBoilerData.HeatingData[i].EndMin = 0;
		}
		break;
	}
	return waterBoilerData;
}

const WoodBoilerData& DefaultsClass::GetDefaultWoodBoilerData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		woodBoilerData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.Name[j] = 0;
		strlcpy(woodBoilerData.Name, "default", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.TSensorName[j] = 0;
		strlcpy(woodBoilerData.TSensorName, "katilas", MAX_NAME_LENGTH);
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.KTypeName[j] = 0;
		strlcpy(woodBoilerData.KTypeName, "dumu_temp", MAX_NAME_LENGTH);
		woodBoilerData.LadomatChannel = 17;
		woodBoilerData.ExhaustFanChannel = 18;
		woodBoilerData.WorkingTemp = 8500;
		woodBoilerData.WorkingHisteresis = 200;
		woodBoilerData.LadomatTemp = 6200;
		strlcpy(woodBoilerData.LadomatTempTriggerName, "ladomat", MAX_NAME_LENGTH);
		break;
	default:
		woodBoilerData.Valid = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.TSensorName[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.KTypeName[j] = 0;
		woodBoilerData.LadomatChannel = 0xff;
		woodBoilerData.ExhaustFanChannel = 0xff;
		woodBoilerData.WorkingTemp = 7500;
		woodBoilerData.WorkingHisteresis = 200;
		break;
	}

	return woodBoilerData;
}

const SystemFanData & DefaultsClass::GetDefaultSystemFanData(uint8_t pos)
{
	switch (pos)
	{
	case 0:
		sysFanData.Valid = 1;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) sysFanData.TSensorName[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) sysFanData.Name[j] = 0;
		strlcpy(sysFanData.Name, "systemfan", MAX_NAME_LENGTH);
		strlcpy(sysFanData.TSensorName, "arduino", MAX_NAME_LENGTH);
		sysFanData.MinTemp = 2500;
		sysFanData.MaxTemp = 3500;		
		break;
	default:
		sysFanData.Valid = 0;
		sysFanData.MinTemp = 0;
		sysFanData.MaxTemp = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) sysFanData.TSensorName[j] = 0;
		for (uint8_t j = 0; j < MAX_NAME_LENGTH; j++) sysFanData.Name[j] = 0;
		break;
	}
	return sysFanData;
}

const AlarmData & DefaultsClass::GetDefaultAlarmData(uint8_t pos)
{
	switch (pos)
	{
	default:
		alarmData.Valid = 1;
		alarmData.alarm_temp = 9500;
		alarmData.histeresis = 500;

		//ExhaustFan
		alarmData.channelInfo[0].channel = 18;
		alarmData.channelInfo[0].opened = 0;
		//Ladomat
		alarmData.channelInfo[1].channel = 17;
		alarmData.channelInfo[1].opened = 1;
		//Radiatoriu CircPump
		alarmData.channelInfo[2].channel = 20;
		alarmData.channelInfo[2].opened = 1;
		//Rusio radiatorius1
		alarmData.channelInfo[3].channel = 14;
		alarmData.channelInfo[3].opened = 1;
		//Rusio radiatorius2
		alarmData.channelInfo[4].channel = 15;
		alarmData.channelInfo[4].opened = 1;

		alarmData.channelInfo[5].channel = 0xFF;
		alarmData.channelInfo[5].opened = 0;

		alarmData.channelInfo[6].channel = 0xFF;
		alarmData.channelInfo[6].opened = 0;

		alarmData.channelInfo[7].channel = 0xFF;
		alarmData.channelInfo[7].opened = 0;
		return alarmData;
	}
}

DefaultsClass Defaults;