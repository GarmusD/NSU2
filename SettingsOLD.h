struct DataAddressesOLD
{
	unsigned long int MyAddr;
	unsigned long int SwitchAddr;
	unsigned long int TSensorsAddr;
	unsigned long int RelayAddr;
	unsigned long int TempTriggersAddr;
	unsigned long int CirculationPumpsAddr;
	unsigned long int CollectorsAddr;
	unsigned long int ComfortZonesAddr;
	unsigned long int KTypeAddr;
	unsigned long int BoilerisAddr;
	unsigned long int KatilasAddr;
};

///////////////////////////////////////////////////////////////////////////////////

struct TSensorsDataOLD
{
	byte Valid;
	SensorID sid;
	int interval;
};

struct SwitchDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char DependOn[MAX_NAME_LENGTH];
	byte depstate;
	byte forcestate;
};

struct RelayModuleDataOLD
{
	byte Valid;
	byte ActiveLow;
	byte Inverted;
};

struct TempTriggerPieceDataOLD
{
	byte Valid;
	char SensorName[MAX_NAME_LENGTH];
	byte Condition;
	int Temperature;
	int Histeresis;
};

struct TempTriggerDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	TempTriggerPieceDataOLD ttpiece[4];
};

struct CircPumpDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	byte Channel;
	byte MaxSpeed;
	byte Spd1Channel;
	byte Spd2Channel;
	byte Spd3Channel;
};

struct CollectorsDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char CircPump[MAX_NAME_LENGTH];
	byte valve_count;
	ThermoValve valves[MAX_COLLECTOR_VALVES];
};

struct ComfortZonesDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	char room_sensor[MAX_NAME_LENGTH];
	char floor_sensor[MAX_NAME_LENGTH];
	char col_name[MAX_NAME_LENGTH];
	int room_temp_hi;
	int room_temp_low; //
	int floor_temp_hi; //
	int floor_temp_low; //
	int histeresis;
	byte channel;
};

struct KTypeDataOLD
{
	byte Valid;
	char Name[MAX_NAME_LENGTH];
	int interval;
};

struct WaterBoilerDataOLD
{
	byte Valid;
	char TSensorName[MAX_NAME_LENGTH];
	char TempTriggerName[MAX_NAME_LENGTH];
	char CircPumpName[MAX_NAME_LENGTH];
	byte ElPowerChannel;
	ElHeatingData PowerData[7];
};

struct WoodBoilerDataOLD
{
	byte Valid;
	char TSensorName[MAX_NAME_LENGTH];
	char KTypeName[MAX_NAME_LENGTH];
	byte LadomatChannel;
	byte SmokeFanChannel;
	int WorkingTemp;
	int WorkingHisteresis;
	int LadomatTemp; //<- nauja
	char LadomatTempTriggerName[MAX_NAME_LENGTH]; // <- nauja
};