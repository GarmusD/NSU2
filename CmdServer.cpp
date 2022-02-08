#include "CmdServer.h"
#include "Settings.h"
#include "FreeMem.h"
#include "advancedFunctions.h"
//#include "windows.h"
#include <ArduinoJson.h>
#include "aJson.h"


#include "WoodBoiler.h"



//Messenger* messenger = NULL;
//CmdMessenger messenger;// = NULL;

String printAddr(uint8_t addr[]){
	return String(addr[0], HEX) + ":" + String(addr[1], HEX) + ":" + String(addr[2], HEX) + ":" + String(addr[3], HEX) + ":" +
				String(addr[4], HEX) + ":" + String(addr[5], HEX) + ":" + String(addr[6], HEX) + ":" + String(addr[7], HEX);
}

void CheckName(char* buff){
	if(buff[0] == 'N' && buff[1] == 0){
		buff[0] = 0;
	}
}

char cmdID[16];
char action[16];

//String PrintMsgDone()
//{
//	Serial.println(String(msgID) + " done");
//}

void PrintResponse(String resp)
{
	Log.log("CMD: "+String(cmdID) + " " + resp);
	cmdID[0] = 0;
}

//void PrintJsonResponse(JsonObject& jo)
//{
//	Log.log("JSON: ", false);
//	jo.printTo(Serial);
//	Log.newLine();
//	cmdID[0] = 0;
//}

void OnMessage()
{
	//if (messenger == NULL) return;
	//if(cmdServer == NULL) return;

	cmdServer.ParseMessage();
}

void OnMessageOneId()
{

}

void OnMessageTwoId()
{

}

CmdServer::CmdServer(Stream & comms, const char fld_separator/* = ','*/,
	const char cmd_separator/* = ';'*/,
	const char esc_character/* = '/'*/) :messenger(comms, fld_separator, cmd_separator, esc_character)
{
	messenger.printLfCr(true);
	cmdID[0] = 0;
	action[0] = 0;
}

CmdMessenger& CmdServer::getCmdMessenger()
{
	return messenger;
}

void CmdServer::begin()
{
	TimeSlice.RegisterTimeSlice(this);
	while (Serial.available())
	{
		Serial.read();
	}

	messenger.attach(OnMessage);

	//messenger = &msger;

	//Lets start communications! {obsolete way}
	//OBSOLETE
	Log.log("NSU_CMD_Server_Started");
}

CmdServer::~CmdServer(void)
{
}

void CmdServer::OnTimeSlice()//:TimeEvent(uint32_t t){
{
	//Log.debug("CmdServer::OnTimeSlice()");
	messenger.feedinSerialData();
	/*
	if(Serial.available()){
	msger.process(Serial.read());
	}
	*/
}

