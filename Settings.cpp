#include "Settings.h"
#include <SPI.h>

SettingsClass::SettingsClass(void)
{
	isReady = true;
}


SettingsClass::~SettingsClass(void)
{
}

bool SettingsClass::IsReady(){
	return isReady && FileManager.SDCardOk();
}

bool SettingsClass::Begin()
{

	addrtable.BoilerisAddr = 0;
	addrtable.CirculationPumpsAddr = 0;
	addrtable.CollectorsAddr = 0;
	addrtable.ComfortZonesAddr = 0;
	addrtable.KatilasAddr = 0;
	addrtable.KTypeAddr = 0;
	addrtable.MyAddr = 0;
	addrtable.RelayAddr = 0;
	addrtable.TempTriggersAddr = 0;
	addrtable.TSensorsAddr = 0;
	addrtable.SwitchAddr = 0;

	if(!FileManager.SDCardOk()){
		Log.error("SettingsClass SD access error!");
		return false;
	}
	if(!FileManager.SDFat().exists(CONFIG_FILE))
	{
		Log.debug("Config file does not exist. Creating.");
		if(file.open(CONFIG_FILE, O_RDWR | O_CREAT))
		{
			create_structure();
			file.close();
			isReady = true;
		}
		else//file not created
		{
			isReady = false;
		}
	}
	else if (file.open(CONFIG_FILE, O_RDWR))
	{
		Log.debug("Going to check file version...");
		isReady = checkVersion();
		file.close();
	}
	return isReady;
}

bool SettingsClass::BeginLongRead()
{
	if (!IsReady()) return false;
	if (!longread)
	{
		if (!file.isOpen())
		{
			if (file.open(CONFIG_FILE, O_RDWR))
			{
				longread = true;
			}
			else
			{

				longread = false;
				return longread;
			}
		}
		longread = true;
	}
	return longread;
}

void SettingsClass::EndLongRead()
{
	if (file.isOpen())
	{
		file.close();
	}
	longread = false;
}

bool SettingsClass::checkReadWrite(int value)
{
	return value > 0 ? true : false;
}

bool SettingsClass::beginflashread(unsigned int addr){
	if (!IsReady()) return false;
	if (!file.isOpen())
		file.open(CONFIG_FILE, O_RDWR);
	return file.seekSet(addr);
}

bool SettingsClass::beginflashwrite(unsigned int addr){
	if (!IsReady()) return false;
	if (!file.isOpen())
		file.open(CONFIG_FILE, O_RDWR);
	return file.seekSet(addr);
}

void SettingsClass::endflash()
{
	if (longread) return;
	if (!FileManager.SDCardOk()) return;
	if(file.isOpen()) file.close();
}

byte SettingsClass::readbyte()
{
	return file.read();
}

int SettingsClass::readint()
{
	int32_t res = 0;
	file.read(&res, sizeof(res));

	return res;
}

void SettingsClass::writebyte(byte data){
	//if(!FileManager.SDCardOk()) return;
	file.write(data);
}

void SettingsClass::writeint(int32_t data){
	//if(!FileManager.SDCardOk()) return;
	file.write(&data, sizeof(data));
}

void SettingsClass::writezeros(int count)
{
	for(int i =0; i < count; i++)
	{
		file.write((byte)0);
	}
}

bool SettingsClass::checkVersion(){
	VersionInfo version;
	
	if(!beginflashread(0)){
		Log.debug("checkVersion().beginflashread(0) failed.");
		return false;
	}

	if(file.fileSize() >= 8){
		if (!checkReadWrite(file.read(&version, sizeof(VersionInfo))))
		{
			Log.debug("file.read(&version, sizeof(VersionInfo)) failed.");
			return false;
		}
	}else{
		version.vstring[0] = 0;
		version.vstring[1] = 0;
		version.vstring[2] = 0;
		version.version = 0;
	}

	Log.debug("NSU Version: V[0]: " + String(version.vstring[0]) + ", V[1]: " + String(version.vstring[1]) + ", V[2]: " + String(version.vstring[2]) + ", V: " + String(version.version));
	

	if(version.vstring[0] != 'N' || version.vstring[1] != 'S' || version.vstring[2] != 'U')
	{
		Log.debug("Version signature mismatch. Recreating config file with defaults.");
		file.truncate(0);
		create_structure();
		//Set position to read AddrTable and keep file open
		beginflashread(addrtable.MyAddr);
	}
	else if (version.version < SETTINGS_VERSION)
	{
		Log.debug("SettingsClass::checkVersion() - Version in '" + String(CONFIG_FILE) + "' is lower. Converting to current.");
		if (file.isOpen()) file.close();
		if (!FileManager.SDFat().remove(CONFIG_FILE))// .rename(CONFIG_FILE, CONFIG_FILE_OLD);
		{
			Log.debug("SettingsClass::checkVersion() - Failed to remove '" + String(CONFIG_FILE) + "'");
			return false;
		}
		if (!file.open(CONFIG_FILE, O_RDWR | O_CREAT))
		{
			Log.debug("SettingsClass::checkVersion() - Failed to ctreate new file '" + String(CONFIG_FILE) + "'");
			return false;
		}
		//SdFile fileOld;
		//fileOld.open(CONFIG_FILE_OLD, O_RDWR);
		//convert(fileOld, file);
		Log.debug("SettingsClass::checkVersion() - New '" + String(CONFIG_FILE) + "' file created. Calling convert().");
		convert();
		//Set position to read AddrTable and keep file open
		beginflashread(addrtable.MyAddr);
	}

	if (!checkReadWrite(file.read(&addrtable, sizeof(DataAddresses))))
	{
		Log.debug("SettingsClass::checkVersion() - Failed to read AddrTable");
		return false;
	}
	Log.debug("addrtable.MyAddr = " + String(addrtable.MyAddr));
	Log.debug("addrtable.TSensorsAddr = " + String(addrtable.TSensorsAddr));
	Log.debug("addrtable.RelayAddr = " + String(addrtable.RelayAddr));
	Log.debug("addrtable.TempTriggersAddr = " + String(addrtable.TempTriggersAddr));
	Log.debug("addrtable.CirculationPumpsAddr = " + String(addrtable.CirculationPumpsAddr));
	Log.debug("addrtable.CollectorsAddr = " + String(addrtable.CollectorsAddr));
	Log.debug("addrtable.ComfortZonesAddr = " + String(addrtable.ComfortZonesAddr));
	Log.debug("addrtable.KTypeAddr = " + String(addrtable.KTypeAddr));
	Log.debug("addrtable.KatilasAddr = " + String(addrtable.KatilasAddr));

	return true;
}

void SettingsClass::create_structure(){
	int i;
	Log.debug("Creating config file structure...");
	
	beginflashwrite(0);
	//write version
	VersionInfo version;
	version.version = SETTINGS_VERSION;
	version.vstring[0] = 'N';
	version.vstring[1] = 'S';
	version.vstring[2] = 'U';
	file.write(&version, sizeof(VersionInfo));
	
	addrtable.MyAddr = file.curPosition();
	//allocate space for data addresses
	writezeros(sizeof(DataAddresses));
	
	//write switch data
	addrtable.SwitchAddr = file.curPosition();
	for(i=0; i<MAX_SWITCHES_COUNT; i++){
		writezeros(sizeof(SwitchData));
	}

	//write temperature sensor data
	addrtable.TSensorsAddr = file.curPosition();
	for(i=0; i<MAX_SENSOR_COUNT; i++){
		writezeros(sizeof(TSensorsData));
	}
	
	//Relay module
	addrtable.RelayAddr = file.curPosition();
	for(i=0; i<MAX_RELAY_MODULES; i++)
	{
		writezeros(sizeof(RelayModuleData));
	}
	
	//Temp triggers
	addrtable.TempTriggersAddr = file.curPosition();
	for(i=0; i<MAX_TEMP_TRIGGERS; i++){
		writezeros(sizeof(TempTriggerData));
	}
	
	//Cirkuliaciniai siurbliai
	addrtable.CirculationPumpsAddr = file.curPosition();
	for(i=0; i<MAX_CIRCULATION_PUMPS; i++){
		writezeros(sizeof(CircPumpData));
	}
	
	//Collectors
	addrtable.CollectorsAddr = file.curPosition();
	for(i=0; i<MAX_COLLECTORS; i++){
		writezeros(sizeof(CollectorsData));
	}

	//Comfort zones
	addrtable.ComfortZonesAddr = file.curPosition();
	for(i=0; i<MAX_COMFORT_ZONES; i++){
		writezeros(sizeof(ComfortZonesData));
	}

	//KType
	addrtable.KTypeAddr = file.curPosition();
	for(i=0; i<MAX_KTYPE_SENSORS; i++){
		writezeros(sizeof(KTypeData));
	}

	//Boilerio informacija
	addrtable.BoilerisAddr = file.curPosition();
	writezeros(sizeof(WaterBoilerData));

	//Katilo informacija
	addrtable.KatilasAddr = file.curPosition();
	writezeros(sizeof(WoodBoilerData));

	//Write addresses
	file.seekSet(addrtable.MyAddr);

	file.write(&addrtable, sizeof(DataAddresses));

	file.sync();
	endflash();

	Log.debug("Config file structure created.");
}

void SettingsClass::Log_SD_Or_Pos_Error(byte pos)
{
	Log.error("SD Error OR wrong position (" + String(pos) + ")! Returning default data!");
}