void CmdServer::ParseMessage()
{
	//if (messenger == NULL) return;
	String x = String(messenger.readStringArg());
	Log.debug("Msg is: ["+x+"]");
	//if msg is JSON format
	if(x.startsWith("{"))
	{
		ParseJsonMessage(x);
		return;
	}
	Log.debug("Msg is NOT JSON");
	PrintJson.PrintResultNotAJson();

	//no, not a JSON, do in old way...
	x.toCharArray(cmdID, 15);
	//messenger.copyStringArg(msgID, 31);

	String cmd = String(messenger.readStringArg());

	if(cmd.equals("tsensor")){
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("config")){
			byte pos = messenger.readInt16Arg();
			TSensorsData data = Settings.getTempSensorData(pos);
			String name = String(data.sid.name);
			if(name == ""){
				name = "N";
			}
			String response = String(data.Valid) + " " + printAddr(data.sid.addr) + " " + name + " " + String(data.interval);
			PrintResponse(response);
		}else if(sub_cmd.equals("system")){
			byte pos = messenger.readInt16Arg();
			TempSensor* sensor = TSensors.getByIndex(pos);
			if(sensor == NULL){
				PrintResponse("null");
			}else{
				String response = printAddr(sensor->getAddr()) + " " + String((int)(sensor->getTemp() * 100));
				PrintResponse(response);
			}
		}else if(sub_cmd.equals("clear")){
			byte pos = messenger.readInt16Arg();
			Settings.clearTempSensorData(pos);
			PrintResponse("ok");
		}else if(sub_cmd.equals("set")){
			TSensorsData data;
			byte pos = messenger.readInt16Arg();
			if(pos < MAX_SENSOR_COUNT)
			{
				data.Valid = messenger.readInt16Arg();
				char addr_str[MAX_NAME_LENGTH];
				messenger.copyStringArg(addr_str, MAX_NAME_LENGTH);
				messenger.copyStringArg(data.sid.name, MAX_NAME_LENGTH);
				data.interval = messenger.readInt16Arg();

				CheckName(data.sid.name);

				char* p = addr_str;
				char* str;
				for(byte i=0; i<8; i++){
					if((str = strtok_r(p, ":", &p)) != NULL){
						data.sid.addr[i] = strtol(str, NULL, 16);
					}
				}
			
				Settings.setTempSensorData(pos, data);

				TempSensor* sensor = TSensors.getByAddr(data.sid.addr);
				if(sensor){
					sensor->setName(data.sid.name);
					sensor->SetInterval(data.interval);
				}

				PrintResponse("ok");
			}
			else
			{
				PrintResponse("invalid cfg_pos");
			}
		}else{
			PrintResponse("unknown_command. cmdID: "+String(cmdID)+", cmd: "+cmd);
		}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}else if(cmd.equals("relay")){
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get")){
			int pos = messenger.readInt16Arg();
			RelayModuleData data = Settings.getRelayModuleData(pos);
			String response = String(data.Valid) + " " + String(data.ActiveLow) + " " + String(data.Inverted);
			PrintResponse(response);
		}else if(sub_cmd.equals("set")){
			RelayModuleData data;
			int pos = messenger.readInt16Arg();
			data.Valid = messenger.readInt16Arg();
			data.ActiveLow = messenger.readInt16Arg();
			data.Inverted = messenger.readInt16Arg();

			Settings.setRelayModuleData(pos, data);
			PrintResponse("ok");
		}else if(sub_cmd.equals("clear")){
			int pos = messenger.readInt16Arg();
			Settings.clearRelayModuleData(pos);
			PrintResponse("ok");
		}else if(sub_cmd.equals("open") || sub_cmd.equals("on")){
			int pos = messenger.readInt16Arg();
			RelayModules.OpenChannel(pos);
			PrintResponse("ok");
			PrintResponse(RelayModules.GetInfoString());
		}else if (sub_cmd.equals("close") || sub_cmd.equals("off")) {
			int pos = messenger.readInt16Arg();
			RelayModules.CloseChannel(pos);
			PrintResponse("ok");
			PrintResponse(RelayModules.GetInfoString());
		}else if (sub_cmd.equals("info")) {
			PrintResponse(RelayModules.GetInfoString());
		}
		else{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}else if(cmd.equals("trigger")){
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get")){
			int pos = messenger.readInt16Arg();
			TempTriggerData data = Settings.getTempTriggerData(pos);
			String name = String(data.Name);
			if(name == "") name = "N";
			String ttp1name = String(data.ttpiece[0].SensorName);
			if(ttp1name == "") ttp1name = "N";
			String ttp2name = String(data.ttpiece[1].SensorName);
			if(ttp2name == "") ttp2name = "N";
			String ttp3name = String(data.ttpiece[2].SensorName);
			if(ttp3name == "") ttp3name = "N";
			String ttp4name = String(data.ttpiece[3].SensorName);
			if(ttp4name == "") ttp4name = "N";

			String buff = String(data.Valid) +" " + name;
			buff += " " + String(data.ttpiece[0].Valid) + " " + ttp1name +" "+ String(data.ttpiece[0].Condition) +" "+ String(data.ttpiece[0].Temperature) +" "+ String(data.ttpiece[0].Histeresis);
			buff += " " + String(data.ttpiece[1].Valid) + " " + ttp2name +" "+ String(data.ttpiece[1].Condition) +" "+ String(data.ttpiece[1].Temperature) +" "+ String(data.ttpiece[1].Histeresis);
			buff += " " + String(data.ttpiece[2].Valid) + " " + ttp3name +" "+ String(data.ttpiece[2].Condition) +" "+ String(data.ttpiece[2].Temperature) +" "+ String(data.ttpiece[2].Histeresis);
			buff += " " + String(data.ttpiece[3].Valid) + " " + ttp4name +" "+ String(data.ttpiece[3].Condition) +" "+ String(data.ttpiece[3].Temperature) +" "+ String(data.ttpiece[3].Histeresis);
			PrintResponse(buff);
		}else if(sub_cmd.equals("set")){
			int pos = messenger.readInt16Arg();
			TempTriggerData data;
			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.Name, MAX_NAME_LENGTH);
			CheckName(data.Name);

			data.ttpiece[0].Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.ttpiece[0].SensorName, MAX_NAME_LENGTH);
			CheckName(data.ttpiece[0].SensorName);
			data.ttpiece[0].Condition = messenger.readInt16Arg();
			data.ttpiece[0].Histeresis = messenger.readInt16Arg();

			data.ttpiece[1].Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.ttpiece[1].SensorName, MAX_NAME_LENGTH);
			CheckName(data.ttpiece[1].SensorName);
			data.ttpiece[1].Condition = messenger.readInt16Arg();
			data.ttpiece[1].Histeresis = messenger.readInt16Arg();

			data.ttpiece[2].Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.ttpiece[2].SensorName, MAX_NAME_LENGTH);
			CheckName(data.ttpiece[2].SensorName);
			data.ttpiece[2].Condition = messenger.readInt16Arg();
			data.ttpiece[2].Histeresis = messenger.readInt16Arg();

			data.ttpiece[3].Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.ttpiece[3].SensorName, MAX_NAME_LENGTH);
			CheckName(data.ttpiece[3].SensorName);
			data.ttpiece[3].Condition = messenger.readInt16Arg();
			data.ttpiece[3].Histeresis = messenger.readInt16Arg();
			
			Settings.setTempTriggerData(pos, data);
			PrintResponse("ok");
		}else if(sub_cmd.equals("clear")){
			int pos = messenger.readInt16Arg();
			Settings.clearTempTriggerData(pos);
			PrintResponse("ok");
		}else if(sub_cmd.equals("info")){
			char name[MAX_NAME_LENGTH];
			messenger.copyStringArg(name, MAX_NAME_LENGTH);
			TempTrigger* trg = TempTriggers.GetByName(name);
			if(trg != NULL)
			{
				PrintResponse(trg->GetInfoString());
			}
			else
			{
				PrintResponse("null");
			}
			
		}else{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}else if(cmd.equals("circpump")){
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get")){
			byte pos = messenger.readInt16Arg();
			CircPumpData data = Settings.getCircPumpData(pos);
			String name = String(data.Name);
			String t1name = String(data.TempTriggerName);
			if(name == "") name = "N";
			if(t1name == "") t1name = "N";

			String buff = String(data.Valid)+" "+name+" "+String(data.Channel)+" "+String(data.MaxSpeed)+" "+String(data.Spd1Channel)+" "+String(data.Spd2Channel)+" "+String(data.Spd3Channel)+" "+t1name;
			PrintResponse(buff);
		}else if(sub_cmd.equals("set")){
			byte pos = messenger.readInt16Arg();
			CircPumpData data;

			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.Name, MAX_NAME_LENGTH);
			CheckName(data.Name);
			data.Channel = messenger.readInt16Arg();
			data.MaxSpeed = messenger.readInt16Arg();
			data.Spd1Channel = messenger.readInt16Arg();
			data.Spd2Channel = messenger.readInt16Arg();
			data.Spd3Channel = messenger.readInt16Arg();
			messenger.copyStringArg(data.TempTriggerName, MAX_NAME_LENGTH);
			CheckName(data.TempTriggerName);
			
			Settings.setCircPumpData(pos, data);
			PrintResponse("ok");
			//cmd circpump set 2 1 boilerio 22 1 255 255 255 N
		}
		else if (sub_cmd.equals("clear")) {
			byte pos = messenger.readInt16Arg();
			Settings.clearCircPumpData(pos);
			PrintResponse("ok");
		}
		else if (sub_cmd.equals("click"))
		{
			char name[MAX_NAME_LENGTH];
			messenger.copyStringArg(name, MAX_NAME_LENGTH);
			CirculationPump* cp = CirculationPumps.GetByName(name);
			if (cp != NULL)
			{
				cp->SwitchManualMode();
				PrintResponse("ok");
			}
			else
			{
				PrintResponse("null");
			}
		}else if(sub_cmd.equals("info"))
		{
			char name[MAX_NAME_LENGTH];
			messenger.copyStringArg(name, MAX_NAME_LENGTH);
			CirculationPump* cp = CirculationPumps.GetByName(name);
			if(cp != NULL)
			{
				PrintResponse(cp->GetInfoString());
			}
			else
			{
				PrintResponse("null");
			}
		}else{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}else if(cmd.equals("collector")){
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
			byte pos = messenger.readInt16Arg();
			CollectorsData data = Settings.getCollectorsData(pos);

			String name = String(data.Name);
			String cpump = String(data.CircPump);
			if(name == "") name = "N";
			if(cpump == "") cpump = "N";
			String valv;
			for(byte i=0; i<MAX_COLLECTOR_VALVES; i++){
				if(i>0) valv += " ";
				valv += String((byte)data.valves[i].type) + " " + String(data.valves[i].relay_channel);
			}
			
			String buff = String(data.Valid) + " " + name + " " + cpump + " " + String(data.valve_count)+" " + valv;
			PrintResponse(buff);

		}
		else if(sub_cmd.equals("set"))
		{
			byte pos = messenger.readInt16Arg();
			CollectorsData data;
			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.Name, MAX_NAME_LENGTH);
			CheckName(data.Name);
			messenger.copyStringArg(data.CircPump, MAX_NAME_LENGTH);
			CheckName(data.CircPump);
			data.valve_count = messenger.readInt16Arg();
			for(byte i=0; i<MAX_COLLECTOR_VALVES; i++){
				data.valves[i].type = (ValveType)messenger.readInt16Arg();
				data.valves[i].relay_channel = messenger.readInt16Arg();
			}
			Settings.setCollectorsData(pos, data);
			PrintResponse("ok");

		}
		else if(sub_cmd.equals("clear"))
		{
			byte pos = messenger.readInt16Arg();
			Settings.clearCollectorsData(pos);
			PrintResponse("ok");
		}else if(sub_cmd.equals("info")){
			char name[MAX_NAME_LENGTH];
			messenger.copyStringArg(name, MAX_NAME_LENGTH);
			Collector * c = Collectors.GetByName(name);
			if(c != NULL)
			{
				PrintResponse(c->GetInfoString());
			}
			else
			{
				PrintResponse("null");
			}
		}else{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("comfort"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
			byte pos = messenger.readInt16Arg();
			ComfortZonesData data = Settings.getComfortZonesData(pos);
			String name = String(data.Name);
			String cname = String(data.col_name);
			String fname = String(data.floor_sensor);
			String rname = String(data.room_sensor);
			if(name == "") name = "N";
			if(cname == "") cname = "N";
			if(fname == "") fname = "N";
			if(rname == "") rname = "N";

			String buff = String(data.Valid)+" "+
				name+" "+
				String(data.room_temp_hi)+" "+
				String(data.room_temp_low)+" "+
				String(data.floor_temp_hi)+" "+
				String(data.floor_temp_low)+" "+
				String(data.histeresis)+" "+
				cname+" "+
				String(data.channel)+" "+
				rname+" "+
				fname;
			PrintResponse(buff);

		}
		else if(sub_cmd.equals("set"))
		{
			byte pos = messenger.readInt16Arg();
			ComfortZonesData data;
			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.Name, MAX_NAME_LENGTH);
			CheckName(data.Name);
			data.room_temp_hi = messenger.readInt16Arg();
			data.room_temp_low = messenger.readInt16Arg();
			data.floor_temp_hi = messenger.readInt16Arg();
			data.floor_temp_low = messenger.readInt16Arg();
			data.histeresis = messenger.readInt16Arg();
			messenger.copyStringArg(data.col_name, MAX_NAME_LENGTH);
			CheckName(data.col_name);
			data.channel = messenger.readInt16Arg();
			messenger.copyStringArg(data.room_sensor, MAX_NAME_LENGTH);
			CheckName(data.room_sensor);
			messenger.copyStringArg(data.floor_sensor, MAX_NAME_LENGTH);
			CheckName(data.floor_sensor);
			Settings.setComfortZonesData(pos, data);
			PrintResponse("ok");
		}
		else if(sub_cmd.equals("clear"))
		{
			byte pos = messenger.readInt16Arg();
			Settings.clearComfortZonesData(pos);
			PrintResponse("ok");
		}
		else if(sub_cmd.equals("info"))
		{
			PrintResponse("unknown_command");
		}
		else
		{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if(cmd.equals("ktype"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
			byte pos = messenger.readInt16Arg();
			KTypeData data = Settings.getKTypeData(pos);
			String name = String(data.Name);
			if(name == "") name = "N";
			String buff = String(data.Valid)+" "+name+" "+String(data.interval);
			PrintResponse(buff);
		}
		else if(sub_cmd.equals("set"))
		{
			byte pos = messenger.readInt16Arg();
			KTypeData data;
			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.Name, MAX_NAME_LENGTH);
			CheckName(data.Name);
			data.interval = messenger.readInt16Arg();
			Settings.setKTypeData(pos, data);
			PrintResponse("ok");
		}
		else if (sub_cmd.equals("info"))
		{
			char name[MAX_NAME_LENGTH];
			messenger.copyStringArg(name, MAX_NAME_LENGTH);
			MAX31855 * ktp = KTypes.GetByName(name);
			if (ktp != NULL)
			{
				PrintResponse(ktp->GetInfoString());
			}
			else
			{
				PrintResponse("null");
			}
		}
		else if(sub_cmd.equals("clear"))
		{
			byte pos = messenger.readInt16Arg();
			Settings.clearKTypeData(pos);
			PrintResponse("ok");
		}
		else
		{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("katilas"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
			byte pos = messenger.readInt16Arg();
			WoodBoilerData data = Settings.getWoodBoilerData(pos);
			String sname = String(data.TSensorName);
			String kname = String(data.KTypeName);
			if(sname == "") sname = "N";
			if(kname == "") kname = "N";

			String buff = String(data.Valid)+" "+sname+" "+kname+" "+String(data.LadomatChannel)+" "+String(data.ExhaustFanChannel)+" "+String(data.WorkingTemp)+" "+String(data.WorkingHisteresis)+" "+String(data.LadomatTemp)+" "+String(data.LadomatTempTriggerName);
			PrintResponse(buff);
		}
		else if(sub_cmd.equals("set"))
		{
			byte pos = messenger.readInt16Arg();
			WoodBoilerData data;

			data.Valid = messenger.readInt16Arg();
			messenger.copyStringArg(data.TSensorName, MAX_NAME_LENGTH);
			CheckName(data.TSensorName);
			messenger.copyStringArg(data.KTypeName, MAX_NAME_LENGTH);
			CheckName(data.KTypeName);
			data.LadomatChannel = messenger.readInt16Arg();
			data.ExhaustFanChannel = messenger.readInt16Arg();
			data.WorkingTemp = messenger.readInt16Arg();
			data.WorkingHisteresis = messenger.readInt16Arg();
			data.LadomatTemp = messenger.readInt16Arg();
			messenger.copyStringArg(data.LadomatTempTriggerName, MAX_NAME_LENGTH);
			CheckName(data.LadomatTempTriggerName);
			Settings.setWoodBoilerData(pos, data);
			PrintResponse("ok");
		}
		else if(sub_cmd.equals("info"))
		{
			PrintResponse(WoodBoilers.Get(0)->GetInfoString());
		}
		else if(sub_cmd.equals("clear"))
		{
			PrintResponse("unknown_command");
		}
		else
		{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("simulation"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("set"))
		{
			bool res = false;
			String arg = String(messenger.readStringArg());
			if(arg.equals("on"))
			{
				res = WoodBoilers.Get(0)->SetSimulationMode(true);
				if(res)
				{
					PrintResponse("Entered into simulation mode!");
				}
				else
				{
					PrintResponse("Failed to enter Simulation mode.");
				}
			}
			else if(arg.equals("off"))
			{
				res = WoodBoilers.Get(0)->SetSimulationMode(false);
				if(res)
				{
					PrintResponse("Exited Simulation mode!");
				}
				else
				{
					PrintResponse("Failed to exit Simulation mode.");
				}
			}
			else if(arg.equals("temp"))
			{
				float tmp = messenger.readFloatArg();
				res = WoodBoilers.Get(0)->SetSimulationTemp(tmp);
				if(res)
				{
					PrintResponse("Temp change success!");
				}
				else
				{
					PrintResponse("Failed to change temp.");
				}
			}
		}
		else
		{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}
	
	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("switch"))
	{
		String sub_cmd = String(messenger.readStringArg());	
		if(sub_cmd.equals("get"))
		{
			int pos = messenger.readInt16Arg();
			
			if(pos < MAX_SWITCHES_COUNT)
			{
				SwitchData data = Settings.getSwitchData(pos);
				String name = String(data.Name);
				String dname = String(data.DependOn);
				if(name == "") name = "N";
				if(dname == "") dname = "N";
				String response = String(data.Valid) + " " +
					name + " " +
					dname + " " +
					String((int)data.depstate) + " " +
					String((int)data.forcestate);
				PrintResponse(response);
			}
			else
			{
				PrintResponse("null");
			}
		}
		else if(sub_cmd.equals("set"))
		{

		}
		else if(sub_cmd.equals("click"))
		{
			char sname[MAX_NAME_LENGTH];
			messenger.copyStringArg(sname, MAX_NAME_LENGTH);
			Switch* swth = Switches.GetByName(sname);
			if(swth != NULL)
			{
				swth->SwitchState();
				PrintResponse("ok");
			}
			else
			{
				PrintResponse("null "+String(sname));
			}
		}
		else if(sub_cmd.equals("info"))
		{
			char sname[MAX_NAME_LENGTH];
			messenger.copyStringArg(sname, MAX_NAME_LENGTH);
			Switch* swth = Switches.GetByName(sname);
			if(swth != NULL)
			{
				PrintResponse(swth->GetInfoString());
			}
			else
			{
				PrintResponse("null "+String(sname));
			}
		}
		else
		{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}

	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("eeprom"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
			int pos = messenger.readInt16Arg();
			PrintResponse("EEPROM: "+String(pos)+" "+String(EEPROM.GetByte(pos)));
		}
		else if(sub_cmd.equals("set"))
		{
			int pos = messenger.readInt16Arg();
			int val = messenger.readInt16Arg();
			EEPROM.SetByte(pos, (byte)val);
			PrintResponse("ok");
		}else{
			PrintResponse("unknown_command. cmdID: " + String(cmdID) + ", cmd: " + cmd);
		}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if(cmd.equals("freeze"))
	{
		PrintResponse("Arduino frozen :(");
		while(true)
		{
		}

	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("ikurimas"))
	{
		WoodBoilers.Get(0)->EnterPhase2();
		PrintResponse("done");

	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("NSU_Ready"))
	{
		Log.log("NSU_CMD_Server_Started");

	
	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("reinittft"))
	{
		//Windows.ReInitTFT();
		PrintResponse("ok");

	
	}////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("set_time"))
	{
		int hour = messenger.readInt16Arg();
		int min = messenger.readInt16Arg();
		int sec = messenger.readInt16Arg();
		int year = messenger.readInt16Arg();
		int month = messenger.readInt16Arg();
		int day = messenger.readInt16Arg();

		rtc.setTime(hour, min, sec, 24);
		rtc.setDate(year, month, day, 1);
		PrintResponse("done Week day is: "+String(rtc.weekDay(rtc.getTimestamp())));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	else if(cmd.equals("freemem"))
	{
		PrintResponse(String(FreeMem::GetFreeMem()));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals("reply"))
	{
		String resp = String(messenger.readStringArg());
		String nextresp = String(messenger.readStringArg());

		while (messenger.isArgOk())
		{
			resp += " ";
			resp += nextresp;
			nextresp = String(messenger.readStringArg());
		}
		PrintResponse(resp);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if (cmd.equals("system"))
	{
		String sub_cmd = String(messenger.readStringArg());
		if (sub_cmd.equals("get"))
		{
			String task = String(messenger.readStringArg());
			if (task.equals("cfgid") || task.equals("configid"))
			{
				PrintResponse("00000000-0000-0000-0000-000000000000");
			}
		}
		else if (sub_cmd.equals("set"))
		{
		}
		else if (sub_cmd.equals("clear"))
		{
		}
		else
		{
			PrintResponse("unknown_command");
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(cmd.equals(""))
	{
		String sub_cmd = String(messenger.readStringArg());
		if(sub_cmd.equals("get"))
		{
		}
		else if(sub_cmd.equals("set"))
		{
		}
		else if(sub_cmd.equals("clear"))
		{
		}
		else
		{
			PrintResponse("unknown_command");
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else
	{
		//Unknown command
		PrintResponse("unknown_command. cmdID: [" + String(cmdID) + "], cmd: [" + cmd +"]");
	}

}

//void CmdServer::PrintJSON(JsonObject & jo)
//{
//	if (cmdID[0])
//	{
//		jo[jKeyCmdID] = cmdID;
//	}
//	PrintJsonResponse(jo);
//}

void CmdServer::ParseJsonMessage(String& json)
{
	StaticJsonBuffer<JsonBuffSize> jsonBuffer;
	JsonObject& jo = jsonBuffer.parseObject(json);
	if (!jo.success())
	{
		PrintJson.PrintResultParseError();
		return;
	}

	if (!jo.containsKey(jKeyTarget))
	{
		PrintJson.PrintResultFormatError();
		return;

	}
	String target = jo[jKeyTarget];

	//save for response
	if (jo.containsKey(jKeyCmdID))
	{
		PrintJson.setCmdID(jo[jKeyCmdID]);
	}
	else
	{
		PrintJson.setCmdID("");
	}

	Log.debug("CmdServer::ParseJsonMessage() Target - " + target);
	if (target.equals(jTargetBoot))
	{
		BootChecker.ParseJson(jo);
		return;
	}
	if (target.equals(jTargetSystem))
	{
		SystemStatus.ParseJson(jo);
		return;
	}
	if (target.equals(jTargetSwitch))
	{
		Switches.ParseJson(jo);
		return;
	}
	if (target.equals(jTargetCircPump))
	{
		CirculationPumps.ParseJson(jo);
		return;
	}
	if (target.equals(jTargetWoodBoiler))
	{
		WoodBoilers.ParseJson(jo);
		return;
	}
	if (target.equals(jTargetFileManager))
	{
		FileManager.ParseJson(jo);
		return;
	}
	Log.debug("CmdServer::ParseJsonMessage(). Target ["+target+"] not found.");
}


CmdServer cmdServer(Serial, ' ', '\n');