const TSensorsData& SettingsClass::getTempSensorData(byte pos){
	if (IsReady() && pos < MAX_SENSOR_COUNT)
	{
		beginflashread(addrtable.TSensorsAddr + pos*sizeof(TSensorsData));
		file.read(&tsensorData, sizeof(TSensorsData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultTSensorData(pos);
	}
	return tsensorData;
}

const SwitchData& SettingsClass::getSwitchData(byte pos){
	if (IsReady() && pos < MAX_SWITCHES_COUNT)
	{
		beginflashread(addrtable.SwitchAddr + pos*sizeof(SwitchData));
		file.read(&switchData, sizeof(SwitchData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultSwitchData(pos);
	}
	return switchData;
}

const RelayModuleData& SettingsClass::getRelayModuleData(byte pos){
	if (IsReady() && pos < MAX_RELAY_MODULES)
	{
		beginflashread(addrtable.RelayAddr + pos*sizeof(RelayModuleData));
		file.read(&relayData, sizeof(RelayModuleData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultRelayModuleData(pos);
	}
	return relayData;
}
 
const TempTriggerData& SettingsClass::getTempTriggerData(byte pos){
	if (IsReady() && pos < MAX_TEMP_TRIGGERS)
	{
		beginflashread(addrtable.TempTriggersAddr + pos*sizeof(TempTriggerData));
		file.read(&triggerData, sizeof(TempTriggerData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultTempTriggerData(pos);
	}
	return triggerData;
}
 
const CircPumpData& SettingsClass::getCircPumpData(byte pos){
	if (IsReady() && pos < MAX_CIRCULATION_PUMPS)
	{
		beginflashread(addrtable.CirculationPumpsAddr + pos*sizeof(CircPumpData));
		file.read(&cpumpData, sizeof(CircPumpData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultCircPumpData(pos);
	}
	return cpumpData;
}
 
const CollectorsData& SettingsClass::getCollectorsData(byte pos){
	if (IsReady() && pos < MAX_COLLECTORS)
	{
		beginflashread(addrtable.CollectorsAddr + pos*sizeof(CollectorsData));
		file.read(&collectorData, sizeof(CollectorsData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultCollectorData(pos);
	}
	return collectorData;
}
 
const ComfortZonesData& SettingsClass::getComfortZonesData(byte pos){
	if (IsReady() && MAX_COMFORT_ZONES)
	{
		beginflashread(addrtable.ComfortZonesAddr + pos*sizeof(ComfortZonesData));
		file.read(&czonesData, sizeof(ComfortZonesData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultComfortZoneData(pos);
	}
	return czonesData;
}
 
const KTypeData& SettingsClass::getKTypeData(byte pos){
	if (IsReady() && pos < MAX_KTYPE_SENSORS)
	{
		beginflashread(addrtable.KTypeAddr);
		file.read(&ktypeData, sizeof(KTypeData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultKTypeData(pos);
	}
	return ktypeData;
}
 
const WoodBoilerData& SettingsClass::getWoodBoilerData(byte pos){
	if (IsReady() && pos < MAX_WOOD_BOILERS_COUNT)
	{
		beginflashread(addrtable.KatilasAddr);
		file.read(&woodBoilerData, sizeof(WoodBoilerData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultWoodBoilerData(pos);
	}
	return woodBoilerData;
}
 
const WaterBoilerData& SettingsClass::getWaterBoilerData(byte pos)
{
	if (IsReady() && pos < MAX_WATERBOILER_COUNT)
	{
		beginflashread(addrtable.BoilerisAddr);
		file.read(&waterBoilerData, sizeof(WaterBoilerData));
		endflash();
	}
	else
	{
		Log_SD_Or_Pos_Error(pos);
		return GetDefaultWaterBoilerData(pos);
	}
	return waterBoilerData;
}



void SettingsClass::setTempSensorData(byte pos, const TSensorsData& data){
	beginflashwrite(addrtable.TSensorsAddr + pos*sizeof(TSensorsData));
	file.write(&data, sizeof(TSensorsData));
	file.sync();
	endflash();
}
 
void SettingsClass::setSwitchData(byte pos, const SwitchData& data){
	beginflashwrite(addrtable.SwitchAddr + pos*sizeof(SwitchData));
	file.write(&data, sizeof(SwitchData));
	file.sync();
	endflash();
}

void SettingsClass::setRelayModuleData(byte pos, const RelayModuleData& data){
	beginflashwrite(addrtable.RelayAddr + pos*sizeof(RelayModuleData));
	file.write(&data, sizeof(RelayModuleData));
	file.sync();
	endflash();
}
 
void SettingsClass::setTempTriggerData(byte pos, const TempTriggerData& data){
	beginflashwrite(addrtable.TempTriggersAddr + pos*sizeof(TempTriggerData));
	file.write(&data, sizeof(TempTriggerData));
	file.sync();
	endflash();
}
 
void SettingsClass::setCircPumpData(byte pos, const CircPumpData& data){
	beginflashwrite(addrtable.CirculationPumpsAddr + pos*sizeof(CircPumpData));
	file.write(&data, sizeof(CircPumpData));
	file.sync();
	endflash();
}
 
void SettingsClass::setCollectorsData(byte pos, const CollectorsData& data){
	beginflashwrite(addrtable.CollectorsAddr + pos*sizeof(CollectorsData));
	file.write(&data, sizeof(CollectorsData));
	file.sync();
	endflash();
}
 
void SettingsClass::setComfortZonesData(byte pos, const ComfortZonesData& data){
	beginflashwrite(addrtable.ComfortZonesAddr + pos*sizeof(ComfortZonesData));
	file.write(&data, sizeof(ComfortZonesData));
	file.sync();
	endflash();
}
 
void SettingsClass::setKTypeData(byte pos, const KTypeData& data){
	beginflashwrite(addrtable.KTypeAddr);
	file.write(&data, sizeof(KTypeData));
	file.sync();
	endflash();
}
 
void SettingsClass::setWoodBoilerData(byte pos, const WoodBoilerData& data){
	beginflashwrite(addrtable.KatilasAddr);
	file.write(&data, sizeof(WoodBoilerData));
	file.sync();
	endflash();
}
 
void SettingsClass::setWaterBoilerData(byte pos, const WaterBoilerData& data)
{
	beginflashwrite(addrtable.BoilerisAddr);
	file.write(&data, sizeof(WaterBoilerData));
	file.sync();
	endflash();
}



void SettingsClass::clearTempSensorData(byte pos){
	beginflashwrite(addrtable.TSensorsAddr + pos*sizeof(TSensorsData));
	writezeros(sizeof(TSensorsData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearSwitchData(byte pos){
	beginflashwrite(addrtable.SwitchAddr + pos*sizeof(SwitchData));
	writezeros(sizeof(SwitchData));
	file.sync();
	endflash();
}

void SettingsClass::clearRelayModuleData(byte pos){
	beginflashwrite(addrtable.RelayAddr + pos*sizeof(RelayModuleData));
	writezeros(sizeof(RelayModuleData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearTempTriggerData(byte pos){
	beginflashwrite(addrtable.TempTriggersAddr + pos*sizeof(TempTriggerData));
	writezeros(sizeof(TempTriggerData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearCircPumpData(byte pos){
	beginflashwrite(addrtable.CirculationPumpsAddr + pos*sizeof(CircPumpData));
	writezeros(sizeof(CircPumpData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearCollectorsData(byte pos){
	beginflashwrite(addrtable.CollectorsAddr + pos*sizeof(CollectorsData));
	writezeros(sizeof(CollectorsData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearComfortZonesData(byte pos){
	beginflashwrite(addrtable.ComfortZonesAddr + pos*sizeof(ComfortZonesData));
	writezeros(sizeof(ComfortZonesData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearKTypeData(byte pos){
	beginflashwrite(addrtable.KTypeAddr);
	writezeros(sizeof(KTypeData));
	file.sync();
	endflash();
}
 
void SettingsClass::clearWaterBoilerData(byte pos)
{
	beginflashwrite(addrtable.BoilerisAddr);
	writezeros(sizeof(WaterBoilerData));
	file.sync();
	endflash();
}

void SettingsClass::clearWoodBoilerData(byte pos){
	beginflashwrite(addrtable.KatilasAddr);
	writezeros(sizeof(WoodBoilerData));
	file.sync();
	endflash();
}

void SetAddr(byte* a, byte b0, byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7)
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

const SwitchData& SettingsClass::GetDefaultSwitchData(byte pos)
{
	switch (pos)
	{
	case 0:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_WINTER_MODE].c_str(), MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (byte)STATUS_OFF;
		switchData.forcestate = (byte)STATUS_OFF;
		switchData.Valid = 1;
		break;
	case 1:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_TEMPERATURE_MODE].c_str(), MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (byte)STATUS_OFF;
		switchData.forcestate = (byte)STATUS_OFF;
		switchData.Valid = 1;
		break;
	case 2:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_HOT_WATER_MODE].c_str(), MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		strlcpy(switchData.DependOn, SWITCH_NAMES[SWITCH_NAME_WINTER_MODE].c_str(), MAX_NAME_LENGTH);
		switchData.depstate = (byte)STATUS_OFF;
		switchData.forcestate = (byte)STATUS_ON;
		switchData.Valid = 1;
		break;
	case 3:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		strlcpy(switchData.Name, SWITCH_NAMES[SWITCH_NAME_BURN_MODE].c_str(), MAX_NAME_LENGTH);
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (byte)STATUS_OFF;
		switchData.forcestate = (byte)STATUS_OFF;
		switchData.Valid = 1;
		break;
	default:
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.Name[i] = 0;
		for (int i = 0; i < MAX_NAME_LENGTH; i++) switchData.DependOn[i] = 0;
		switchData.depstate = (byte)STATUS_OFF;
		switchData.forcestate = (byte)STATUS_OFF;
		switchData.Valid = 0;
		break;
	}

	return switchData;

}

const TSensorsData& SettingsClass::GetDefaultTSensorData(byte pos)
{
	tsensorData.Valid = 1;
	switch (pos)
	{
	case 0:
		SetAddr(tsensorData.sid.addr, 0x28, 0x80, 0x1f, 0x94, 0x4, 0x0, 0x0, 0x96);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "tualetas", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 1:
		SetAddr(tsensorData.sid.addr, 0x28, 0xe0, 0x14, 0x94, 0x4, 0x0, 0x0, 0x25);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "svetaine_maza", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 2:
		SetAddr(tsensorData.sid.addr, 0x28, 0x54, 0x5, 0x94, 0x4, 0x0, 0x0, 0x4f);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_vidurys", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 3:
		SetAddr(tsensorData.sid.addr, 0x28, 0x2a, 0xa, 0x94, 0x4, 0x0, 0x0, 0xab);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "boileris_virsus", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 4:
		SetAddr(tsensorData.sid.addr, 0x28, 0x3e, 0x93, 0xa0, 0x4, 0x0, 0x0, 0x2e);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_apacia", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 5:
		SetAddr(tsensorData.sid.addr, 0x28, 0xfe, 0xe9, 0x93, 0x4, 0x0, 0x0, 0xa1);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "svetaine_didele", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 6:
		SetAddr(tsensorData.sid.addr, 0x28, 0xa1, 0x5, 0x94, 0x4, 0x0, 0x0, 0xd6);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "virtuve", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 7:
		SetAddr(tsensorData.sid.addr, 0x28, 0xf1, 0xf0, 0x93, 0x4, 0x0, 0x0, 0xa);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "katilas", MAX_NAME_LENGTH);
		tsensorData.interval = 15;
		break;
	case 8:
		SetAddr(tsensorData.sid.addr, 0x28, 0x25, 0x42, 0x94, 0x4, 0x0, 0x0, 0x58);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "darbo_kambarys", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 9:
		SetAddr(tsensorData.sid.addr, 0x28, 0x5d, 0x47, 0xa0, 0x4, 0x0, 0x0, 0xf5);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "arduino", MAX_NAME_LENGTH);
		tsensorData.interval = 60;
		break;
	case 10:
		SetAddr(tsensorData.sid.addr, 0x28, 0x3, 0xe3, 0x93, 0x4, 0x0, 0x0, 0x24);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "koridorius", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 11:
		SetAddr(tsensorData.sid.addr, 0x28, 0x43, 0xb9, 0x93, 0x4, 0x0, 0x0, 0x79);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "sveciu_kamb", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 12:
		SetAddr(tsensorData.sid.addr, 0x28, 0x87, 0xb1, 0x93, 0x4, 0x0, 0x0, 0x4);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "boileris_apacia", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 13:
		SetAddr(tsensorData.sid.addr, 0x28, 0x9f, 0xd7, 0x9f, 0x4, 0x0, 0x0, 0x61);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "svabrine", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 14:
		SetAddr(tsensorData.sid.addr, 0x28, 0xff, 0x78, 0x93, 0x4, 0x0, 0x0, 0xec);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "akum_virsus", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 15:
		SetAddr(tsensorData.sid.addr, 0x28, 0x41, 0x93, 0x93, 0x4, 0x0, 0x0, 0x52);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "laukas", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 16:
		SetAddr(tsensorData.sid.addr, 0x28, 0x11, 0x45, 0xA0, 0x4, 0x0, 0x0, 0x7e);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "pozemis", MAX_NAME_LENGTH);
		tsensorData.interval = 30;
		break;
	case 17:
		SetAddr(tsensorData.sid.addr, 0x28, 0xfe, 0x05, 0xa0, 0x4, 0x0, 0x0, 0x9a);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "rusys1", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 18:
		SetAddr(tsensorData.sid.addr, 0x28, 0x48, 0xa0, 0xa0, 0x4, 0x0, 0x0, 0xce);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "rusys2", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	case 19:
		SetAddr(tsensorData.sid.addr, 0x28, 0xB2, 0x8A, 0x9F, 0x4, 0x0, 0x0, 0xE4);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		strlcpy(tsensorData.sid.name, "grindu_paduodamas", MAX_NAME_LENGTH);
		tsensorData.interval = 240;
		break;
	default:
		SetAddr(tsensorData.sid.addr, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) tsensorData.sid.name[i] = 0;
		tsensorData.interval = 0;
		tsensorData.Valid = 0;
		break;
	}

	return tsensorData;
}

const RelayModuleData& SettingsClass::GetDefaultRelayModuleData(byte pos)
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

const TempTriggerData& SettingsClass::GetDefaultTempTriggerData(byte pos)
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
			strlcpy(triggerData.Name, "akumuliac42", MAX_NAME_LENGTH);

			triggerData.ttpiece[0].Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) triggerData.ttpiece[0].SensorName[i] = 0;
			strlcpy(triggerData.ttpiece[0].SensorName, "akum_virsus", MAX_NAME_LENGTH);
			triggerData.ttpiece[0].Condition = (TriggerCondition)1;
			triggerData.ttpiece[0].Temperature = 4200;
			triggerData.ttpiece[0].Histeresis = 200;

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

const CircPumpData& SettingsClass::GetDefaultCircPumpData(byte pos)
{
		switch (pos)
		{
		case 0:
			cpumpData.Valid = 1;
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.Name[i] = 0;
			strlcpy(cpumpData.Name, "grindu", MAX_NAME_LENGTH);
			for (int i = 0; i < MAX_NAME_LENGTH; i++) cpumpData.TempTriggerName[i] = 0;
			strlcpy(cpumpData.TempTriggerName, "akumuliac42", MAX_NAME_LENGTH);
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

const CollectorsData& SettingsClass::GetDefaultCollectorData(byte pos)
{
	switch (pos)
	{
	case 0:
		collectorData.Valid = 1;
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		strlcpy(collectorData.Name, "1aukstas", MAX_NAME_LENGTH);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;
		strlcpy(collectorData.CircPump, "grindu", MAX_NAME_LENGTH);

		collectorData.valve_count = 10;

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
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		strlcpy(collectorData.Name, "rusys", MAX_NAME_LENGTH);
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;
		strlcpy(collectorData.CircPump, "radiatoriu", MAX_NAME_LENGTH);

		collectorData.valve_count = 2;

		collectorData.valves[0].type = (ValveType)0;
		collectorData.valves[0].relay_channel = 0xFF;

		collectorData.valves[1].type = (ValveType)0;
		collectorData.valves[1].relay_channel = 0xFF;

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
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.Name[i] = 0;
		for (byte i = 0; i < MAX_NAME_LENGTH; i++) collectorData.CircPump[i] = 0;

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

const ComfortZonesData& SettingsClass::GetDefaultComfortZoneData(byte pos)
{
	switch (pos)
	{
	case 0:
		//Name: svetaine_maza Channel: 1 Col name: 1aukstas Room sensor: svetaine_maza Floor sensor:  Temperature: 2100 Histeresis: 100
		czonesData.Valid = 1;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "svetaine_maza", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Svetaine maza", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svetaine_maza", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 1;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "svetaine_didele", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Svetaine didele", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svetaine_didele", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 3;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "darbo_kambarys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Darbo kambarys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "darbo_kambarys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 2;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "sveciu_kambarys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Sveciu kambarys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "sveciu_kamb", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 4;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "skalbykla", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Skalbykla", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "svabrine", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 6;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "koridorius", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Koridorius", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "koridorius", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 7;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "tualetas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Tualetas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "tualetas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 8;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "virtuve", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Virtuve", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "1aukstas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "virtuve", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 5;
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
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "rusys1", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Rusys1", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "rusys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "rusys1", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 255;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	case 9:
		//Name: rusys2 Channel: 255 Col name: rusys Room sensor: rusys2 Floor sensor:  Temperature: 2000 Histeresis: 100
		czonesData.Valid = 1;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		strlcpy(czonesData.Name, "rusys2", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Title[j] = 0;
		strlcpy(czonesData.Title, "Rusys2", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		strlcpy(czonesData.col_name, "rusys", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		strlcpy(czonesData.room_sensor, "rusys2", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 255;
		czonesData.room_temp_hi = 2100;
		czonesData.room_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.floor_temp_hi = 2400; //   <<<<<------- pakeitimas
		czonesData.floor_temp_low = 1000; //   <<<<<------- pakeitimas
		czonesData.histeresis = 50;
		czonesData.low_temp_mode = 0;
		break;
	
	default:
		czonesData.Valid = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.Name[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.col_name[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.room_sensor[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) czonesData.floor_sensor[j] = 0;
		//strcpy(data.floor_sensor, "");
		czonesData.channel = 0xff;
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

const KTypeData& SettingsClass::GetDefaultKTypeData(byte pos)
{
	switch (pos)
	{
	case 0:
		ktypeData.Valid = 1;
		ktypeData.interval = 5;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) ktypeData.Name[j] = 0;
		strlcpy(ktypeData.Name, "dumu_temp", MAX_NAME_LENGTH);
		break;
	default:
		ktypeData.Valid = 0;
		ktypeData.interval = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) ktypeData.Name[j] = 0;
		break;
	}
	return ktypeData;
}

const WaterBoilerData& SettingsClass::GetDefaultWaterBoilerData(byte pos)
{
	switch (pos)
	{
	case 0:
		waterBoilerData.Valid = 1;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.Name[j] = 0;
		strlcpy(waterBoilerData.Name, "default", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TSensorName[j] = 0;
		strlcpy(waterBoilerData.TSensorName, "boileris_virsus", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TempTriggerName[j] = 0;
		strlcpy(waterBoilerData.TempTriggerName, "boilerio", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.CircPumpName[j] = 0;
		strlcpy(waterBoilerData.CircPumpName, "boilerio", MAX_NAME_LENGTH);
		waterBoilerData.ElHeatingEnabled = 0;
		waterBoilerData.ElHeatingChannel = 0xFF;

		for (byte i = 0; i<7; i++)
		{
			waterBoilerData.HeatingData[i].StartHour = 0;
			waterBoilerData.HeatingData[i].StartMin = 0;
			waterBoilerData.HeatingData[i].EndHour = 0;
			waterBoilerData.HeatingData[i].EndMin = 0;
		}
		break;
	default:
		waterBoilerData.Valid = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TSensorName[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.TempTriggerName[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) waterBoilerData.CircPumpName[j] = 0;
		waterBoilerData.ElHeatingEnabled = 0;
		waterBoilerData.ElHeatingChannel = 0xFF;

		for (byte i = 0; i<7; i++)
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

const WoodBoilerData& SettingsClass::GetDefaultWoodBoilerData(byte pos)
{
	switch (pos)
	{
	case 0:
		woodBoilerData.Valid = 1;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.Name[j] = 0;
		strlcpy(woodBoilerData.Name, "default", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.TSensorName[j] = 0;
		strlcpy(woodBoilerData.TSensorName, "katilas", MAX_NAME_LENGTH);
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.KTypeName[j] = 0;
		strlcpy(woodBoilerData.KTypeName, "dumu_temp", MAX_NAME_LENGTH);
		woodBoilerData.LadomatChannel = 17;
		woodBoilerData.ExhaustFanChannel = 18;
		woodBoilerData.WorkingTemp = 8000;
		woodBoilerData.WorkingHisteresis = 200;
		woodBoilerData.LadomatTemp = 6000;
		strlcpy(woodBoilerData.LadomatTempTriggerName, "ladomat", MAX_NAME_LENGTH);
		break;
	default:
		woodBoilerData.Valid = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.TSensorName[j] = 0;
		for (byte j = 0; j < MAX_NAME_LENGTH; j++) woodBoilerData.KTypeName[j] = 0;
		woodBoilerData.LadomatChannel = 0xff;
		woodBoilerData.ExhaustFanChannel = 0xff;
		woodBoilerData.WorkingTemp = 7500;
		woodBoilerData.WorkingHisteresis = 200;
		break;
	}

	return woodBoilerData;
}


void SettingsClass::convert()
{
	Log.debug("Converting config file with default values.");
	
	create_structure();

	Log.debug("Writing new data...");
	//Lets go :)
	{
		//Switches - nothing to read

		//write 3 built in switches
		setSwitchData(SWITCH_NAME_WINTER_MODE, GetDefaultSwitchData(SWITCH_NAME_WINTER_MODE));
		EEPROM.SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_WINTER_MODE], (byte)STATUS_ON);

		setSwitchData(SWITCH_NAME_TEMPERATURE_MODE, GetDefaultSwitchData(SWITCH_NAME_TEMPERATURE_MODE));
		EEPROM.SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_TEMPERATURE_MODE], (byte)STATUS_ON);

		setSwitchData(SWITCH_NAME_HOT_WATER_MODE, GetDefaultSwitchData(SWITCH_NAME_HOT_WATER_MODE));
		EEPROM.SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_HOT_WATER_MODE], (byte)STATUS_OFF);

		setSwitchData(SWITCH_NAME_BURN_MODE, GetDefaultSwitchData(SWITCH_NAME_BURN_MODE));
		EEPROM.SetByte(EEPROM_POS_SWITCH[SWITCH_NAME_BURN_MODE], (byte)STATUS_ON);
	}

	//TempSensors
	{
		setTempSensorData(0, GetDefaultTSensorData(0));
		setTempSensorData(1, GetDefaultTSensorData(1));
		setTempSensorData(2, GetDefaultTSensorData(2));
		setTempSensorData(3, GetDefaultTSensorData(3));
		setTempSensorData(4, GetDefaultTSensorData(4));
		setTempSensorData(5, GetDefaultTSensorData(5));
		setTempSensorData(6, GetDefaultTSensorData(6));
		setTempSensorData(7, GetDefaultTSensorData(7));
		setTempSensorData(8, GetDefaultTSensorData(8));
		setTempSensorData(9, GetDefaultTSensorData(9));
		setTempSensorData(10, GetDefaultTSensorData(10));
		setTempSensorData(11, GetDefaultTSensorData(11));
		setTempSensorData(12, GetDefaultTSensorData(12));
		setTempSensorData(13, GetDefaultTSensorData(13));
		setTempSensorData(14, GetDefaultTSensorData(14));
		setTempSensorData(15, GetDefaultTSensorData(15));
		setTempSensorData(16, GetDefaultTSensorData(16));
		setTempSensorData(17, GetDefaultTSensorData(17));
		setTempSensorData(18, GetDefaultTSensorData(18));
	}

	//RelayModule - nothing to do, skipping to next records
	{

			setRelayModuleData(0, GetDefaultRelayModuleData(0));
			setRelayModuleData(1, GetDefaultRelayModuleData(1));
			setRelayModuleData(2, GetDefaultRelayModuleData(2));
			setRelayModuleData(3, GetDefaultRelayModuleData(3));
			setRelayModuleData(4, GetDefaultRelayModuleData(4));

	}
	//TempTriggers
	{

			setTempTriggerData(0, GetDefaultTempTriggerData(0));
			setTempTriggerData(1, GetDefaultTempTriggerData(1));
			setTempTriggerData(2, GetDefaultTempTriggerData(2));
			setTempTriggerData(3, GetDefaultTempTriggerData(3));
	}

	//CircPumps
	{
			setCircPumpData(0, GetDefaultCircPumpData(0));
			setCircPumpData(1, GetDefaultCircPumpData(1));
			setCircPumpData(2, GetDefaultCircPumpData(2));
	}

	//CollectorsData
	{
		/*
		Name: 1aukstas CircPumpName: grindu Valve count:10
        Valve 1 ValveType: 0 Channel: 1
        Valve 2 ValveType: 0 Channel: 2
        Valve 3 ValveType: 0 Channel: 3
        Valve 4 ValveType: 0 Channel: 4
        Valve 5 ValveType: 0 Channel: 5
        Valve 6 ValveType: 0 Channel: 6
        Valve 7 ValveType: 0 Channel: 7
        Valve 8 ValveType: 0 Channel: 8
        Valve 9 ValveType: 0 Channel: 9
        Valve 10 ValveType: 0 Channel: 10
        Valve 11 ValveType: 0 Channel: 0
        Valve 12 ValveType: 0 Channel: 0
        Valve 13 ValveType: 0 Channel: 0
        Valve 14 ValveType: 0 Channel: 0
        Valve 15 ValveType: 0 Channel: 0
        Valve 16 ValveType: 0 Channel: 0
		*/

		
		setCollectorsData(0, GetDefaultCollectorData(0));
		setCollectorsData(1, GetDefaultCollectorData(1));
	}

	//ComfortZonesData
	{
	/*
		*****************************************************
		Comfort zones
		*****************************************************
		Name: svetaine_maza Channel: 1 Col name: 1aukstas Room sensor: svetaine_maza Floor sensor:  Temperature: 2100 Histeresis: 100
		Name: svetaine_didele Channel: 3 Col name: 1aukstas Room sensor: svetaine_didele Floor sensor:  Temperature: 2100 Histeresis: 100
		Name: darbo_kambarys Channel: 2 Col name: 1aukstas Room sensor: darbo_kambarys Floor sensor:  Temperature: 2100 Histeresis: 100
		Name: sveciu_kambarys Channel: 5 Col name: 1aukstas Room sensor: sveciu_kamb Floor sensor:  Temperature: 2000 Histeresis: 100
		Name: skalbykla Channel: 8 Col name: 1aukstas Room sensor: svabrine Floor sensor:  Temperature: 2000 Histeresis: 100
		Name: koridorius Channel: 9 Col name: 1aukstas Room sensor: koridorius Floor sensor:  Temperature: 2100 Histeresis: 100
		Name: tualetas Channel: 10 Col name: 1aukstas Room sensor: tualetas Floor sensor:  Temperature: 2100 Histeresis: 100
		Name: virtuve Channel: 6 Col name: 1aukstas Room sensor: virtuve Floor sensor:  Temperature: 2000 Histeresis: 100
	*/
		int idx = 0;
		ComfortZonesData data;
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: svetaine_didele Channel: 3 Col name: 1aukstas Room sensor: svetaine_didele Floor sensor:  Temperature: 2100 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: darbo_kambarys Channel: 2 Col name: 1aukstas Room sensor: darbo_kambarys Floor sensor:  Temperature: 2100 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: sveciu_kambarys Channel: 5 Col name: 1aukstas Room sensor: sveciu_kamb Floor sensor:  Temperature: 2000 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: skalbykla Channel: 8 Col name: 1aukstas Room sensor: svabrine Floor sensor:  Temperature: 2000 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: koridorius Channel: 9 Col name: 1aukstas Room sensor: koridorius Floor sensor:  Temperature: 2100 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: tualetas Channel: 10 Col name: 1aukstas Room sensor: tualetas Floor sensor:  Temperature: 2100 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: virtuve Channel: 6 Col name: 1aukstas Room sensor: virtuve Floor sensor:  Temperature: 2000 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if(data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}
		
		if(data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: rusys1 Channel: 255 Col name: rusys Room sensor: rusys1 Floor sensor:  Temperature: 2000 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if (data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}

		if (data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;

		//Name: rusys2 Channel: 255 Col name: rusys Room sensor: rusys2 Floor sensor:  Temperature: 2000 Histeresis: 100
		data = GetDefaultComfortZoneData(idx);

		if (data.room_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_ROOM_SENSOR[idx], true);
		}

		if (data.floor_sensor[0] != 0)
		{
			EEPROM.SetBool(EEPROM_POS_COMFORT_ZONE_FLOOR_SENSOR[idx], true);
		}

		setComfortZonesData(idx, data);
		idx++;
		
	}

	//KTypeData
	{		
		setKTypeData(0, GetDefaultKTypeData(0));
	}

	//WaterBoiler - set default values or do nothing?
	{
		setWaterBoilerData(0, GetDefaultWaterBoilerData(0));
	}

	//Katilas
	{		
		//write
		setWoodBoilerData(0, GetDefaultWoodBoilerData(0));
	}
	Log.debug("Conversion done.");
}


SettingsClass Settings;